#include <cstring>
#include <shlobj.h>
#include <shellapi.h>
#include "DropTarget.h"
#include "Window.h"

namespace DamSDK {
namespace Gui {
namespace Platform {
namespace Windows {
    // FUNCTION: DELAYLAMA 0x10008690
    DropTarget* createDropTarget(Window* frame) {
        return new DropTarget(frame);
    }

    // FUNCTION: DELAYLAMA 0x100086f0
    DropTarget::DropTarget(Window* frame) {
        this->refCount = 0;
        this->parentFrame = frame;
    }

    // FUNCTION: DELAYLAMA 0x10008730
    DropTarget::~DropTarget() {}

    // FUNCTION: DELAYLAMA 0x10008740
    STDMETHODIMP DropTarget::QueryInterface(REFIID riid, void** object) {
        if (riid == IID_IDropTarget || riid == IID_IUnknown) {
            *object = this;
            AddRef();
            return NOERROR;
        }
        *object = 0;
        return E_NOINTERFACE;
    }

    // FUNCTION: DELAYLAMA 0x100087a0
    STDMETHODIMP_(ULONG) DropTarget::AddRef(void) {
        return ++this->refCount;
    }

    // FUNCTION: DELAYLAMA 0x100087b0
    STDMETHODIMP_(ULONG) DropTarget::Release(void) {
        this->refCount--;
        if (this->refCount <= 0)
            delete this;
        return this->refCount;
    }

    // FUNCTION: DELAYLAMA 0x100087e0
    STDMETHODIMP DropTarget::DragEnter(IDataObject* dataObject, DWORD keyState, POINTL pt, DWORD* effect) {
        this->canAcceptDrop = false;
        if (dataObject) {
            FORMATETC formatTextDrop = {CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
            if (S_OK == dataObject->QueryGetData(&formatTextDrop)) {
                this->canAcceptDrop = true;
                return DragOver(keyState, pt, effect);
            }
            FORMATETC formatHDrop = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
            if (S_OK == dataObject->QueryGetData(&formatHDrop)) {
                this->canAcceptDrop = true;
                return DragOver(keyState, pt, effect);
            }
        }
        *effect = DROPEFFECT_NONE;
        return S_OK;
    }

    // FUNCTION: DELAYLAMA 0x100088c0
    STDMETHODIMP DropTarget::DragOver(DWORD keyState, POINTL pt, DWORD* effect) {
        if (this->canAcceptDrop) {
            if (keyState & MK_CONTROL)
                *effect = DROPEFFECT_COPY;
            else
                *effect = DROPEFFECT_MOVE;
        }
        else
            *effect = DROPEFFECT_NONE;
        return S_OK;
    }

    // FUNCTION: DELAYLAMA 0x10008900
    STDMETHODIMP DropTarget::DragLeave(void) {
        return S_OK;
    }

    // FUNCTION: DELAYLAMA 0x10008910
    STDMETHODIMP DropTarget::Drop(IDataObject* dataObject, DWORD keyState, POINTL pt, DWORD* effect) {
        // Hands dropped files (type 0) or text (type 1) to the view under the mouse
        if (this->parentFrame) {
            STGMEDIUM medium;
            FORMATETC formatTextDrop = {CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
            FORMATETC formatHDrop    = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

            long type = 0;
            HRESULT hr = dataObject->GetData(&formatTextDrop, &medium);
            if (hr == S_OK)
                type = 1;
            else
                hr = dataObject->GetData(&formatHDrop, &medium);

            if (hr == S_OK) {
                void* hDrop = medium.hGlobal;
                if (hDrop) {
                    switch (type) {
                        case 0: {
                            long nbOfItems = (long)DragQueryFileA((HDROP)hDrop, 0xFFFFFFFF, 0, 0);
                            char fileDropped[1024];
                            if (nbOfItems > 0) {
                                char** ptrItems = new char*[nbOfItems];
                                long itemIndex = 0;
                                long nbRealItems = 0;
                                while (itemIndex < nbOfItems) {
                                    if (DragQueryFileA((HDROP)hDrop, itemIndex, fileDropped, sizeof(fileDropped))) {
                                        // A shortcut is replaced by the file it points to
                                        checkResolveLink(fileDropped, fileDropped);
                                        ptrItems[nbRealItems] = new char[sizeof(fileDropped)];
                                        strcpy(ptrItems[nbRealItems], fileDropped);
                                        nbRealItems++;
                                    }
                                    itemIndex++;
                                }
                                POINT where = {0, 0};
                                this->parentFrame->getLocalMousePos(&where);
                                this->parentFrame->onDrop((void**)ptrItems, nbOfItems, 0, &where);
                                for (long i = 0; i < nbRealItems; i++)
                                    delete[] ptrItems[i];
                                delete[] ptrItems;
                            }
                        } break;
                        case 1: {
                            void* data = GlobalLock(medium.hGlobal);
                            long dataSize = (long)GlobalSize(medium.hGlobal);
                            if (data && dataSize) {
                                POINT where = {0, 0};
                                this->parentFrame->getLocalMousePos(&where);
                                this->parentFrame->onDrop(&data, dataSize, 1, &where);
                            }
                            GlobalUnlock(medium.hGlobal);
                            if (medium.pUnkForRelease)
                                medium.pUnkForRelease->Release();
                            else
                                GlobalFree(medium.hGlobal);
                        } break;
                    }
                }
            }
        }
        DragLeave();
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10008b60
    bool checkResolveLink(const char* nativePath, char* resolved) {
        // Resolves a dropped .lnk shortcut to its target path
        const char* ext = strrchr(nativePath, '.');
        if (ext && _stricmp(ext, ".lnk") == 0) {
            IShellLinkA* psl;
            IPersistFile* ppf;
            WIN32_FIND_DATAA wfd;
            HRESULT hres;
            WCHAR wsz[2048];

            hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&psl);
            if (SUCCEEDED(hres)) {
                hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
                if (SUCCEEDED(hres)) {
                    MultiByteToWideChar(CP_ACP, 0, nativePath, -1, wsz, 2048);
                    hres = ppf->Load(wsz, STGM_READ);
                    if (SUCCEEDED(hres)) {
                        hres = psl->Resolve(0, MAKELONG(SLR_ANY_MATCH | SLR_NO_UI, 500));
                        if (SUCCEEDED(hres))
                            hres = psl->GetPath(resolved, 2048, &wfd, SLGP_SHORTPATH);
                    }
                    ppf->Release();
                }
                psl->Release();
            }
            return SUCCEEDED(hres);
        }
        return false;
    }
}
}
}
}