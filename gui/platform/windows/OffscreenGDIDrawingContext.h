#pragma once
#include <Windows.h>
#include <windef.h>
#include "GDIDrawingContext.h"
#include <damsdk/utils/portable_stdint.h>

namespace DamSDK {
    namespace Gui {
        namespace Platform {
            namespace Windows {
                class Window;
                class Bitmap;
            }
        }
    }
}

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    extern HINSTANCE g_hInstance;

    // VTABLE: DELAYLAMA 0x1000bbe8
    class OffscreenGDIDrawingContext : public GDIDrawingContext {
        public:
            bool destroyPixmap; // 0x74: delete the offscreen bitmap in the destructor
            char unused[3]; // 0x75
            Bitmap* bitmap; // 0x78
            Bitmap* backgroundBitmap; // 0x7c
            int height; // 0x80
            int width; // 0x84
            Api::Color backgroundColor; // 0x88
        public:
            OffscreenGDIDrawingContext(Window* parentFramePtr, int width, int height, Api::Color color);
            ~OffscreenGDIDrawingContext();
    };
}
}
}
}