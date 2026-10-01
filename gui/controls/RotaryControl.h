#pragma once
#include "control.h"
#include "damsdk/utils/portable_stdint.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include "damsdk/gui/platform/windows/Bitmap.h"
#include "damsdk/api/DamPlugin.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // VTABLE: DELAYLAMA 0x1000bd78
    class RotaryControl : public Control {
        public:
            POINT srcPoint;
            Api::Color indicatorHighlightColor;
            Api::Color indicatorShadowColor;
            Platform::Windows::Bitmap *bmp;
            int knobRadius;
            float startAngle;
            float totalRange;
            float deadZoneSize;
            float angleRange;
            float angleOffset;
            float center;
            float fineTuneDivider;

        public:
            RotaryControl(RECT* pRect, ControlListener* listener, int parameterId, Platform::Windows::Bitmap* bmp1, Platform::Windows::Bitmap* bmp2, POINT* srcPoint);
            ~RotaryControl();
            virtual void onDraw(Platform::Windows::GDIDrawingContext* drawingContext) override;
            virtual void onMouseDown(Platform::Windows::GDIDrawingContext* drawingContext, POINT* mousePos) override;

            // New virtual functions, in the original vtable order.
            virtual void drawIndicator(Platform::Windows::GDIDrawingContext* drawingContext);
            virtual void setStartAngle(float startAngle);
            virtual float getStartAngle();
            virtual void setTotalRange(float totalRange);
            virtual float getTotalRange();
            virtual void calculateXYFromValue(POINT* outPoint);
            virtual float calculateAngleFromPoint(POINT* point);
            virtual void setKnobRadius(int radius);
            virtual void setIndicatorShadowColor(Api::Color color);
            virtual void setIndicatorHighlightColor(Api::Color color);
            virtual void setBitmap(Platform::Windows::Bitmap* bmp);
            virtual void setFineTuneDivider(float divider);
            virtual float getFineTuneDivider();

            void updateMathConstants();
    };
}
}
}