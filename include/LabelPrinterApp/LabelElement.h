#pragma once

#include <string>

enum class LabelElementType
{
    Text,
    Barcode
};

struct LabelElement
{
    LabelElementType type = LabelElementType::Text;
    std::string text;

    int x = 20;
    int y = 20;

    int fontHeight = 30;
    int fontWidth = 30;
    int barcodeHeight = 60;

    bool bold = false;

    LabelElement() = default;

    static LabelElement Text(
        const std::string& textValue,
        int xValue,
        int yValue,
        int heightValue,
        int widthValue,
        bool boldValue = false
    )
    {
        LabelElement element;
        element.type = LabelElementType::Text;
        element.text = textValue;
        element.x = xValue;
        element.y = yValue;
        element.fontHeight = heightValue;
        element.fontWidth = widthValue;
        element.bold = boldValue;
        return element;
    }

    static LabelElement Barcode(
        const std::string& value,
        int xValue,
        int yValue,
        int heightValue
    )
    {
        LabelElement element;
        element.type = LabelElementType::Barcode;
        element.text = value;
        element.x = xValue;
        element.y = yValue;
        element.barcodeHeight = heightValue;
        return element;
    }
};
