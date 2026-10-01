#include <cstdio>
#include <windowsx.h>
#include "Window.h"
#include "damsdk/gui/controls/control.h"
#include "damsdk/api/AudioBaseExtended.h"
#include "damsdk/api/EditorBase.h"
#include "GDIDrawingContext.h"
#include "DropTarget.h"
#include "Bitmap.h"

// Logging
#include "utils/Logger.h"

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    static LRESULT CALLBACK pluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // GLOBAL: DELAYLAMA 0x1000d80c
    char g_szWindowClassName[64];
    // GLOBAL: DELAYLAMA 0x1000d870
    int g_RegistrationCount = 0;

    // FUNCTION: DELAYLAMA 0x100072a0
    Window::Window(RECT *pRect,HWND hParent, Api::EditorBase *editor) : View(pRect) {
        this->editor = editor;
        this->handle = hParent;
        this->backgroundBitmap = NULL;
        this->numChildren = 0;
        this->maxChildren = 0;
        this->children = NULL;
        this->modalView = NULL;
        this->editView = nullptr;
        this->redrawPending = true;
        this->unused3[0] = 0;
        this->isActive = false;
        this->visible = true;
        this->closeParameter = nullptr;
        this->hWnd = nullptr;
        openPluginWindow(hParent);
    }

    // FUNCTION: DELAYLAMA 0x10007350
    Window::~Window() {
        setCursor(0);  // back to the default cursor
        setDragAndDropState(false);
        bool callExtraFlag = true;

        destroyChildren(callExtraFlag);
        if (this->backgroundBitmap != nullptr) {
          this->backgroundBitmap->unregisterBitmap();
        }
        if (this->hWnd != nullptr) {
          #if defined(_WIN64) || defined(SetWindowLongPtrA)
            SetWindowLongPtrA(this->hWnd, GWLP_USERDATA, 0);
          #else
            SetWindowLongA(this->hWnd, GWL_USERDATA, 0);
          #endif
          DestroyWindow(this->hWnd);
          Window::unregisterWindowClass();
        }
        if (this->isActive) {
          closeWindow();
        }
        if (this->closeParameter != nullptr) {
          free(this->closeParameter);
        }
    }

    // FUNCTION: DELAYLAMA 0x10007520
    void Window::onDraw(GDIDrawingContext *drawingContext) {
        if (this->redrawPending) {
            this->redrawPending = false;
        }

        Bitmap* background = this->backgroundBitmap;
        if (background != nullptr) {

            RECT destRect;
            destRect.bottom = background->height;
            destRect.right = background->width;
            destRect.left = 0;
            destRect.top = 0;

            POINT srcPoint;
            srcPoint.x = 0;
            srcPoint.y = 0;

            background->blit(drawingContext,&destRect,&srcPoint);
        }
        int i = 0;
        if (0 < this->numChildren) {
            do {
                this->children[i]->isDirty();
                this->children[i]->onDraw(drawingContext);
                this->children[i]->setDirty(false);
            i += 1;
            } while (i < this->numChildren);
        }
        if (this->modalView != NULL) {
            this->modalView->onDraw(drawingContext);
        }
    }

    // FUNCTION: DELAYLAMA 0x10007960
    void Window::refresh() {
        if ((this->visible) && (!this->redrawPending)) {
            if (needsRedraw()) {
                HDC hDC = GetDC(this->hWnd);
                GDIDrawingContext* drawingContext = new GDIDrawingContext(this, hDC, this->hWnd);
                if (drawingContext != nullptr) {
                    this->update(drawingContext);
                    delete drawingContext;
                }
                ReleaseDC(this->hWnd,hDC);
            }
        }
    }

    // FUNCTION: DELAYLAMA 0x100078b0
    void Window::update(GDIDrawingContext *drawingContext)
    {
        if (this->visible) {
            if (this->modalView != nullptr) {
                this->modalView->update(drawingContext);
                return;
            }

            bool windowIsDirty = this->isDirty();
            if (windowIsDirty) {
                this->onDraw(drawingContext);
                this->setDirty(false);
                return;
            }

            int i = 0;
            if (0 < this->numChildren) {
            do {
                this->children[i]->update(drawingContext);
                i += 1;
            } while (i < this->numChildren);
            }
        }
    }

    // FUNCTION: DELAYLAMA 0x10007450
    bool Window::openPluginWindow(HWND hParent) {
        HWND hChild;
  
        if (hParent == nullptr) {
            return false;
        }

        Window::registerWindowClass();

        int width  = this->rect.right - this->rect.left;
        int height = this->rect.bottom - this->rect.top;

        hChild = CreateWindowExA(
            0,
            (LPCSTR)g_szWindowClassName,
            "Window",
            WS_CHILD | WS_VISIBLE,
            0, 0,
            width, height,
            this->handle,
            nullptr,
            Windows::g_hInstance,
            nullptr
        );
        this->hWnd = hChild;

        #if defined(_WIN64) || defined(SetWindowLongPtrA)
            SetWindowLongPtrA(hChild, GWLP_USERDATA, (LONG_PTR)this);
        #else
            SetWindowLongA(hChild, GWL_USERDATA, (LONG)this);
        #endif
        setDragAndDropState(true);
        
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10007a10
    void Window::setBackgroundBitmap(Bitmap *background) {
        if (this->backgroundBitmap != nullptr) {
            this->backgroundBitmap->unregisterBitmap();
        }
        this->backgroundBitmap = background;
        
        if (background != nullptr) {
            background->remember();
        }
    }

    // FUNCTION: DELAYLAMA 0x100082a0
    bool Window::registerWindowClass()
    {
        g_RegistrationCount++;
        if (g_RegistrationCount == 1)
        {
            // Generate unique class name: "Plugin" + hex instance handle
            sprintf(g_szWindowClassName, "Plugin%08x", (unsigned int)(uintptr_t)Windows::g_hInstance);

            WNDCLASSA windowClass;
            windowClass.style         = CS_GLOBALCLASS;
            windowClass.lpfnWndProc   = pluginWndProc;
            windowClass.cbClsExtra = 0;
            windowClass.cbWndExtra = 0;
            windowClass.hInstance     = Windows::g_hInstance;
            windowClass.hIcon = nullptr;
            windowClass.hCursor       = LoadCursorA(nullptr, IDC_ARROW);
            windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
            windowClass.lpszMenuName = nullptr;
            windowClass.lpszClassName = (LPCSTR)&g_szWindowClassName;

            RegisterClassA(&windowClass);
        }
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10008360
    static LRESULT CALLBACK pluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        #ifdef GetWindowLongPtr
            Window* parentFramePtr = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        #else
            // Will break if it were to be compiled in 64-bit
            Window* parentFramePtr = (Window*)GetWindowLong(hWnd, GWL_USERDATA);
        #endif

        switch (uMsg)
        {
            case WM_DESTROY:
            {
                if (parentFramePtr != nullptr) {
                    parentFramePtr->visible = false;
                    parentFramePtr->handle = nullptr;
                }
                break;
            }

            case WM_PAINT:
            {
                if (parentFramePtr != nullptr && GetUpdateRect(hWnd, nullptr, FALSE)) {
                    PAINTSTRUCT ps;
                    HDC hdcPaint = BeginPaint(hWnd, &ps);
                    
                    GDIDrawingContext* drawingContext = new GDIDrawingContext(parentFramePtr, hdcPaint, hWnd);
                    parentFramePtr->onDraw(drawingContext);
                    
                    delete drawingContext;
                    
                    EndPaint(hWnd, &ps);
                    return 0;
                }
                break;
            }

            case WM_CTLCOLOREDIT:
            {
                if (parentFramePtr != nullptr && parentFramePtr->editView != nullptr) {
                    HDC hdc = (HDC)wParam; 

                    uint32_t rawTextColor = ((uint32_t*)parentFramePtr->editView)[30];
                    COLORREF textColor = RGB(rawTextColor & 0xFF, (rawTextColor >> 8) & 0xFF, (rawTextColor >> 16) & 0xFF);
                    SetTextColor(hdc, textColor);

                    uint32_t rawBgColor = ((uint32_t*)parentFramePtr->editView)[31];
                    COLORREF bgColor = RGB(rawBgColor & 0xFF, (rawBgColor >> 8) & 0xFF, (rawBgColor >> 16) & 0xFF);
                    SetBkColor(hdc, bgColor);

                    // (Text-edit views use 32-bit field offsets here; Delay Lama has none,
                    // so this is never reached.)
                    if (((uint32_t*)parentFramePtr->editView)[37] != 0) {
                        DeleteObject((HGDIOBJ)(uintptr_t)((uint32_t*)parentFramePtr->editView)[37]);
                    }

                    HBRUSH hBrush = CreateSolidBrush(bgColor);
                    ((uint32_t*)parentFramePtr->editView)[37] = (uint32_t)(uintptr_t)hBrush;

                    return (LRESULT)hBrush;
                }
                break;
            }

            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            {
                if (parentFramePtr != nullptr) {
                    HDC hdcPaint = GetDC(hWnd);
                    GDIDrawingContext* drawingContext = new GDIDrawingContext(parentFramePtr, hdcPaint, hWnd);

                    // Mouse position from lParam
                    POINT pt;
                    pt.x = GET_X_LPARAM(lParam);
                    pt.y = GET_Y_LPARAM(lParam);

                    parentFramePtr->onMouseDown(drawingContext, &pt);

                    delete drawingContext;
                    ReleaseDC(hWnd, hdcPaint);
                    return 0;
                }
                break;
            }
        }

        // Default window procedure
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }

    // FUNCTION: DELAYLAMA 0x100077e0
    bool Window::onMouseWheel(GDIDrawingContext *drawingContext, POINT *relativeMousePoint, float scrollDelta) {
        bool result = false;
        Controls::Control* view = this->getChildAtMousePos();
        if (view) {
            HDC hdc = GetDC(this->hWnd);
            GDIDrawingContext* context = new GDIDrawingContext(this, hdc, this->hWnd);
            if (context) {
                POINT where = {0, 0};
                this->getLocalMousePos(&where);
                result = view->onMouseWheel(context, &where, scrollDelta);
                delete context;
            }
            ReleaseDC(this->hWnd, hdc);
        }
        return result;
    }

    // FUNCTION: DELAYLAMA 0x100075c0
    void Window::drawControlOrSelf(Controls::Control *target) {
        Controls::Control* viewToDraw = nullptr;
        if (target) {
            for (int i = 0; i < this->numChildren; i++) {
                if (this->children[i] == target) {
                    viewToDraw = this->children[i];
                    break;
                }
            }
        }

        HDC hdc = GetDC(this->hWnd);
        GDIDrawingContext* context = new GDIDrawingContext(this, hdc, this->hWnd);
        if (context) {
            if (viewToDraw)
                viewToDraw->onDraw(context);
            else
                this->onDraw(context);
            delete context;
        }
        ReleaseDC(this->hWnd, hdc);
    }

    // FUNCTION: DELAYLAMA 0x10007920
    bool Window::needsRedraw() {
        if (this->modalView || this->isDirty())
            return true;
        for (int i = 0; i < this->numChildren; i++) {
            if (this->children[i]->isDirty())
                return true;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10007a40
    bool Window::registerControl(Controls::Control *control) {
        if (this->numChildren == this->maxChildren) {
            this->maxChildren += 10;
            if (this->children)
                this->children = (Controls::Control**)realloc(this->children, this->maxChildren * sizeof(Controls::Control*));
            else
                this->children = (Controls::Control**)malloc(this->maxChildren * sizeof(Controls::Control*));
            if (this->children == nullptr) {
                this->maxChildren = 0;
                return false;
            }
        }
        this->children[this->numChildren] = control;
        this->numChildren++;
        control->parent = this;
        control->attached(this);   // attached()
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10008340
    void Window::unregisterWindowClass() {
        if (--g_RegistrationCount == 0)
            UnregisterClassA(g_szWindowClassName, g_hInstance);
    }

    // FUNCTION: DELAYLAMA 0x10007410
    bool Window::closeWindow() {
        if (((this->isActive) && (this->visible)) && (this->handle != nullptr))
        {
          this->editor->mainPlugin->closePluginEditorOnHost(this->closeParameter);
          this->handle = nullptr;
          return true;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100074c0
    bool Window::setDragAndDropState(bool enable) {
        // Accept files dragged onto the editor (VSTGUI's CFrame::setDropActive)
        if (!this->dropActive && !enable)
            return true;
        if (!this->hWnd)
            return false;
        if (enable)
            RegisterDragDrop(this->hWnd, (IDropTarget*)createDropTarget(this));
        else
            RevokeDragDrop(this->hWnd);
        this->dropActive = enable;
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10007690
    void Window::onMouseDown(GDIDrawingContext *drawingContext, POINT *mousePos) {
        if (this->editView) {
            this->editView->onFocusLost(nullptr);
            this->editView = nullptr;
        }

        if (this->modalView) {
            Base::View* modal = this->modalView;
            if (mousePos->x >= modal->rect.left && mousePos->x <= modal->rect.right &&
                mousePos->y >= modal->rect.top && mousePos->y <= modal->rect.bottom)
                modal->onMouseDown(drawingContext, mousePos);
        }
        else {
            for (int i = this->numChildren - 1; i >= 0; i--) {
                if (this->children[i]->getEnabled() &&
                    mousePos->x >= this->children[i]->rect.left && mousePos->x <= this->children[i]->rect.right &&
                    mousePos->y >= this->children[i]->rect.top && mousePos->y <= this->children[i]->rect.bottom) {
                    this->children[i]->onMouseDown(drawingContext, mousePos);
                    return;
                }
            }
        }
    }

    // FUNCTION: DELAYLAMA 0x10007740
    bool Window::onDrop(void** items, long count, long type, POINT* mousePos) {
        if (this->modalView || this->editView)
            return false;

        bool result = false;
        for (int i = this->numChildren - 1; i >= 0; i--) {
            if (this->children[i]->getEnabled()) {
                Controls::Control* control = this->children[i];
                if (mousePos->x >= control->rect.left && mousePos->x <= control->rect.right &&
                    mousePos->y >= control->rect.top && mousePos->y <= control->rect.bottom) {
                    if (control->onDrop(items, count, type, mousePos)) {
                        result = true;
                        break;
                    }
                }
            }
        }
        return result;
    }

    // FUNCTION: DELAYLAMA 0x10007ac0
    bool Window::removeChild(Controls::Control* child, const bool& withForget) {
        bool found = false;
        for (int i = 0; i < this->numChildren; i++) {
            if (found)
                this->children[i - 1] = this->children[i];
            if (this->children[i] == child) {
                child->removed(this);
                if (withForget)
                    child->release();
                found = true;
            }
        }
        if (found)
            this->numChildren--;
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10007b30
    bool Window::destroyChildren(const bool& withForget) {
        if (this->editView) {
            this->editView->onFocusLost(nullptr);
            this->editView = nullptr;
        }
        if (this->children) {
            for (int i = 0; i < this->numChildren; i++) {
                this->children[i]->removed(this);
                if (withForget)
                    this->children[i]->release();
                this->children[i] = nullptr;
            }
            free(this->children);
            this->children = nullptr;
            this->numChildren = 0;
            this->maxChildren = 0;
        }
        return true;
    }

    // FUNCTION: DELAYLAMA 0x10007bb0
    bool Window::containsChild(Controls::Control* target) {
        bool found = false;
        for (int i = 0; i < this->numChildren; i++) {
            if (this->children[i] == target) {
                found = true;
                break;
            }
        }
        return found;
    }

    // FUNCTION: DELAYLAMA 0x10007be0
    int32_t Window::setModalView(Base::View* view) {
        if ((view != nullptr) && (this->modalView != nullptr)) {
          return 0;
        }
        if (this->modalView != nullptr) {
          this->modalView->removed(this);
        }
        this->modalView = view;
        if (view != nullptr) {
          view->attached(this);
        }
        return 1;
    }

    // FUNCTION: DELAYLAMA 0x10007c20
    void Window::beginEdit(int parameterId) {
        if (this->editor != nullptr) {
            this->editor->mainPlugin->notifyHostClientBeginningParameterEdit(parameterId);
        }
    }

    // FUNCTION: DELAYLAMA 0x10007c40
    void Window::endEdit(int parameterId) {
        if (this->editor != nullptr) {
            this->editor->mainPlugin->notifyHostClientEndingParameterEdit(parameterId);
        }
    }

    // FUNCTION: DELAYLAMA 0x10007c60
    Controls::Control* Window::getChildAtMousePos() {
        POINT where = {0, 0};
        this->getLocalMousePos(&where);
        for (int i = this->numChildren - 1; i >= 0; i--) {
            Controls::Control* view = this->children[i];
            if (view && where.x >= view->rect.left && where.x <= view->rect.right &&
                where.y >= view->rect.top && where.y <= view->rect.bottom)
                return this->children[i];
        }
        return nullptr;
    }

    // FUNCTION: DELAYLAMA 0x10007cd0
    bool Window::getLocalMousePos(POINT* mousePos) {
        HWND hWnd = this->hWnd;
        POINT cursor;
        GetCursorPos(&cursor);
        mousePos->x = cursor.x;
        mousePos->y = cursor.y;
        if (hWnd) {
            RECT windowRect;
            GetWindowRect(hWnd, &windowRect);
            mousePos->x -= windowRect.left;
            mousePos->y -= windowRect.top;
        }
        return true;
    }


    // FUNCTION: DELAYLAMA 0x10007d30
    void Window::setCursor(int cursorType) {
        // 0 default, 1 wait, 2 horizontal resize, 3 vertical resize, 4 move,
        // 5 and 6 diagonal resize (VSTGUI's CFrame::setCursor)
        if (!this->defaultCursor)
            this->defaultCursor = GetCursor();
        switch (cursorType) {
            case 0:
                SetCursor(this->defaultCursor);
                break;
            case 1:
                SetCursor(LoadCursorA(NULL, IDC_WAIT));
                break;
            case 2:
                SetCursor(LoadCursorA(NULL, IDC_SIZEWE));
                break;
            case 3:
                SetCursor(LoadCursorA(NULL, IDC_SIZENS));
                break;
            case 5:
                SetCursor(LoadCursorA(NULL, IDC_SIZENWSE));
                break;
            case 6:
                SetCursor(LoadCursorA(NULL, IDC_SIZENESW));
                break;
            case 4:
                SetCursor(LoadCursorA(NULL, IDC_SIZEALL));
                break;
        }
    }
}
}
}
}