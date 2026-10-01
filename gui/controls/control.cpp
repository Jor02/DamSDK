#include "control.h"
#include "damsdk/api/EditorBase.h"
#include "damsdk/gui/platform/windows/Window.h"
#include "damsdk/gui/platform/windows/Bitmap.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include "damsdk/utils/portable_stdint.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // FUNCTION: DELAYLAMA 0x10008c80
    Control::Control(RECT *pRect, ControlListener* listener, int parameterId, Platform::Windows::Bitmap *bmp) : Base::View(pRect) {
        this->listener = listener;
        this->prevValue = 1.0f;
        this->max = 1.0f;
        this->parameterId = parameterId;
        this->defaultValue = 0.5f;
        this->value = 0.0f;
        this->min = 0.0f;
        this->wheelSensitivity = 0.1f;
        this->bitmap = bmp;
        this->useAlphaBlending = false;
        this->isEnabled = true;

        if (bmp != nullptr) {
            bmp->remember();
        }
    }

    // FUNCTION: DELAYLAMA 0x10008d30
    Control::~Control() {
        Platform::Windows::Bitmap* bmp = this->bitmap;
        if (bmp != nullptr) {
          bmp->unregisterBitmap();
        }
    }

    // FUNCTION: DELAYLAMA 0x10001960 FOLDED
    void Control::setDefaultValue(float defaultValue) {
        this->defaultValue = defaultValue;
    }

    // FUNCTION: DELAYLAMA 0x100047b0
    void Control::setValue(float newValue) {
        this->value = newValue;
    }

    // FUNCTION: DELAYLAMA 0x10004790
    void Control::onIdle() {
        if (this->parent != nullptr && this->parent->editor != nullptr)
            this->parent->editor->idleHandler();
    }

    // FUNCTION: DELAYLAMA 0x100047c0
    float Control::getValue() {
        return this->value;
    }

    // FUNCTION: DELAYLAMA 0x100047d0
    void Control::setMin(float min) {
        this->min = min;
    }

    // FUNCTION: DELAYLAMA 0x100047e0
    float Control::getMin() {
        return this->min;
    }

    // FUNCTION: DELAYLAMA 0x100047f0
    void Control::setMax(float max) {
        this->max = max;
    }

    // FUNCTION: DELAYLAMA 0x10004800
    float Control::getMax() {
        return this->max;
    }

    // FUNCTION: DELAYLAMA 0x10001950 FOLDED
    void Control::setPreviousValue(float previousValue) {
        this->prevValue = previousValue;
    }

    // FUNCTION: DELAYLAMA 0x10004810
    float Control::getPreviousValue() {
        return this->prevValue;
    }

    // FUNCTION: DELAYLAMA 0x10004820
    float Control::getDefaultValue() {
        return this->defaultValue;
    }

    // FUNCTION: DELAYLAMA 0x10004830
    void Control::setParameterId(int parameterId) {
        this->parameterId = parameterId;
    }

    // FUNCTION: DELAYLAMA 0x10004840
    Platform::Windows::Bitmap* Control::getBitmap() {
        return this->bitmap;
    }

    // FUNCTION: DELAYLAMA 0x10004850
    void Control::setWheelSensitivity(float sensitivity) {
        this->wheelSensitivity = sensitivity;
    }

    // FUNCTION: DELAYLAMA 0x10004860
    float Control::getWheelSensitivity() {
        return this->wheelSensitivity;
    }

    // FUNCTION: DELAYLAMA 0x10008d90
    bool Control::isDirty() {
        if ((this->prevValue == this->value) && (!this->_isDirty)) {
          return false;
        }
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10008db0
    void Control::setDirty(bool isDirty) {
        this->_isDirty = isDirty;
        if (isDirty) {
            if (this->value != -1.0f)
                this->prevValue = -1.0f;
            else
                this->prevValue = 0.0f;
        }
        else {
            this->prevValue = this->value;
        }
    }

    // FUNCTION: DELAYLAMA 0x10008de0
    void Control::changeBitmap(Platform::Windows::Bitmap* newBitmap) {
        Platform::Windows::Bitmap* oldBitmap = this->bitmap;
        if (oldBitmap != nullptr) {
          oldBitmap->unregisterBitmap();
        }
        this->bitmap = newBitmap;
        if (newBitmap != nullptr) {
          newBitmap->remember();
        }
    }

    // FUNCTION: DELAYLAMA 0x10008e10
    void Control::clampValue() {
        if (this->value > this->max)
            this->value = this->max;
        else if (this->value < this->min)
            this->value = this->min;
    }

    // FUNCTION: DELAYLAMA 0x10009410
    bool Control::onMouseWheel(Platform::Windows::GDIDrawingContext* drawingContext, POINT* mousePos, float wheelDelta) {
        if (!this->isEnabled) {
          return false;
        }

        byte inputMask = drawingContext->getMouseButtons();
        float valueChange = wheelDelta * this->wheelSensitivity;

        if ((inputMask & kShift) != 0) {
          valueChange = valueChange * 1.0f;
        }

        this->value = valueChange + this->value;
        this->clampValue();

        bool isDirty = this->isDirty();
        if (isDirty) {
            this->listener->valueChanged(drawingContext, this);
        }
        return true;
    }

    bool Control::returnTrue(Platform::Windows::Window* window) {
        return true;
    }
}
}
}