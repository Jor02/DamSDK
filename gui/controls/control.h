#pragma once
#include "damsdk/gui/base/View.h"

namespace DamSDK {
    namespace Gui {
        namespace Controls {
            class Control;
        }
        namespace Platform {
            namespace Windows {
                class Bitmap;
                class GDIDrawingContext;
            }
        }
    }
}

namespace DamSDK {
namespace Gui {
namespace Controls {

    class Control;

    // VTABLE: DELAYLAMA 0x1000b8f0
    // Receives value changes from controls (VSTGUI CControlListener).
    class ControlListener {
        public:
            virtual void valueChanged(Platform::Windows::GDIDrawingContext* context, Control* control) = 0;
    };


    // VTABLE: DELAYLAMA 0x1000bcd4
    class Control : public Base::View {
        public:
            ControlListener* listener;
            int parameterId;
            float prevValue;
            float defaultValue;
            float value;
            float min;
            float max;
            float wheelSensitivity;
            Platform::Windows::Bitmap *bitmap;
        public:
            Control(RECT *pRect, ControlListener* listener, int parameterId, Platform::Windows::Bitmap *bmp);
            virtual ~Control();

            virtual bool isDirty() override;
            virtual void setDirty(bool isDirty) override;
            virtual bool onMouseWheel(Platform::Windows::GDIDrawingContext* drawingContext, POINT* mousePos, float wheelDelta) override;

            // New virtual functions in the original vtable order (VSTGUI 2.x CControl).
            virtual void onIdle();
            virtual void setValue(float newValue);
            virtual float getValue();
            virtual void setMin(float min);
            virtual float getMin();
            virtual void setMax(float max);
            virtual float getMax();
            virtual void setPreviousValue(float previousValue);
            virtual float getPreviousValue();
            virtual void setDefaultValue(float defaultValue = 0);
            virtual float getDefaultValue();
            virtual void setParameterId(int parameterId);
            virtual void changeBitmap(Platform::Windows::Bitmap* newBitmap);
            virtual Platform::Windows::Bitmap* getBitmap();
            virtual void setWheelSensitivity(float sensitivity);
            virtual float getWheelSensitivity();
            virtual void clampValue();
            bool returnTrue(Platform::Windows::Window* window);
    };
}
}
}