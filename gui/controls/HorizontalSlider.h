#pragma once
#include "control.h"
#include "damsdk/gui/platform/windows/Bitmap.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include "damsdk/api/DamPlugin.h"
#include "damsdk/utils/portable_stdint.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // VTABLE: DELAYLAMA 0x1000c094
    class HorizontalSlider : public Control {
        public:
            POINT backgroundOffset;                 // 0x58
            POINT handlePos;                        // 0x60
            Platform::Windows::Bitmap *handleImage; // 0x68
            int handleWidth;
            int handleHeight;
            int trackMinX;
            int trackMaxX;
            int flags;
            int trackLeftX;
            int handleMinPos;
            int handleMaxPos;
            int trackWidth;
            int trackHeight;
            float fineTuneDivider;                  // 0x94
            bool isHandleTransparent;               // 0x98
            bool snapToMouse;                       // 0x99
        public:
            HorizontalSlider(RECT *pRect, ControlListener* listener, int parameterId, int minValue, int maxValue, Platform::Windows::Bitmap *handleBmp, Platform::Windows::Bitmap *backgroundBmp, POINT* offset, int flags);
            ~HorizontalSlider();
            virtual void onDraw(Platform::Windows::GDIDrawingContext* drawingContext) override;
            virtual void onMouseDown(Platform::Windows::GDIDrawingContext* drawingContext, POINT* mousePos) override;

            // New virtual functions in the original vtable order.
            virtual void setIsHandleTransparent(bool transparent);
            virtual void setSnapToMouse(bool snapToMouse);
            virtual bool getSnapToMouse();
            virtual void setHandlePos(POINT* handlePos);
            virtual void changeHandle(Platform::Windows::Bitmap* newHandle);
            virtual Platform::Windows::Bitmap* getHandleImage();
            virtual void setFinetuneDivider(float currentValue);
            virtual float getFinetuneDivider();
    };
}
}
}