#include <Windows.h>
#include <windef.h>
#include <wingdi.h>

#include "damsdk/utils/portable_stdint.h"
#include "Bitmap.h"
#include "GDIDrawingContext.h"

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {

    // FUNCTION: DELAYLAMA 0x10007e20
    Bitmap::Bitmap(int resId) {        
        this->resourceId = resId;
        this->refCount = 1;
        this->width = 0;
        this->height = 0;
        this->maskBitmap = 0;
        
        HBITMAP hBitmap = LoadBitmapA(g_hInstance,(LPCSTR)(uintptr_t)(resId & 0xffff));
        this->bitmap = hBitmap;
        if (hBitmap != NULL) {
            tagBITMAP bitmapInfo;
            int bytesWritten = GetObjectA(hBitmap, sizeof(tagBITMAP), &bitmapInfo);
            if (bytesWritten != 0) {
                this->width = bitmapInfo.bmWidth;
                this->height = bitmapInfo.bmHeight;
            }
        }
    }

    // FUNCTION: DELAYLAMA 0x10007e90
    Bitmap::~Bitmap() {
        if (this->bitmap != nullptr) {
        DeleteObject(this->bitmap);
        }
        if (this->maskBitmap != nullptr) {
            DeleteObject(this->maskBitmap);
        }
        return;
    }

    // FUNCTION: DELAYLAMA 0x10007f00
    void Bitmap::blit(GDIDrawingContext *drawingContext, RECT *destRect, POINT *srcPoint) {
        if (this->bitmap != nullptr) {
            HDC hdc = CreateCompatibleDC(drawingContext->hDC);
            HGDIOBJ h = SelectObject(hdc,this->bitmap);
            BitBlt(
                drawingContext->hDC,
                drawingContext->drawOffset.x + destRect->left,
                drawingContext->drawOffset.y + destRect->top,
                destRect->right - destRect->left,
                destRect->bottom - destRect->top,
                hdc,
                srcPoint->x,
                srcPoint->y,
                SRCCOPY
            );
            SelectObject(hdc,h);
            DeleteDC(hdc);
        }
    }

    // FUNCTION: DELAYLAMA 0x100085a0
    HBITMAP Bitmap::createMaskBitmap(HDC hdcRef,HANDLE hBitmapSrc,COLORREF colorKey) {
        HDC hdcSrc = CreateCompatibleDC(hdcRef);
        SelectObject(hdcSrc,hBitmapSrc);
        
        BITMAP bm;
        GetObjectA(hBitmapSrc,sizeof(BITMAP),&bm);
        
        tagPOINT bitmapSize = {bm.bmWidth, bm.bmHeight};
        DPtoLP(hdcSrc,&bitmapSize,1);
        
        HDC hdcMask = CreateCompatibleDC(hdcRef);
        
        HBITMAP hMaskBitmap = CreateBitmap(bitmapSize.x,bitmapSize.y,1,1,NULL);
        HGDIOBJ oldMaskBitmap = SelectObject(hdcMask,hMaskBitmap);

        int oldMapMode = GetMapMode(hdcRef);
        SetMapMode(hdcSrc,oldMapMode);
        
        COLORREF oldBkColor = SetBkColor(hdcSrc,colorKey);
        BitBlt(hdcMask,0,0,bitmapSize.x,bitmapSize.y,hdcSrc,0,0,SRCCOPY);
        SetBkColor(hdcSrc,oldBkColor);
        SelectObject(hdcMask,oldMaskBitmap);
        
        DeleteDC(hdcMask);
        DeleteDC(hdcSrc);
        
        return hMaskBitmap;
    }

    // FUNCTION: DELAYLAMA 0x10007f90
    void Bitmap::drawMasked(GDIDrawingContext* drawingContext, RECT* destRect, POINT* srcPoint)
    {
        // The classic GDI transparent blit (as in VSTGUI 2.x): the mask is built once
        // from the transparent colour, then the bitmap is combined with the background
        // in an off-screen copy and the result is copied to the screen.
        if (this->maskBitmap == nullptr)
            this->maskBitmap = createMaskBitmap(drawingContext->hDC, this->bitmap,
                RGB(kTransparentColor.red, kTransparentColor.green, kTransparentColor.blue));

        HDC hdcBitmap = CreateCompatibleDC(drawingContext->hDC);
        SelectObject(hdcBitmap, this->bitmap);

        BITMAP bm;
        POINT ptSize;
        GetObjectA(this->bitmap, sizeof(BITMAP), &bm);
        ptSize.x = bm.bmWidth;
        ptSize.y = bm.bmHeight;
        DPtoLP(hdcBitmap, &ptSize, 1);

        HDC hdcBack   = CreateCompatibleDC(drawingContext->hDC);
        HDC hdcObject = CreateCompatibleDC(drawingContext->hDC);
        HDC hdcMem    = CreateCompatibleDC(drawingContext->hDC);
        HDC hdcSave   = CreateCompatibleDC(drawingContext->hDC);

        HBITMAP bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
        HBITMAP bmAndMem  = CreateCompatibleBitmap(drawingContext->hDC, ptSize.x, ptSize.y);
        HBITMAP bmSave    = CreateCompatibleBitmap(drawingContext->hDC, ptSize.x, ptSize.y);

        HGDIOBJ bmBackOld   = SelectObject(hdcBack, bmAndBack);
        HGDIOBJ bmObjectOld = SelectObject(hdcObject, this->maskBitmap);
        HGDIOBJ bmMemOld    = SelectObject(hdcMem, bmAndMem);
        HGDIOBJ bmSaveOld   = SelectObject(hdcSave, bmSave);

        // Keep the bitmap, and make the inverse of the mask
        BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcBitmap, 0, 0, SRCCOPY);
        BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);

        // Background under the bitmap, cut out where the bitmap is opaque
        BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, drawingContext->hDC,
            destRect->left + drawingContext->drawOffset.x - srcPoint->x,
            destRect->top + drawingContext->drawOffset.y - srcPoint->y, SRCCOPY);
        BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

        // Bitmap with its transparent parts cleared, combined with the background
        BitBlt(hdcBitmap, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
        BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcBitmap, 0, 0, SRCPAINT);

        BitBlt(drawingContext->hDC,
            destRect->left + drawingContext->drawOffset.x, destRect->top + drawingContext->drawOffset.y,
            destRect->right - destRect->left, destRect->bottom - destRect->top,
            hdcMem, srcPoint->x, srcPoint->y, SRCCOPY);

        // Restore the bitmap
        BitBlt(hdcBitmap, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

        DeleteObject(SelectObject(hdcBack, bmBackOld));
        DeleteObject(SelectObject(hdcMem, bmMemOld));
        DeleteObject(SelectObject(hdcSave, bmSaveOld));
        SelectObject(hdcObject, bmObjectOld);

        DeleteDC(hdcMem);
        DeleteDC(hdcBack);
        DeleteDC(hdcObject);
        DeleteDC(hdcSave);
        DeleteDC(hdcBitmap);
    }

    // FUNCTION: DELAYLAMA 0x10007ec0
    void Bitmap::remember() {
        this->refCount = this->refCount + 1;
    }

    // FUNCTION: DELAYLAMA 0x10007ed0
    void Bitmap::unregisterBitmap()
    {
        if (this->refCount > 0 && --this->refCount == 0)
            delete this;
    }
}
}
}
}