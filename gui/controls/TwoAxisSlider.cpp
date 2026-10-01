#pragma once
#include "TwoAxisSlider.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include "damsdk/gui/platform/windows/Window.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // FUNCTION: DELAYLAMA 0x10004350
    TwoAxisSlider::TwoAxisSlider(RECT *bounds, ControlListener* listener, int parameterId, int minValue, int maxValue, Platform::Windows::Bitmap *handle, Platform::Windows::Bitmap *background, POINT* offset, int flags) : HorizontalSlider(bounds,listener,parameterId,minValue,maxValue,handle,background,offset,flags)
    {
        int handleHeight = this->handleHeight;
        
        int halfHandleHeight = handleHeight / 2;
        this->halfHandleHeight2 = halfHandleHeight;
        this->yTop = bounds->top;
        
        int handleMaxY = bounds->bottom - halfHandleHeight;
        this->yTrackBottom = 0;
        this->yBottom = handleMaxY;
        this->xRange = (float)((handleMaxY - bounds->top) + handleHeight);
        
        int handleWidth = this->handleWidth;
        this->xMinOffset = minValue - bounds->left;
        this->xMaxOffset = (handleWidth - bounds->left) + maxValue;
        this->xValueRange = (float)(maxValue - minValue);
        this->yValueRange = (float)(handleMaxY - this->yTop);
    }
    
    // FUNCTION: DELAYLAMA 0x10004450 FOLDED
    void TwoAxisSlider::onDraw(DamSDK::Gui::Platform::Windows::GDIDrawingContext* drawContext) {}

    // FUNCTION: DELAYLAMA 0x10004460
    void TwoAxisSlider::onMouseDown(DamSDK::Gui::Platform::Windows::GDIDrawingContext* drawContext, POINT* mousePos) {
        if (!this->isEnabled)
            return;

        uint32_t button = drawContext->getMouseButtons();
        if (!(button & kLButton))
            return;

        // 201 tells the editor that singing starts
        if (button) {
            this->value = 201.0f;
            this->listener->valueChanged(drawContext, this);
        }

        int xAnchor = this->handleWidth / 2 + this->trackMinX;
        int yAnchor = this->halfHandleHeight2 / 2 + this->yTop;

        this->parent->beginEdit(this->parameterId);
        while (drawContext->getMouseButtons()) {
            // X axis: pitch (0..1)
            this->value = (float)(mousePos->x - xAnchor) / this->xValueRange;
            this->clampValue();
            this->listener->valueChanged(drawContext, this);

            // Y axis: vowel, sent as 100..101 so the editor can tell the axes apart
            this->value = (float)(mousePos->y - yAnchor) / this->yValueRange;
            this->clampValue();
            this->value = this->value + 100.0f;
            this->listener->valueChanged(drawContext, this);

            drawContext->getRelativeMousePos(mousePos);
            this->onIdle();
        }

        // 200 tells the editor that singing stops
        this->value = 200.0f;
        this->listener->valueChanged(drawContext, this);
    }
}
}
}