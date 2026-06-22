#pragma once

#include <QWidget>

#include "core/LabelTemplate.h"
#include "core/VariableResolver.h"

class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(QWidget* parent = nullptr);

    void setTemplate(const LabelTemplate& labelTemplate);
    void setVariables(const VariableContext& context);
    void setSelectedElement(int index);

signals:
    void elementSelected(int index);
    void elementMoved(int index, double xInches, double yInches);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QRectF labelRect() const;
    QRectF elementRect(const LabelElement& element, const QRectF& label) const;
    QPointF labelToWidget(double xInches, double yInches, const QRectF& label) const;
    QPointF widgetToLabel(const QPointF& point, const QRectF& label) const;
    int hitTest(const QPointF& point) const;
    void drawGrid(QPainter& painter, const QRectF& label) const;
    void drawTextElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const;
    void drawBarcodeElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const;
    void drawQrElement(QPainter& painter, const LabelElement& element, const QRectF& label, bool selected) const;

    LabelTemplate template_;
    VariableContext variables_;
    int selectedElement_ = -1;
    int draggingElement_ = -1;
    QPointF dragOffsetInches_;
};
