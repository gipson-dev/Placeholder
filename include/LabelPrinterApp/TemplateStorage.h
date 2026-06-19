#pragma once
#include "LabelTemplate.h"

class TemplateStorage
{
public:
    static LabelTemplate LoadDefaultTemplate()
    {
        LabelTemplate label;
        label.name = "Default Label";
        label.labelWidthDots = 400;
        label.labelHeightDots = 240;

        label.addElement(LabelElement::Text("ITEM: 123456", 30, 30, 35, 35, true));
        label.addElement(LabelElement::Text("PART A-100", 30, 80, 30, 30));
        label.addElement(LabelElement::Text("QTY: 25", 30, 125, 30, 30));
        label.addElement(LabelElement::Barcode("123456", 30, 170, 50));

        return label;
    }
};
