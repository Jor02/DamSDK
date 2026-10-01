#pragma once
#include <Windows.h>
#include <windef.h>
#include <damsdk/utils/portable_stdint.h>
#include <damsdk/api/DamPlugin.h>
#include <damsdk/gui/MouseButtons.h>

namespace DamSDK {
    namespace Gui {
        namespace Platform {
            namespace Windows {
                class Window;
            }
        }
    }
}

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    extern HINSTANCE g_hInstance;

    // VSTGUI's CRect and CPoint, used where the original passes them by value.
    // The copy constructor is a real function in the original (not inlined).
    struct Rect : RECT {
        Rect(const RECT& rect);
    };

    struct Point : POINT {
        Point(long x, long y) { this->x = x; this->y = y; }
    };

    extern Api::Color kWhiteColor;
    extern Api::Color kGreyColor;
    extern Api::Color kBlackColor;
    extern Api::Color kTransparentColor;

    // VTABLE: DELAYLAMA 0x1000bbe4
    class GDIDrawingContext {
        public:
            POINT screenPos;
            POINT drawOffset;
            HDC hDC;
            HWND hWnd;
            void*	parentFrame;
            char unused1[8];
            Api::Color textColor;
            char unused2[8];
            LONG penWidth;
            Api::Color penColor;
            Api::Color backgroundColor;
            int32_t lineStyle;
            char unused3[4];
            RECT rect;
            HBRUSH obj1;
            HPEN obj2;
            HBRUSH obj3;
            HGDIOBJ originalPen;
            HGDIOBJ OriginalBrush;
            HGDIOBJ OriginalFont;
            UINT penStyle;

        public:
            GDIDrawingContext(Window *parentFramePtr,HDC hDC,HWND hWnd);
            virtual ~GDIDrawingContext();
            static int32_t setModuleHandle(HINSTANCE hInstance);
            void setPenColor(Api::Color color);
            void setPenDashMode(int32_t lineStyle);
            void setBackgroundColorAndBrush(Api::Color color);
            void setTextColor(Api::Color color);
            void moveToEx(POINT* target);
            void lineTo(POINT* tageet);
            void drawRectangleOutline(RECT* rect);
            void fillRectangleInset(RECT* rect);
            void getRelativeMousePos(POINT* outRelMousePos);
            void copyToScreen(GDIDrawingContext* dest, Rect destRect, Point srcOffset);
            uint32_t getMouseButtons();
    };
}
}
}
}