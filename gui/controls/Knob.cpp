#pragma once
#include "Knob.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // FUNCTION: DELAYLAMA 0x100097c0
    Knob::Knob(RECT *pRect, ControlListener* listener, int parameterId, int totalFrames, int frameHeight, Platform::Windows::Bitmap *bmp, POINT *origin) : RotaryControl(pRect, listener, parameterId, bmp, nullptr, origin)
    {
        this->frameHeight = frameHeight;
        this->totalFrames = totalFrames;
        this->knobRadius = 0.0;
    }

    // FUNCTION: DELAYLAMA 0x10009830
    Knob::~Knob() {
    }

    // FUNCTION: DELAYLAMA 0x10009840
    void Knob::onDraw(Platform::Windows::GDIDrawingContext* drawingContext) {
        // Same steps as VSTGUI's CAnimKnob::draw: pick the frame for the value
        POINT where;
        where.x = 0;
        if (this->value > 0.0f) {
            long lastFrameY = (this->totalFrames - 1) * this->frameHeight;
            where.y = (long)(lastFrameY * this->value);
            for (long frameY = 0; frameY <= lastFrameY; frameY += this->frameHeight) {
                if (where.y < frameY) {
                    where.y = frameY - this->frameHeight;
                    break;
                }
            }
        }
        else {
            where.y = 0;
        }

        if (this->bitmap) {
            if (this->useAlphaBlending)
                this->bitmap->drawMasked(drawingContext, &this->rect, &where);
            else
                this->bitmap->blit(drawingContext, &this->rect, &where);
        }
        this->setDirty(false);
    }
}
}
}