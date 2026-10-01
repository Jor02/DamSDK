#pragma once
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include <cmath>
#include "RotaryControl.h"
#include "damsdk/api/EditorBase.h"
#include "damsdk/gui/platform/windows/Window.h"

namespace DamSDK {
namespace Gui {
namespace Controls {
    // FUNCTION: DELAYLAMA 0x10008e40
    RotaryControl::RotaryControl(RECT* pRect, ControlListener* listener, int parameterId, Platform::Windows::Bitmap* bmp1, Platform::Windows::Bitmap* bmp2, POINT* srcPoint) : Control(pRect, listener, parameterId, bmp1) {
        this->srcPoint.x = srcPoint->x;
        LONG y = srcPoint->y;
        this->bmp = bmp2;
        this->srcPoint.y = y;
        if (bmp2 == nullptr) {
            this->knobRadius = 3;
        }
        else {
            bmp2->remember();
            int result = static_cast<int>(bmp2->width * 0.5f + 2.5f);
            this->knobRadius = result;
        }

        this->indicatorShadowColor = Platform::Windows::kGreyColor;
        this->indicatorHighlightColor = Platform::Windows::kWhiteColor;

        int right = pRect->right;
        int left = pRect->left;

        this->totalRange = 1.0f;
        this->center = (float)(right - left) * 0.5f;
        setStartAngle(3.9269907f);
        setTotalRange(-4.712389f);
        this->fineTuneDivider = 1.5;
    }

    // FUNCTION: DELAYLAMA 0x10008f80
    RotaryControl::~RotaryControl() {
        Platform::Windows::Bitmap* bmp = this->bmp;
        if (bmp != nullptr) {
            bmp->unregisterBitmap();
        }
    }

    // FUNCTION: DELAYLAMA 0x10008fe0
    void RotaryControl::onDraw(Platform::Windows::GDIDrawingContext *drawingContext) {
        if (this->bitmap) {
            if (this->useAlphaBlending)
                this->bitmap->drawMasked(drawingContext, &this->rect, &this->srcPoint);
            else
                this->bitmap->blit(drawingContext, &this->rect, &this->srcPoint);
        }
        this->drawIndicator(drawingContext);
        this->setDirty(false);
    }

    // FUNCTION: DELAYLAMA 0x10009030
    void RotaryControl::drawIndicator(Platform::Windows::GDIDrawingContext* drawingContext) {
        // Same steps as VSTGUI's CKnob::drawHandle
        POINT where = {0, 0};
        this->calculateXYFromValue(&where);

        if (this->bmp) {
            // Draw the handle bitmap centred on the handle position
            long width = this->bmp->width;
            long height = this->bmp->height;
            where.x += this->rect.left - width / 2;
            where.y += this->rect.top - height / 2;
            RECT handleRect = {where.x, where.y, where.x + width, where.y + height};
            POINT zero = {0, 0};
            this->bmp->drawMasked(drawingContext, &handleRect, &zero);
        }
        else {
            // Draw the indicator line from the handle to the centre, with a shadow
            POINT origin;
            origin.x = (this->rect.right - this->rect.left) / 2;
            origin.y = (this->rect.bottom - this->rect.top) / 2;
            where.x += this->rect.left - 1;
            where.y += this->rect.top;
            origin.x += this->rect.left - 1;
            origin.y += this->rect.top;
            drawingContext->setPenColor(this->indicatorShadowColor);
            drawingContext->moveToEx(&where);
            drawingContext->lineTo(&origin);

            where.x += 1;
            where.y -= 1;
            origin.x += 1;
            origin.y -= 1;
            drawingContext->setPenColor(this->indicatorHighlightColor);
            drawingContext->moveToEx(&where);
            drawingContext->lineTo(&origin);
        }
    }

    // FUNCTION: DELAYLAMA 0x10009190
    void RotaryControl::onMouseDown(Platform::Windows::GDIDrawingContext *drawingContext, POINT *mousePos) {
        // Same steps as VSTGUI's CKnob::mouse
        if (!this->isEnabled)
            return;

        uint32_t button = drawingContext->getMouseButtons();
        if (!(button & kLButton))
            return;

        // Ctrl + click: reset to the default value
        if (button == (kLButton | kControl)) {
            this->value = this->getDefaultValue();
            if (this->isDirty())
                this->listener->valueChanged(drawingContext, this);
            return;
        }

        float old = this->prevValue;
        POINT firstPoint = {0, 0};
        bool modeLinear = false;
        float entryState = this->value;
        float middle = (this->max - this->min) * 0.5f;
        float range = 200.0f;
        float coef = (this->max - this->min) / range;
        uint32_t oldButton = button;

        // Linear mode when the knob mode is linear, or when Alt is held (inverted)
        int mode = 0;
        int newMode = Api::GLOBAL_KNOB_MODE;
        if (newMode == 2) {
            if (!(button & kAlt))
                mode = newMode;
        }
        else if (button & kAlt) {
            mode = 2;
        }

        if (mode == 2 && (button & kLButton)) {
            if (button & kShift)
                range *= this->fineTuneDivider;
            firstPoint = *mousePos;
            modeLinear = true;
            coef = (this->max - this->min) / range;
        }
        else {
            POINT where2 = *mousePos;
            where2.x -= this->rect.left;
            where2.y -= this->rect.top;
            old = this->calculateAngleFromPoint(&where2);
        }

        POINT oldWhere = {-1, -1};
        this->parent->beginEdit(this->parameterId);
        do {
            button = drawingContext->getMouseButtons();
            if (mousePos->x != oldWhere.x || mousePos->y != oldWhere.y) {
                oldWhere = *mousePos;
                if (modeLinear) {
                    long diff = (firstPoint.y - mousePos->y) + (mousePos->x - firstPoint.x);
                    if (oldButton != button) {
                        range = 200.0f;
                        if (button & kShift)
                            range *= this->fineTuneDivider;
                        float coef2 = (this->max - this->min) / range;
                        entryState += diff * (coef - coef2);
                        coef = coef2;
                        oldButton = button;
                    }
                    this->value = entryState + diff * coef;
                    this->clampValue();
                }
                else {
                    mousePos->x -= this->rect.left;
                    mousePos->y -= this->rect.top;
                    this->value = this->calculateAngleFromPoint(mousePos);
                    if (old - this->value > middle)
                        this->value = this->max;
                    else if (this->value - old > middle)
                        this->value = this->min;
                    else
                        old = this->value;
                }
                if (this->isDirty())
                    this->listener->valueChanged(drawingContext, this);
            }
            drawingContext->getRelativeMousePos(mousePos);
            this->onIdle();
        } while (button & kLButton);

        this->parent->endEdit(this->parameterId);
    }

    // FUNCTION: DELAYLAMA 0x10009470
    void RotaryControl::setStartAngle(float startAngle) {
        this->startAngle = startAngle;
        updateMathConstants();
    }

    // FUNCTION: DELAYLAMA 0x10009480
    void RotaryControl::setTotalRange(float totalRange) {
        this->totalRange = totalRange;
        updateMathConstants();
    }

    // FUNCTION: DELAYLAMA 0x10009490
    void RotaryControl::updateMathConstants() {
        this->angleRange = (this->max - this->min) / this->totalRange;
        this->angleOffset = this->min - this->angleRange * this->startAngle;
        this->deadZoneSize = (6.2831855f - (float)fabs(this->totalRange)) * 0.5f;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x100094d0
    void RotaryControl::calculateXYFromValue(POINT* outPoint) {
        float angle = (this->value - this->angleOffset) / this->angleRange;

        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        int radius = static_cast<int>(this->knobRadius);

        // X coordinate: center - radius * cos(angle) + 0.5f
        float tempX = (this->center - static_cast<float>(radius) * cosAngle) + 0.5f;
        outPoint->x = static_cast<LONG>(tempX);

        // Y coordinate: center - radius * sin(angle) + 0.5f
        float tempY = (this->center - static_cast<float>(radius) * sinAngle) + 0.5f;
        outPoint->y = static_cast<LONG>(tempY);
    }

    // FUNCTION: DELAYLAMA 0x10009530
    float RotaryControl::calculateAngleFromPoint(POINT* point) {

        float rawAngle = atan2(this->center - point->y, point->x - this->center);
        if (rawAngle < 0.0f) {
            rawAngle = rawAngle + 6.2831855f;
        }
        
        rawAngle = rawAngle - this->startAngle;
        float normalizedAngle;
        float rangePlusDeadzone;
        if (this->totalRange < 0.0f) {
            rawAngle = rawAngle - this->totalRange;
            normalizedAngle = rawAngle;
            if (rawAngle < 0.0f) {
                normalizedAngle = rawAngle + 6.2831855f;
            }
            else if (rawAngle > 6.283185307179586) {
                normalizedAngle = rawAngle - 6.2831855f;
            }

            rangePlusDeadzone = this->deadZoneSize - this->totalRange;
            if (normalizedAngle > rangePlusDeadzone) {
                return this->max;
            }
            if (normalizedAngle > -this->totalRange) {
                return this->min;
            }
            if (rawAngle > rangePlusDeadzone) {
                return (rawAngle - 6.2831855f) * this->angleRange + this->max;
            }
            if (rawAngle < -this->deadZoneSize) {
                rawAngle = rawAngle + 6.2831855f;
            }
            return rawAngle * this->angleRange + this->max;
        }

        normalizedAngle = rawAngle;
        if (rawAngle < 0.0f) {
            normalizedAngle = rawAngle + 6.2831855f;
        }
        else if (rawAngle > 6.283185307179586) {
            normalizedAngle = rawAngle - 6.2831855f;
        }

        rangePlusDeadzone = this->totalRange + this->deadZoneSize;
        if (normalizedAngle > rangePlusDeadzone) {
            return this->min;
        }
        if (normalizedAngle > this->totalRange) {
            return this->max;
        }
        if (rawAngle > rangePlusDeadzone) {
            return (rawAngle - 6.2831855f) * this->angleRange + this->min;
        }
        if (rawAngle < -this->deadZoneSize) {
            rawAngle = rawAngle + 6.2831855f;
        }
        return rawAngle * this->angleRange + this->min;
    }

    // FUNCTION: DELAYLAMA 0x100096c0
    void RotaryControl::setIndicatorShadowColor(Api::Color color) {
        this->indicatorShadowColor = color;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x100096f0
    void RotaryControl::setIndicatorHighlightColor(Api::Color color) {
        this->indicatorHighlightColor = color;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x10009720
    void RotaryControl::setBitmap(Platform::Windows::Bitmap* bmp) {
        Platform::Windows::Bitmap* oldBitmap = this->bmp;
        if (oldBitmap != nullptr) {
            oldBitmap->unregisterBitmap();
            this->bmp = nullptr;
        }
        if (bmp != nullptr) {
            this->bmp = bmp;
            bmp->remember();
            int bitmapWidth = bmp->width;
            int knobRadius = static_cast<int>(static_cast<float>(bitmapWidth) * 0.5f + 2.5f);
            this->knobRadius = knobRadius;
        }
    }

    // FUNCTION: DELAYLAMA 0x10009770
    float RotaryControl::getStartAngle() {
        return this->startAngle;
    }

    // FUNCTION: DELAYLAMA 0x10009780
    float RotaryControl::getTotalRange() {
        return this->totalRange;
    }

    // FUNCTION: DELAYLAMA 0x10009790
    void RotaryControl::setKnobRadius(int radius) {
        this->knobRadius = radius;
    }

    // FUNCTION: DELAYLAMA 0x100097a0
    void RotaryControl::setFineTuneDivider(float divider) {
        this->fineTuneDivider = divider;
    }

    // FUNCTION: DELAYLAMA 0x100097b0
    float RotaryControl::getFineTuneDivider() {
        return this->fineTuneDivider;
    }
}
}
}