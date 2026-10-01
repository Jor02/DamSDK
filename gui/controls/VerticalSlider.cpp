#pragma once
#include "VerticalSlider.h"
#include "damsdk/gui/platform/windows/OffscreenGDIDrawingContext.h"
#include "damsdk/gui/platform/windows/Bitmap.h"
#include "damsdk/gui/platform/windows/Window.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // FUNCTION: DELAYLAMA 0x10009a40
    VerticalSlider::VerticalSlider(RECT *pRect, ControlListener* listener, int parameterId, int minValue, int maxValue, Platform::Windows::Bitmap *handleBmp, Platform::Windows::Bitmap *backgroundBmp, POINT* offset, int flags) : Control(pRect, listener, parameterId, backgroundBmp)
    {
        this->backgroundOffset.x = offset->x;
        this->backgroundOffset.y = offset->y;
        this->handlePos.x = 0;
        this->handlePos.y = 0;
        this->flags = flags;
        this->handleImage = handleBmp;
        this->trackMinY = minValue;
        this->trackMaxY = maxValue;
        this->snapToMouse = true;
        this->isHandleTransparent = true;
        if (handleBmp == nullptr) {
            this->handleWidth = 1;
            this->handleHeight = 1;
        }
        else {
            handleBmp->remember();
            this->handleWidth = this->handleImage->width;
            this->handleHeight = this->handleImage->height;
        }
        this->trackWidth = this->rect.right - this->rect.left;
        this->trackTopY = maxValue;
        this->trackHeight = this->rect.bottom - this->rect.top;
        this->handlePos.x = 0;
        this->handlePos.y = 0;
        this->fineTuneDivider = 10.0f;
        this->handleMinPos = minValue - this->rect.top;
        this->handleMaxPos = (this->handleHeight - this->rect.top) + maxValue;
    }

    // FUNCTION: DELAYLAMA 0x10009bf0
    VerticalSlider::~VerticalSlider() {
        if (this->handleImage != nullptr) {
          this->handleImage->unregisterBitmap();
        }
    }

     // FUNCTION: DELAYLAMA 0x10009c50
     void VerticalSlider::onDraw(Platform::Windows::GDIDrawingContext* drawingContext) {
        // Same steps as VSTGUI's CSlider::draw, always through an offscreen context.
        float value;
        if (this->flags & 0x20)
            value = this->value;
        else
            value = 1.0f - this->value;

        Platform::Windows::OffscreenGDIDrawingContext* offscreen =
            new Platform::Windows::OffscreenGDIDrawingContext(this->parent, this->trackWidth, this->trackHeight, Platform::Windows::kBlackColor);

        // Background
        RECT rect = {0, 0, this->trackWidth, this->trackHeight};
        if (this->bitmap) {
            if (this->useAlphaBlending)
                this->bitmap->drawMasked(offscreen, &rect, &this->backgroundOffset);
            else
                this->bitmap->blit(offscreen, &rect, &this->backgroundOffset);
        }

        // Handle position
        RECT handleRect;
        handleRect.left = this->handlePos.x;
        handleRect.right = handleRect.left + this->handleWidth;
        handleRect.top = this->handlePos.y + (int)(value * (this->trackMaxY - this->trackMinY));
        if (handleRect.top < this->handleMinPos)
            handleRect.top = this->handleMinPos;
        handleRect.bottom = handleRect.top + this->handleHeight;
        if (handleRect.bottom > this->handleMaxPos)
            handleRect.bottom = this->handleMaxPos;

        if (this->handleImage) {
            if (this->isHandleTransparent) {
                POINT zero = {0, 0};
                this->handleImage->drawMasked(offscreen, &handleRect, &zero);
            }
            else {
                POINT zero = {0, 0};
                this->handleImage->blit(offscreen, &handleRect, &zero);
            }
        }

        offscreen->copyToScreen(drawingContext, this->rect, Platform::Windows::Point(0, 0));
        delete offscreen;

        this->trackTopY = this->rect.top + handleRect.top;
        this->setDirty(false);
    }

    // FUNCTION: DELAYLAMA 0x10009e10
    void VerticalSlider::onMouseDown(Platform::Windows::GDIDrawingContext* drawingContext, POINT* mousePos) {
        // Same steps as VSTGUI's CSlider::mouse
        if (!this->isEnabled)
            return;

        uint32_t button = drawingContext->getMouseButtons();

        // Ctrl + click: reset to the default value
        if (button == (kLButton | kControl)) {
            this->value = this->getDefaultValue();
            if (this->isDirty())
                this->listener->valueChanged(drawingContext, this);
            return;
        }

        // Left button only
        if (!(button & kLButton))
            return;

        int delta = this->trackMinY;
        if (!this->snapToMouse) {
            // The click must be on the handle
            RECT handleRect;
            handleRect.left = this->rect.left;
            handleRect.top = this->trackTopY;
            handleRect.right = handleRect.left + this->handleWidth;
            handleRect.bottom = handleRect.top + this->handleHeight;
            if (mousePos->x < handleRect.left || mousePos->x > handleRect.right ||
                mousePos->y < handleRect.top || mousePos->y > handleRect.bottom)
                return;
            delta += mousePos->y - handleRect.top;
        }
        else {
            delta += this->handleHeight / 2 - 1;
        }

        float range = (float)(this->trackMaxY - this->trackMinY);
        float oldValue = this->value;
        uint32_t oldButton = button;

        this->parent->beginEdit(this->parameterId);
        while (1) {
            button = drawingContext->getMouseButtons();
            if (!(button & kLButton))
                break;

            if (oldButton != button && (button & kShift)) {
                oldValue = this->value;
                oldButton = button;
            }
            else if (!(button & kShift)) {
                oldValue = this->value;
            }

            this->value = (float)(mousePos->y - delta) / range;
            if (this->flags & 0x40)
                this->value = 1.0f - this->value;
            if (button & kShift)
                this->value = (this->value - oldValue) / this->fineTuneDivider + oldValue;

            this->clampValue();
            if (this->isDirty())
                this->listener->valueChanged(drawingContext, this);

            drawingContext->getRelativeMousePos(mousePos);
            this->onIdle();
        }
        this->parent->endEdit(this->parameterId);
    }
}
}
}