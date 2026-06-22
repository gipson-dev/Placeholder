#include "ui/PreviewWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <algorithm>

#include "core/VariableResolver.h"

PreviewWidget::PreviewWidget(QWidget* parent)
    : QWidget(parent),
      template_(LabelTemplate::defaultTemplate())
{
    setMinimumSize(420, 260);
    setMouseTracking(true);
}

void PreviewWidget::setTemplate(const LabelTemplate& labelTemplate)
{
    template_ = labelTemplate;
    update();
}

void PreviewWidget::setVariables(const VariableContext& context)
{
    variables_ = context;
    update();
}

void PreviewWidget::setSelectedElement(int index)
{
    selectedElement_ = index;
    update();
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(238, 241, 245));

    QRectF label = labelRect();
    painter.setPen(QPen(QColor(210, 210, 210), 1));
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(label, 8, 8);
    drawGrid(painter, label);

    for (int i = 0; i < static_cast<int>(template_.elements.size()); ++i)
    {
        const LabelElement& element = template_.elements[i];
        bool selected = i == selectedElement_;
        if (element.type == LabelElementType::Text)
        {
            drawTextElement(painter, element, label, selected);
        }
        else if (element.type == LabelElementType::QrCode)
        {
            drawQrElement(painter, element, label, selected);
        }
        else
        {
            drawBarcodeElement(painter, element, label, selected);
        }
    }
}

void PreviewWidget::mousePressEvent(QMouseEvent* event)
{
    int hit = hitTest(event->position());
    selectedElement_ = hit;
    emit elementSelected(hit);
    if (hit >= 0)
    {
        QRectF label = labelRect();
        QPointF labelPoint = widgetToLabel(event->position(), label);
        const LabelElement& element = template_.elements[hit];
        dragOffsetInches_ = QPointF(labelPoint.x() - element.xInches, labelPoint.y() - element.yInches);
        draggingElement_ = hit;
    }
    update();
}

void PreviewWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (draggingElement_ < 0 || draggingElement_ >= static_cast<int>(template_.elements.size()))
    {
        return;
    }

    QRectF label = labelRect();
    QPointF labelPoint = widgetToLabel(event->position(), label) - dragOffsetInches_;
    LabelElement& element = template_.elements[draggingElement_];
    element.xInches = std::max(0.0, labelPoint.x());
    element.yInches = std::max(0.0, labelPoint.y());
    emit elementMoved(draggingElement_, element.xInches, element.yInches);
    update();
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent*)
{
    draggingElement_ = -1;
}

QRectF PreviewWidget::labelRect() const
{
    const double aspect = template_.settings.labelWidthInches / template_.settings.labelHeightInches;
    QRectF available = rect().adjusted(28, 28, -28, -28);
    double width = available.width();
    double height = width / aspect;
    if (height > available.height())
    {
        height = available.height();
        width = height * aspect;
    }
    return QRectF(available.center().x() - width / 2.0, available.center().y() - height / 2.0, width, height);
}

QRectF PreviewWidget::elementRect(const LabelElement& element, const QRectF& label) const
{
    QPointF topLeft = labelToWidget(element.xInches, element.yInches, label);
    double scaleX = label.width() / template_.settings.labelWidthInches;
    double scaleY = label.height() / template_.settings.labelHeightInches;

    if (element.type == LabelElementType::Text)
    {
        double width = std::max(36.0, element.boxWidthInches * scaleX);
        double lineHeight = std::max(14.0, element.fontHeightDots / static_cast<double>(template_.settings.dpi) * scaleY);
        int lines = element.wrap || element.multiLine ? element.maxLines : 1;
        return QRectF(topLeft, QSizeF(width, lineHeight * lines + 4));
    }

    if (element.type == LabelElementType::QrCode)
    {
        double size = std::max(36.0, element.qrMagnification * 10.0);
        return QRectF(topLeft, QSizeF(size, size));
    }

    double width = std::max(80.0, static_cast<double>(VariableResolver::elementValue(element, variables_).size() * element.barcodeModuleWidth * 4));
    double height = element.barcodeHeightDots / static_cast<double>(template_.settings.dpi) * scaleY;
    return QRectF(topLeft, QSizeF(width, std::max(24.0, height)));
}

QPointF PreviewWidget::labelToWidget(double xInches, double yInches, const QRectF& label) const
{
    return QPointF(
        label.left() + xInches / template_.settings.labelWidthInches * label.width(),
        label.top() + yInches / template_.settings.labelHeightInches * label.height());
}

QPointF PreviewWidget::widgetToLabel(const QPointF& point, const QRectF& label) const
{
    return QPointF(
        (point.x() - label.left()) / label.width() * template_.settings.labelWidthInches,
        (point.y() - label.top()) / label.height() * template_.settings.labelHeightInches);
}

int PreviewWidget::hitTest(const QPointF& point) const
{
    QRectF label = labelRect();
    for (int i = static_cast<int>(template_.elements.size()) - 1; i >= 0; --i)
    {
        if (elementRect(template_.elements[i], label).contains(point))
        {
            return i;
        }
    }
    return -1;
}

void PreviewWidget::drawGrid(QPainter& painter, const QRectF& label) const
{
    painter.save();
    painter.setPen(QPen(QColor(226, 232, 240), 1, Qt::DotLine));
    for (double x = 0.25; x < template_.settings.labelWidthInches; x += 0.25)
    {
        double wx = label.left() + x / template_.settings.labelWidthInches * label.width();
        painter.drawLine(QPointF(wx, label.top()), QPointF(wx, label.bottom()));
    }
    for (double y = 0.25; y < template_.settings.labelHeightInches; y += 0.25)
    {
        double wy = label.top() + y / template_.settings.labelHeightInches * label.height();
        painter.drawLine(QPointF(label.left(), wy), QPointF(label.right(), wy));
    }
    painter.restore();
}

void PreviewWidget::drawTextElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const
{
    QRectF box = elementRect(element, label);
    QString value = QString::fromStdString(VariableResolver::elementValue(element, variables_));
    double pointSize = std::max(8.0, element.fontHeightDots / 4.0);

    QFont font("Arial", static_cast<int>(pointSize));
    font.setBold(element.bold);
    font.setItalic(element.italic);
    font.setUnderline(element.underline);

    painter.save();
    painter.setFont(font);
    painter.setPen(Qt::black);

    int flags = Qt::AlignTop;
    if (element.alignment == TextAlignment::Center) flags |= Qt::AlignHCenter;
    else if (element.alignment == TextAlignment::Right) flags |= Qt::AlignRight;
    else flags |= Qt::AlignLeft;
    if (element.wrap || element.multiLine) flags |= Qt::TextWordWrap;

    QRectF textBox = box.adjusted(0, -2, 0, 0);
    painter.drawText(textBox, flags, value);
    painter.setPen(QPen(selected ? QColor(0, 120, 215) : QColor(120, 148, 180), 1, selected ? Qt::SolidLine : Qt::DashLine));
    painter.drawRect(box);
    painter.restore();
}

void PreviewWidget::drawBarcodeElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const
{
    QRectF box = elementRect(element, label);
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    int module = std::max(2, element.barcodeModuleWidth * 2);
    for (int x = static_cast<int>(box.left()); x < static_cast<int>(box.right()); x += module * 3)
    {
        painter.drawRect(QRectF(x, box.top(), module, box.height()));
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(selected ? QColor(0, 120, 215) : QColor(70, 130, 180), 1));
    painter.drawRect(box);
    if (element.humanReadable)
    {
        painter.setPen(Qt::black);
        painter.drawText(QRectF(box.left(), box.bottom() + 3, box.width(), 20), Qt::AlignCenter, QString::fromStdString(VariableResolver::elementValue(element, variables_)));
    }
    painter.restore();
}

void PreviewWidget::drawQrElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const
{
    QRectF box = elementRect(element, label);
    painter.save();
    painter.setPen(QPen(selected ? QColor(0, 120, 215) : Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(box);
    painter.drawLine(box.topLeft(), box.bottomRight());
    painter.drawLine(box.topRight(), box.bottomLeft());
    painter.drawText(box, Qt::AlignCenter, "QR");
    painter.restore();
}
