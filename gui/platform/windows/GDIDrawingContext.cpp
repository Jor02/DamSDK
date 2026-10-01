#include "GDIDrawingContext.h"
#include "Window.h"

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    // GLOBAL: DELAYLAMA 0x1000d400
    HINSTANCE g_hInstance = NULL;

    Api::Color kWhiteColor = {255, 255, 255, 0};
    Api::Color kGreyColor = {127, 127, 127, 0};
    Api::Color kBlackColor = {0, 0, 0, 0};

    // Colour treated as transparent by Bitmap::drawMasked (white)
    // GLOBAL: DELAYLAMA 0x1000d86c
    Api::Color kTransparentColor = {255, 255, 255, 0};

    // FUNCTION: DELAYLAMA 0x10006960
    GDIDrawingContext::GDIDrawingContext(Window *parentFramePtr,HDC hDC,HWND hWnd) {
        this->screenPos.x = 0;
        this->screenPos.y = 0;
        this->drawOffset.x = 0;
        this->drawOffset.y = 0;
        this->hDC = hDC;
        this->hWnd = hWnd;
        this->parentFrame = parentFramePtr;

        memset(this->unused2, 0, sizeof(this->unused2));
        this->penWidth = 1;
        this->lineStyle = 0;
        memset(this->unused3, 0, sizeof(this->unused3));

        this->rect.left = 0;
        this->rect.top = 0;
        this->rect.right = 0;
        this->rect.bottom = 0;

        this->obj1 = NULL;
        this->obj2 = NULL;
        this->obj3 = NULL;
        this->originalPen = NULL;
        this->OriginalBrush = NULL;
        this->OriginalFont = NULL;

        // Set the drawing rect
        if (parentFramePtr == NULL)
        {
            this->rect.left = 0;
            this->rect.top = 0;
            this->rect.right = 1000;
            this->rect.bottom = 1000;
        }
        else
        {
            this->rect.left   = parentFramePtr->rect.left;
            this->rect.top    = parentFramePtr->rect.top;
            this->rect.right  = parentFramePtr->rect.right;
            this->rect.bottom = parentFramePtr->rect.bottom;
        }

        // Colors
        this->penColor = kWhiteColor;
        this->backgroundColor = kBlackColor;
        this->textColor = kWhiteColor;

        this->screenPos.x = 0;
        this->screenPos.y = 0;
        this->drawOffset.x = 0;
        this->drawOffset.y = 0;

        if (hDC != NULL)
        {
            this->originalPen   = GetCurrentObject(hDC, OBJ_PEN);
            this->OriginalBrush = GetCurrentObject(hDC, OBJ_BRUSH);
            this->OriginalFont  = GetCurrentObject(hDC, OBJ_FONT);

            SetBkMode(hDC, TRANSPARENT);
        }

        this->penStyle = 0;

        if (hWnd != NULL)
        {
            RECT windowRect;
            GetWindowRect(hWnd, &windowRect);

            this->screenPos.x = windowRect.left;
            this->screenPos.y = windowRect.top;
        }

        if (hDC != NULL)
        {
            this->setPenColor(this->penColor);
            this->setPenDashMode(this->lineStyle);
            this->setBackgroundColorAndBrush(this->backgroundColor);
            this->setTextColor(this->textColor);
        }
    }

    // FUNCTION: DELAYLAMA 0x10006da0
    void GDIDrawingContext::setPenColor(Api::Color color)
    {
        this->penColor = color;
        LOGPEN pen = {this->penStyle, {this->penWidth, this->penWidth}, RGB(this->penColor.red, this->penColor.green, this->penColor.blue)};
        HPEN newPen = CreatePenIndirect(&pen);
        SelectObject(this->hDC, newPen);
        if (this->obj2 != NULL)
            DeleteObject(this->obj2);
        this->obj2 = newPen;
    }

    // FUNCTION: DELAYLAMA 0x10006be0
    void GDIDrawingContext::setPenDashMode(int32_t lineStyle)
    {
        this->lineStyle = lineStyle;
        if (lineStyle == 1)
            this->penStyle = PS_DOT;
        else
            this->penStyle = PS_SOLID;
        LOGPEN pen = {this->penStyle, {this->penWidth, this->penWidth}, RGB(this->penColor.red, this->penColor.green, this->penColor.blue)};
        HPEN newPen = CreatePenIndirect(&pen);
        SelectObject(this->hDC, newPen);
        if (this->obj2 != NULL)
            DeleteObject(this->obj2);
        this->obj2 = newPen;
    }

    // FUNCTION: DELAYLAMA 0x10006e20
    void GDIDrawingContext::setBackgroundColorAndBrush(Api::Color color)
    {
        this->backgroundColor = color;
        SetBkColor(this->hDC, RGB(color.red, color.green, color.blue));
        LOGBRUSH brush = {BS_SOLID, RGB(color.red, color.green, color.blue), 0};
        HBRUSH newBrush = CreateBrushIndirect(&brush);
        if (newBrush == NULL) {
            GetLastError();
            return;
        }
        SelectObject(this->hDC, newBrush);
        if (this->obj1 != NULL)
            DeleteObject(this->obj1);
        this->obj1 = newBrush;
    }

    // FUNCTION: DELAYLAMA 0x10006d60
    void GDIDrawingContext::setTextColor(Api::Color color)
    {
        this->textColor = color;
        SetTextColor(this->hDC, RGB(this->textColor.red, this->textColor.green, this->textColor.blue));
    }

    // FUNCTION: DELAYLAMA 0x10003620
    int32_t GDIDrawingContext::setModuleHandle(HINSTANCE hInstance)
    {
        g_hInstance = hInstance;
        return 1;
    }

    // FUNCTION: DELAYLAMA 0x10006b10
    GDIDrawingContext::~GDIDrawingContext() {
        HGDIOBJ pvVar1 = this->originalPen;
        
        if (pvVar1 != nullptr) {
          SelectObject(this->hDC,pvVar1);
        }

        pvVar1 = this->OriginalBrush;
        if (pvVar1 != nullptr) {
          SelectObject(this->hDC,pvVar1);
        }

        pvVar1 = this->OriginalFont;
        if (pvVar1 != nullptr) {
          SelectObject(this->hDC,pvVar1);
        }

        pvVar1 = this->obj1;
        if (pvVar1 != nullptr) {
          DeleteObject(pvVar1);
        }

        pvVar1 = this->obj2;
        if (pvVar1 != nullptr) {
          DeleteObject(pvVar1);
        }

        pvVar1 = this->obj3;
        if (pvVar1 != nullptr) {
          DeleteObject(pvVar1);
        }
    }

    // FUNCTION: DELAYLAMA 0x10006b80
    void GDIDrawingContext::moveToEx(POINT* target) {
        POINT point = *target;
        point.x += this->drawOffset.x;
        point.y += this->drawOffset.y;
        MoveToEx(this->hDC, point.x, point.y, nullptr);
    }

    // FUNCTION: DELAYLAMA 0x10006bb0
    void GDIDrawingContext::lineTo(POINT* target) {
        POINT point = *target;
        point.x += this->drawOffset.x;
        point.y += this->drawOffset.y;
        LineTo(this->hDC, point.x, point.y);
    }

    // FUNCTION: DELAYLAMA 0x10006c60
    void GDIDrawingContext::drawRectangleOutline(RECT* inRect) {
        // Copy the rect, then offset it (VSTGUI CRect copy + offset())
        RECT rect;
        rect.left = inRect->left;
        rect.top = inRect->top;
        rect.right = inRect->right;
        rect.bottom = inRect->bottom;
        rect.left += this->drawOffset.x;
        rect.right += this->drawOffset.x;
        rect.top += this->drawOffset.y;
        rect.bottom += this->drawOffset.y;

        MoveToEx(this->hDC, rect.left, rect.top, NULL);
        LineTo(this->hDC, rect.right, rect.top);
        LineTo(this->hDC, rect.right, rect.bottom);
        LineTo(this->hDC, rect.left, rect.bottom);
        LineTo(this->hDC, rect.left, rect.top);
    }

    // FUNCTION: DELAYLAMA 0x10006ce0
    void GDIDrawingContext::fillRectangleInset(RECT* inRect) {
        // Copy the rect, then offset it (VSTGUI CRect copy + offset())
        RECT rect;
        rect.left = inRect->left;
        rect.top = inRect->top;
        rect.right = inRect->right;
        rect.bottom = inRect->bottom;
        rect.left += this->drawOffset.x;
        rect.right += this->drawOffset.x;
        rect.top += this->drawOffset.y;
        rect.bottom += this->drawOffset.y;

        // Don't draw the boundary
        RECT fillRect = {rect.left + 1, rect.top + 1, rect.right, rect.bottom};
        HGDIOBJ nullPen = GetStockObject(NULL_PEN);
        HGDIOBJ oldPen = SelectObject(this->hDC, nullPen);
        FillRect(this->hDC, &fillRect, (HBRUSH)this->obj1);
        SelectObject(this->hDC, oldPen);
    }

    // FUNCTION: DELAYLAMA 0x10006f20
    void GDIDrawingContext::getRelativeMousePos(POINT* outRelMousePos) {

        POINT mousePos;
        GetCursorPos(&mousePos);
        outRelMousePos->x = mousePos.x;
        outRelMousePos->y = mousePos.y;
        int yPos = this->screenPos.y;
        outRelMousePos->x = mousePos.x - this->screenPos.x;
        outRelMousePos->y = mousePos.y - yPos;
        return;
    }

    // FUNCTION: DELAYLAMA 0x10006ec0
    uint32_t GDIDrawingContext::getMouseButtons() {
        uint32_t buttons = 0;
        if (GetAsyncKeyState(VK_LBUTTON) < 0)
            buttons |= kLButton;
        if (GetAsyncKeyState(VK_MBUTTON) < 0)
            buttons |= kMButton;
        if (GetAsyncKeyState(VK_RBUTTON) < 0)
            buttons |= kRButton;
        if (GetAsyncKeyState(VK_SHIFT) < 0)
            buttons |= kShift;
        if (GetAsyncKeyState(VK_CONTROL) < 0)
            buttons |= kControl;
        if (GetAsyncKeyState(VK_MENU) < 0)
            buttons |= kAlt;
        return buttons;
    }

    // FUNCTION: DELAYLAMA 0x100070d0
    Rect::Rect(const RECT& rect) {
        this->left = rect.left;
        this->top = rect.top;
        this->right = rect.right;
        this->bottom = rect.bottom;
    }

    // FUNCTION: DELAYLAMA 0x100070f0
    void GDIDrawingContext::copyToScreen(GDIDrawingContext* dest, Rect destRect, Point srcOffset) {
        BitBlt(dest->hDC, dest->drawOffset.x + destRect.left, dest->drawOffset.y + destRect.top,
            destRect.right - destRect.left, destRect.bottom - destRect.top,
            this->hDC, srcOffset.x, srcOffset.y, SRCCOPY);
    }
}
}
}
}