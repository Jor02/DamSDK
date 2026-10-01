#pragma once
#include "control.h"
#include "HorizontalSlider.h"

namespace DamSDK {
namespace Gui {
namespace Controls {

    // VTABLE: DELAYLAMA 0x1000b914
    class TwoAxisSlider : public HorizontalSlider {
        public:
            int yTop;
            int yBottom;
            int unusedA4;
            int xMinOffset;
            int xMaxOffset;
            int yTrackBottom;
            float xRange;
            float yRange;
            int halfHandleHeight1;
            float xValueRange;
            float yValueRange;
            int halfHandleHeight2;
        public:
            TwoAxisSlider(RECT *bounds, ControlListener* listener, int parameterId, int minValue, int maxValue, Platform::Windows::Bitmap *handle, Platform::Windows::Bitmap *background, POINT* offset, int flags);
            // The original draws nothing for the two-axis pad.
            virtual void onDraw(DamSDK::Gui::Platform::Windows::GDIDrawingContext* drawContext) override;
            virtual void onMouseDown(DamSDK::Gui::Platform::Windows::GDIDrawingContext* drawContext, POINT* mousePos) override;
    };
}
}
}