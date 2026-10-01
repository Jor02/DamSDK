#pragma once

namespace DamSDK {
namespace Gui {

    // Mouse button and modifier bits returned by GDIDrawingContext::getMouseButtons
    // (as in VSTGUI)
    enum MouseButtons {
        kLButton = 1,
        kMButton = 2,
        kRButton = 4,
        kShift   = 8,
        kControl = 16,
        kAlt     = 32
    };

}
}
