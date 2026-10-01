#include <windows.h>
#include <ole2.h>
#include <objidl.h>
#include "damsdk/utils/portable_stdint.h"

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
    // VTABLE: DELAYLAMA 0x1000bcb4
    // COM drop target (VSTGUI 2.x CDropTarget): IDropTarget's methods, then the destructor.
    class DropTarget : public IDropTarget {
        long refCount;          // 0x04
        bool canAcceptDrop;     // 0x08
        Window *parentFrame;    // 0x0c
    public:
        DropTarget(Window* frame);
        virtual ~DropTarget();

        STDMETHOD (QueryInterface) (REFIID riid, void** object);
        STDMETHOD_ (ULONG, AddRef) (void);
        STDMETHOD_ (ULONG, Release) (void);
        STDMETHOD (DragEnter) (IDataObject* dataObject, DWORD keyState, POINTL pt, DWORD* effect);
        STDMETHOD (DragOver) (DWORD keyState, POINTL pt, DWORD* effect);
        STDMETHOD (DragLeave) (void);
        STDMETHOD (Drop) (IDataObject* dataObject, DWORD keyState, POINTL pt, DWORD* effect);

    };

    DropTarget* createDropTarget(Window* frame);
    bool checkResolveLink(const char* nativePath, char* resolved);
}
}
}
}