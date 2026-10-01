#pragma once
#include <Windows.h>
#include <windef.h>
#include "DamPlugin.h"

// Forward declarations
namespace DamSDK {
    namespace Api {
        class AudioBaseExtended;
    }
    namespace Gui {
        namespace Platform {
            namespace Windows {
                class Window;
            }
        }
    }
}

namespace DamSDK {
namespace Api {

    extern int GLOBAL_KNOB_MODE;

    // VTABLE: DELAYLAMA 0x1000bbb8
    // Editor interface the host talks to (VST 2.x AEffEditor). Do not reorder the virtuals.
    class EditorInterface {
        public:
            AudioBaseExtended* mainPlugin;  // 0x04
            HWND hParent;                   // 0x08
            BOOL needsRedraw;               // 0x0c
        public:
            EditorInterface(AudioBaseExtended* plugin) {
                this->mainPlugin = plugin;
                this->needsRedraw = 0;
            }
            virtual ~EditorInterface() {}
            virtual int32_t getRect(Rect** outRect);
            virtual int32_t open(HWND hParent);
            virtual void close();
            virtual void onIdle();
            virtual void update();
            virtual void invalidate();
            virtual int32_t keyDown(KeyCode* keycode);
            virtual int32_t keyUp(KeyCode* keycode);
            virtual int32_t setKnobMode(int32_t mode);
            virtual bool onMouseWheel(float wheelDelta);
    };

    // VTABLE: DELAYLAMA 0x1000bb80
    // GUI editor base (VSTGUI 2.x AEffGUIEditor).
    class EditorBase : public EditorInterface {
        public:
            Rect rect;                                      // 0x10
            Gui::Platform::Windows::Window* window;         // 0x18
            DWORD lastIdleTick;                             // 0x1c
            bool isInIdleUpdate;                            // 0x20
        public:
            EditorBase(AudioBaseExtended* plugin);
            virtual ~EditorBase();
            virtual int32_t getRect(Rect** outRect) override;
            virtual int32_t open(HWND hParent) override;
            virtual void onIdle() override;
            virtual int32_t setKnobMode(int32_t mode) override;
            virtual bool onMouseWheel(float wheelDelta) override;

            // New virtual functions, in the original vtable order.
            virtual void dispatcher(int parameterIndex, float value);
            virtual void draw(Rect* rect);
            virtual void idleHandler();

            void sleep(DWORD milliseconds);
            DWORD getTicks();
    };
}
}