#pragma once
#include <Windows.h>
#include <windef.h>
#include <damsdk/utils/portable_stdint.h>

namespace DamSDK {
    namespace Gui {
        namespace Platform {
            namespace Windows {
                class GDIDrawingContext;
                class Bitmap;
                class Window;
            }
        }
    }
}

namespace DamSDK {
namespace Gui {
namespace Base {

    // VTABLE: DELAYLAMA 0x1000bbec
    class View {
        public:
            int referenceCount;
            RECT rect;
            RECT absRect;
            DamSDK::Gui::Platform::Windows::Window *parent;
            int unused;
            bool _isDirty;
            bool isEnabled;
            bool useAlphaBlending;
        public:
            View(RECT *pRect);
            // Virtual functions in the original vtable order (VSTGUI 2.x CView). Do not reorder.
            virtual ~View();
            virtual void onDraw(Platform::Windows::GDIDrawingContext* drawingContext);
            virtual void onMouseDown(Platform::Windows::GDIDrawingContext* drawingContext, POINT* point);
            virtual void update(Platform::Windows::GDIDrawingContext *drawingContext);
            virtual bool onDrop(void** items, long count, long type, POINT* where);
            virtual bool onMouseWheel(Platform::Windows::GDIDrawingContext *drawingContext, POINT *relativeMousePoint, float scrollDelta);
            virtual void onFocusLost(Platform::Windows::GDIDrawingContext* drawingContext);
            virtual void onFocusGained(Platform::Windows::GDIDrawingContext* drawingContext);
            virtual bool isDirty();
            virtual void setDirty(bool isDirty);
            virtual void setEnabled(bool enabled);
            virtual bool getEnabled();
            virtual void setAbsRect(RECT* rect);
            virtual void getRect(RECT* outRect);
            virtual void setUseAlphaBlending(bool useAlphaBlending);
            virtual bool getUseAlphaBlending();
            virtual void setRect(RECT* rect);
            virtual void setParent(Platform::Windows::Window* parent);
            virtual bool removed(Platform::Windows::Window *frame);
            virtual bool attached(Platform::Windows::Window *frame);
            virtual void release();
            virtual void remember();
            virtual int getReferenceCount();


    };
}
}
}