#include "OffscreenGDIDrawingContext.h"
#include "Window.h"
#include "Bitmap.h"

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    // FUNCTION: DELAYLAMA 0x10006f80
    OffscreenGDIDrawingContext::OffscreenGDIDrawingContext(Window* parentFramePtr, int width, int height, Api::Color color) : GDIDrawingContext(parentFramePtr, nullptr, nullptr) {
        this->bitmap = nullptr;
        this->backgroundBitmap = nullptr;
        this->width = width;
        this->height = height;
        this->backgroundColor = color;
        this->destroyPixmap = true;
        HWND hWnd = parentFramePtr->hWnd;
        HDC hdc = GetDC(hWnd);
        this->hDC = CreateCompatibleDC(hdc);
        // As in VSTGUI, the offscreen bitmap is kept in the window handle slot
        HBITMAP h = CreateCompatibleBitmap(hdc,width,height);
        this->hWnd = (HWND)h;
        
        SelectObject(this->hDC,h);
        ReleaseDC(hWnd,hdc);

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = width;
        rect.bottom = height;

        this->setBackgroundColorAndBrush(color);
        this->setPenColor(color);
        this->fillRectangleInset(&rect);
        this->drawRectangleOutline(&rect);
    }

    // FUNCTION: DELAYLAMA 0x10007050
    OffscreenGDIDrawingContext::~OffscreenGDIDrawingContext() {
        if (this->bitmap != nullptr) {
          this->bitmap->unregisterBitmap();
        }

        HDC hdc = this->hDC;
        if (hdc != nullptr) {
          DeleteDC(hdc);
        }

        
        if (this->destroyPixmap) {
            HWND ho = this->hWnd;
            if (ho != nullptr) {
                DeleteObject(ho);
            }    
        }
    }

}
}
}
}
