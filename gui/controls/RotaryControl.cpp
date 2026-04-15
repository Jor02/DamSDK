#pragma once
#include <cmath>
#include "RotaryControl.h"
#include "damsdk/api/EditorBase.h"
#include "damsdk/gui/platform/windows/Window.h"

namespace DamSDK {
namespace Gui {
namespace Controls {
    // STUB: DELAYLAMA 0x10008e40
    RotaryControl::RotaryControl(RECT* pRect, callbackCallback callback, int parameterId, Platform::Windows::Bitmap* bmp1, Platform::Windows::Bitmap* bmp2, POINT* srcPoint) : Control(pRect, callback, parameterId, bmp1) {
        // int iVar1;
        // int iVar2;
        // undefined4 uVar3;
        // float fVar4;
        // undefined4 unaff_ESI;
        // undefined4 unaff_EDI;
        // LONG y;
        //
        // Control::Control((Control *)this,pRect,callback,parameterId,bmp1);
        // this->srcPoint = srcPoint->x;
        // y = srcPoint->y;
        // this->bmp = bmp2;
        // this->y = y;
        // this->vtable = &RotaryControlVTable_1000bd78;
        // if (bmp2 == (Bitmap *)0x0) {
        //   this->knobRadius = 4.2039e-45;
        // }
        // else {
        //   View::useBitmap(bmp2);
        //   fVar4 = (float)_ftol((double)CONCAT44(unaff_ESI,unaff_EDI));
        //   this->knobRadius = fVar4;
        // }
        // uVar3 = DAT_1000d854;
        // this->indicatorShadowColor.bytes.r = (byte)DAT_1000d854;
        // this->indicatorShadowColor.bytes.g = (byte)((uint)uVar3 >> 8);
        // this->indicatorShadowColor.bytes.b = (byte)((uint)uVar3 >> 0x10);
        // this->indicatorShadowColor.bytes.a = (byte)((uint)uVar3 >> 0x18);
        // uVar3 = DAT_FOREGROUND_COLOR;
        // this->indicatorHighlightColor.bytes.r = (byte)DAT_FOREGROUND_COLOR;
        // this->indicatorHighlightColor.bytes.g = (byte)((uint)uVar3 >> 8);
        // this->indicatorHighlightColor.bytes.b = (byte)((uint)uVar3 >> 0x10);
        // this->indicatorHighlightColor.bytes.a = (byte)((uint)uVar3 >> 0x18);
        // iVar1 = pRect->right;
        // iVar2 = pRect->left;
        // this->totalRange = 1.0;
        // this->center = (float)(iVar1 - iVar2) * FLOAT_1000bb34;
        // setStartAngle(this,3.9269907);
        // setTotalRange(this,0xc096cbe4);
        // this->fineTuneDivider = 1.5;
        // return this;
    }

    // FUNCTION: DELAYLAMA 0x10008f60
    RotaryControl::~RotaryControl() {
        this->destroy();
    }

    // FUNCTION: DELAYLAMA 0x10008f80
    void RotaryControl::destroy() {
        Platform::Windows::Bitmap* bmp = this->bmp;
        if (bmp != nullptr) {
            Platform::Windows::Bitmap::unregisterBitmap(bmp);
        }
        Control::destroy();
    }

    // FUNCTION: DELAYLAMA 0x10008fe0
    void RotaryControl::onDraw(Platform::Windows::GDIDrawingContext *drawingContext)
    {
        Platform::Windows::Bitmap *bitmap = this->bitmap;
        if (bitmap != nullptr)
        {
            RECT *destRect = &this->rect;
            POINT *srcPoint = &this->srcPoint;
            if (this->useAlphaBlending == false)
            {
                bitmap->blit(drawingContext, destRect, srcPoint);
            }
            else
            {
                bitmap->drawMasked(drawingContext, destRect, srcPoint);
            }
        }
        this->drawIndicator(drawingContext);
        this->setDirty(false);
    }

    // STUB: DELAYLAMA 0x10009030
    void RotaryControl::drawIndicator(Platform::Windows::GDIDrawingContext* drawingContext) {
        // int centerX;
        // int centerY;
        // POINT sourceOffset;
        // RECT destRect;
        // GDIDrawingContext *drawContext;
        // Bitmap *knobBitmap;
        //
        // centerY = 0;
        // sourceOffset.x = 0;
        // (*this->vtable->calculateXYFromValue)(&centerY);
        // knobBitmap = this->bmp;
        //                   // BITMAP RENDERING
        // if (knobBitmap != (Bitmap *)0x0) {
        //   destRect.left =
        //        centerX + (this->rect.left - (int)knobBitmap->width / 2);
        //   centerY = centerY + (this->rect.top - (int)knobBitmap->height / 2)
        //   ;
        //   destRect.bottom = centerY + knobBitmap->height;
        //   destRect.right = knobBitmap->width + destRect.left;
        //   sourceOffset.x = 0;
        //   sourceOffset.y = 0;
        //   destRect.top = centerY;
        //   Bitmap::drawMasked(knobBitmap,drawContext,&destRect,&sourceOffset);
        //   return;
        // }
        //                   // VECTOR RENDERING (Fallback)
        // sourceOffset.y = this->rect.top;
        // sourceOffset.x =
        //      (this->rect.right -
        //      this->rect.left) / 2 + -1 +
        //      this->rect.left;
        // centerY = centerY + sourceOffset.y;
        // sourceOffset.y =
        //      (this->rect.bottom - sourceOffset.y) / 2 + sourceOffset.y;
        // GDIDrawingContext::setPenColor(drawContext,this->indicatorShadowColor);
        // GDIDrawingContext::moveToEx(drawContext,(POINT *)&stack0xffffffdc);
        // GDIDrawingContext::lineTo(drawContext,&sourceOffset);
        // centerY = centerY + -1;
        // sourceOffset.x = sourceOffset.x + 1;
        // sourceOffset.y = sourceOffset.y + -1;
        // GDIDrawingContext::setPenColor(drawContext,this->indicatorHighlightColor);
        // GDIDrawingContext::moveToEx(drawContext,(POINT *)&stack0xffffffdc);
        // GDIDrawingContext::lineTo(drawContext,&sourceOffset);
        // return;
    }

    // FUNCTION: DELAYLAMA 0x10009190
    void RotaryControl::onMouseDown(Platform::Windows::GDIDrawingContext *drawingContext, POINT *mousePos)
    {
        uint32_t currentModifiers;
        //
        if (this->isEnabled == false)
        {
            return;
        }

        uint32_t lastModifiers = View::GetPressedModifiersAndMouseButtons();

        // Left button not pressed
        if ((lastModifiers & 1) == 0)
        {
            return;
        }

        // CTRL + Click: Reset to Default | 0x10 (Ctrl) | 0x01 (Left)
        if (lastModifiers == 0x11)
        {
            float defaultVal = this->getDefaultValue();
            this->value = defaultVal;
            if (this->isDirty())
            {
                this->callback(drawingContext, this);
            }
            return;
        }
        
        POINT valueRange;
        POINT startMouse;
        
        valueRange.x = this->max - this->min;
        float defaultVal = 0;
        float defaultValue = this->prevValue;
        float lastValue = this->value;
        float halfRange = valueRange.x * 0.5f;
        float stepSize = valueRange.x * 0.005f;
        startMouse.x = 0;
        startMouse.y = 0;
        float relativeY = 0;
        bool isLinearMode = false;
        float sensitivity = 200.0f;
        if (Api::GLOBAL_KNOB_MODE == 2)
        {
            // If Alt is held, use Radial/Angular mode
            if ((lastModifiers & 0x20) != 0)
            {
            LAB_RADIAL_MODE:
                valueRange.x = mousePos->x - this->rect.left;
                valueRange.y = mousePos->y - this->rect.top;
                defaultVal = this->calculateAngleFromPoint(&valueRange);
                defaultValue = defaultVal;
                goto LAB_START_DRAG_LOOP;
            }
        }
        else if ((lastModifiers & 0x20) == 0)
            goto LAB_RADIAL_MODE;

        // Linear Mode Setup
        if ((lastModifiers & 8) != 0)
        {
            // Shift-key for fine-tuning
            sensitivity = this->fineTuneDivider * 200.0f;
        }
        stepSize = (float)valueRange.x / stepSize;
        isLinearMode = true;
        startMouse.x = mousePos->x;
        startMouse.y = mousePos->y;
    LAB_START_DRAG_LOOP:
        int lastMouseX = -1;
        valueRange.y = -1;
        this->parent->beginEdit(this->parameterId);
        do
        {
            currentModifiers = View::GetPressedModifiersAndMouseButtons();
            float mouseX = mousePos->x;
            if ((mouseX != lastMouseX) || (mousePos->y != valueRange.y))
            {
                valueRange.y = mousePos->y;
                lastMouseX = mousePos->x;
                if (isLinearMode)
                {
                    mouseX = ((mouseX - mousePos->y) - startMouse.x) + startMouse.y;
                    if (currentModifiers != lastModifiers)
                    {
                        sensitivity = 200.0f;
                        if ((currentModifiers & 8) != 0)
                        {
                            sensitivity = this->fineTuneDivider * 200.0f;
                        }
                        sensitivity = (this->max - this->min) / sensitivity;
                        lastValue = (stepSize - sensitivity) * (float)mouseX + lastValue;
                        stepSize = sensitivity;
                        lastModifiers = currentModifiers;
                    }
                    this->value = (float)mouseX * stepSize + lastValue;
                    this->clampValue();
                }
                else
                {
                    float rectTop = this->rect.top;
                    mousePos->x = mouseX - this->rect.left;
                    mousePos->y = mousePos->y - rectTop;
                    defaultVal = this->calculateAngleFromPoint(mousePos);
                    this->value = (float)defaultVal;
                    if (defaultValue - defaultVal <= halfRange)
                    {
                        if (defaultVal - defaultValue <= halfRange)
                        {
                            defaultValue = (float)defaultVal;
                        }
                        else
                        {
                            this->value = this->min;
                        }
                    }
                    else
                    {
                        this->value = this->max;
                    }
                }
                bool isDirty2 = this->isDirty();
                if (isDirty2 != false)
                {
                    this->callback(drawingContext, this);
                }
            }
            drawingContext->getRelativeMousePos(mousePos);
            this->onIdle();
        } while ((lastModifiers & 1) != 0);
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
        float valueWidth = (this->max - this->min) / this->totalRange;
        this->angleRange = valueWidth;
        this->angleOffset = this->min - valueWidth * this->startAngle;
        this->deadZoneSize = (6.2831855f - abs((float)this->totalRange)) * 0.5f;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x100094d0
    void RotaryControl::calculateXYFromValue(POINT* outPoint) {
        float angle = (this->value - this->angleOffset) / this->angleRange;

        float cosAngle = std::cos(angle);
        float sinAngle = std::sin(angle);

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
        if (0.0f <= this->totalRange) {
            if (0.0f <= rawAngle) {
            normalizedAngle = rawAngle;
            if (6.283185307179586f < rawAngle) {
                normalizedAngle = rawAngle - 6.2831855f;
            }
            }
            else {
                normalizedAngle = rawAngle + 6.2831855f;
            }

            rangePlusDeadzone = this->totalRange + this->deadZoneSize;
            if (rangePlusDeadzone < normalizedAngle) {
                return this->min;
            }
            if (this->totalRange < normalizedAngle) {
                return this->max;
            }
            if (rangePlusDeadzone < rawAngle) {
                return (rawAngle - 6.2831855f) * (float)this->angleRange + this->min;
            }
            if (rawAngle < -(float)this->deadZoneSize) {
                rawAngle = rawAngle + 6.2831855f;
            }

            return rawAngle * (float)this->angleRange + this->min;
        }

        rawAngle = rawAngle - this->totalRange;
        if (0.0f <= rawAngle) {
            normalizedAngle = rawAngle;
            if (6.283185307179586f < rawAngle) {
                normalizedAngle = rawAngle - 6.2831855f;
            }
        }
        else {
            normalizedAngle = rawAngle + 6.2831855f;
        }

        rangePlusDeadzone = this->deadZoneSize - this->totalRange;

        if (rangePlusDeadzone < normalizedAngle) {
            return this->max;
        }

        if (-this->totalRange < normalizedAngle) {
            return this->min;
        }

        if (rangePlusDeadzone < rawAngle) {
            return (rawAngle - 6.2831855f) * (float)this->angleRange +
                this->max;
        }

        if (rawAngle < -this->deadZoneSize) {
            rawAngle = rawAngle + 6.2831855f;
        }

        return rawAngle * this->angleRange + this->max;
    }

    // FUNCTION: DELAYLAMA 0x100096c0
    void RotaryControl::setIndicatorShadowColor(Api::ColorRGBA color) {
        this->indicatorShadowColor.bytes.r = color.bytes.r;
        this->indicatorShadowColor.bytes.g = color.bytes.g;
        this->indicatorShadowColor.bytes.b = color.bytes.b;
        this->indicatorShadowColor.bytes.a = color.bytes.a;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x100096f0
    void RotaryControl::setIndicatorHighlightColor(Api::ColorRGBA color) {
        this->indicatorHighlightColor.bytes.r = color.bytes.r;
        this->indicatorHighlightColor.bytes.g = color.bytes.g;
        this->indicatorHighlightColor.bytes.b = color.bytes.b;
        this->indicatorHighlightColor.bytes.a = color.bytes.a;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x10009720
    void RotaryControl::setBitmap(Platform::Windows::Bitmap* bmp) {
        Platform::Windows::Bitmap* oldBitmap = this->bmp;
        if (oldBitmap != nullptr) {
            Platform::Windows::Bitmap::unregisterBitmap(oldBitmap);
            this->bmp = nullptr;
        }
        if (bmp != nullptr) {
            this->bmp = bmp;
            View::useBitmap(bmp);
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
    void RotaryControl::setKnobRadius(float radius) {
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