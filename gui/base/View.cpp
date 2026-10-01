#include "View.h"
#include "damsdk/gui/platform/windows/GDIDrawingContext.h"
#include "damsdk/gui/platform/windows/Bitmap.h"

namespace DamSDK {
namespace Gui {
namespace Base {

    // FUNCTION: DELAYLAMA 0x10007140
    View::View(RECT * pRect) {
        this->referenceCount = 1;
        this->rect.left = pRect->left;
        this->rect.top = pRect->top;
        this->rect.right = pRect->right;
        this->rect.bottom = pRect->bottom;
        this->absRect.left = pRect->left;
        this->absRect.top = pRect->top;
        this->absRect.right = pRect->right;
        this->absRect.bottom = pRect->bottom;
        this->parent = nullptr;
        this->unused = 0;
        this->_isDirty = true;
        this->isEnabled = true;
        this->useAlphaBlending = false;
    }

    // FUNCTION: DELAYLAMA 0x100071e0
    View::~View() {
        // this->resetVtable(); This implies that view is a window, but nothing else implies that this is the case.
    }

    // FUNCTION: DELAYLAMA 0x100071a0
    bool View::isDirty() { return _isDirty; }

    // FUNCTION: DELAYLAMA 0x100071b0
    void View::setDirty(bool isDirty) { _isDirty = isDirty; }

    // FUNCTION: DELAYLAMA 0x10007220
    void View::update(Platform::Windows::GDIDrawingContext *drawingContext) {
        bool isActive = this->isDirty();
        if (isActive) {
            this->onDraw(drawingContext);
            this->setDirty(false);
        }
    }

    // FUNCTION: DELAYLAMA 0x10004450 FOLDED
    void View::onDraw(Platform::Windows::GDIDrawingContext* drawingContext) {}

    // FUNCTION: DELAYLAMA 0x10004450 FOLDED
    void View::onFocusLost(Platform::Windows::GDIDrawingContext* drawingContext) {}

    // FUNCTION: DELAYLAMA 0x10004450 FOLDED
    void View::onFocusGained(Platform::Windows::GDIDrawingContext* drawingContext) {}

    // Folded with Bitmap::remember (0x10007ec0) in the original.
    void View::remember() {
        this->referenceCount = this->referenceCount + 1;
    }

    // FUNCTION: DELAYLAMA 0x10007210
    bool View::onMouseWheel(Platform::Windows::GDIDrawingContext *drawingContext, POINT *relativeMousePoint, float scrollDelta) { return false; }

    // FUNCTION: DELAYLAMA 0x100071f0
    void View::onMouseDown(Platform::Windows::GDIDrawingContext* drawingContext, POINT* point) { }

    // FUNCTION: DELAYLAMA 0x10004570
    void View::setEnabled(bool enabled) { this->isEnabled = enabled; }

    // FUNCTION: DELAYLAMA 0x10004580
    bool View::getEnabled() {
        return this->isEnabled;
    }

    // FUNCTION: DELAYLAMA 0x10004590
    void View::setAbsRect(RECT* rect) {
        this->absRect = *rect;
    }

    // FUNCTION: DELAYLAMA 0x100045b0
    void View::getRect(RECT* outRect) {
        *outRect = this->absRect;
    }

    // FUNCTION: DELAYLAMA 0x100045e0
    void View::setUseAlphaBlending(bool useAlphaBlending) {
        this->useAlphaBlending = useAlphaBlending;
        return;
    }

    // FUNCTION: DELAYLAMA 0x100045f0
    bool View::getUseAlphaBlending() {
        return this->useAlphaBlending;
    }

    // FUNCTION: DELAYLAMA 0x10004640
    void View::setParent(Platform::Windows::Window* parent) {
        this->parent = parent;
        return;
    }

    // FUNCTION: DELAYLAMA 0x10004690
    int View::getReferenceCount() {
        return this->referenceCount;
    }

    // FUNCTION: DELAYLAMA 0x10007200
    bool View::onDrop(void** items, long count, long type, POINT* where) {
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10007250
    void View::setRect(RECT* rect) {
        this->rect = *rect;
        this->setDirty(true);
    }

    // FUNCTION: DELAYLAMA 0x10004670 FOLDED
    bool View::removed(Platform::Windows::Window *frame) { return true; }

    // FUNCTION: DELAYLAMA 0x10004670 FOLDED
    bool View::attached(Platform::Windows::Window *frame) { return true; }

    // FUNCTION: DELAYLAMA 0x10007280
    void View::release() {
        if (this->referenceCount > 0) {
            this->referenceCount--;
            if (this->referenceCount == 0)
                delete this;
        }
    }
}
}
}