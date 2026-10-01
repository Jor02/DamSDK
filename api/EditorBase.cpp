#include "EditorBase.h"
#include "AudioBaseExtended.h"
#include "damsdk/gui/platform/windows/Window.h"

namespace DamSDK {
namespace Api {

    // GLOBAL: DELAYLAMA 0x1000d874
    int GLOBAL_KNOB_MODE = 0;

    // GLOBAL: DELAYLAMA 0x1000d828
    MSG g_idleMessage;

    // -- EditorInterface (VST AEffEditor) default implementations --

    // FUNCTION: DELAYLAMA 0x10006710
    int32_t EditorInterface::getRect(Rect** outRect) {
        *outRect = 0;
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10006720
    int32_t EditorInterface::open(HWND hParent) {
        this->hParent = hParent;
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void EditorInterface::close() {}

    // FUNCTION: DELAYLAMA 0x10006730
    void EditorInterface::onIdle() {
        if (this->needsRedraw) {
            this->needsRedraw = 0;
            this->update();
        }
    }

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void EditorInterface::update() {}

    // FUNCTION: DELAYLAMA 0x10003730
    void EditorInterface::invalidate() {
        this->needsRedraw = 1;
    }

    // FUNCTION: DELAYLAMA 0x10003810 FOLDED
    int32_t EditorInterface::keyDown(KeyCode* keycode) { return -1; }

    // FUNCTION: DELAYLAMA 0x10003810 FOLDED
    int32_t EditorInterface::keyUp(KeyCode* keycode) { return -1; }

    // FUNCTION: DELAYLAMA 0x10006750 FOLDED
    int32_t EditorInterface::setKnobMode(int32_t mode) { return 0; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool EditorInterface::onMouseWheel(float wheelDelta) { return false; }

    // -- EditorBase (VSTGUI AEffGUIEditor) --

    // FUNCTION: DELAYLAMA 0x10006680
    EditorBase::EditorBase(AudioBaseExtended* plugin) : EditorInterface(plugin) {
        this->window = nullptr;
        this->isInIdleUpdate = false;

        // effect->setEditor(this)
        plugin->editor = this;
        if (this != nullptr)
            plugin->plugin.flags |= PluginFlags::HasEditor;
        else
            plugin->plugin.flags &= ~PluginFlags::HasEditor;

        this->hParent = nullptr;
        this->lastIdleTick = this->getTicks();
        OleInitialize(NULL);
    }

    // FUNCTION: DELAYLAMA 0x100068b0
    DWORD EditorBase::getTicks() {
        return GetTickCount();
    }

    // FUNCTION: DELAYLAMA 0x100067b0
    EditorBase::~EditorBase() {
        OleUninitialize();
    }

    // FUNCTION: DELAYLAMA 0x100067f0
    int32_t EditorBase::open(HWND hParent) {
        this->invalidate();
        this->hParent = hParent;
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10006940
    int32_t EditorBase::getRect(Rect** outRect) {
        *outRect = &this->rect;
        return 1;
    }

    // FUNCTION: DELAYLAMA 0x10006810
    void EditorBase::onIdle() {
        if (!this->isInIdleUpdate) {
            if (this->needsRedraw) {
                this->needsRedraw = false;
                this->update();
            }
            if (this->window != nullptr) {
                this->window->refresh();
            }
        }
    }

    
    // FUNCTION: DELAYLAMA 0x10006840
    int32_t EditorBase::setKnobMode(int32_t mode) {
        GLOBAL_KNOB_MODE = mode;
        return 1;
    }

    // FUNCTION: DELAYLAMA 0x10006ae0
    void EditorBase::dispatcher(int parameterIndex, float value) {
        this->invalidate();
    }

    // FUNCTION: DELAYLAMA 0x100067d0
    void EditorBase::draw(Rect* rect) {
        if (this->window != nullptr) {
            this->window->drawControlOrSelf(NULL);
        }
    }

    // FUNCTION: DELAYLAMA 0x100068c0
    void EditorBase::idleHandler() {
        DWORD currentTick = this->getTicks();

        this->onIdle();
        
        if (currentTick < this->lastIdleTick + 100) {
            this->sleep(4);
            currentTick += 4;
            if (currentTick < this->lastIdleTick + 50) {
                return;
            }
        }

        if (PeekMessage(&g_idleMessage, nullptr, WM_PAINT, WM_PAINT, PM_REMOVE))
            DispatchMessage(&g_idleMessage);
    
        this->lastIdleTick = currentTick;
        isInIdleUpdate = true;
        if (this->mainPlugin)
        {
            mainPlugin->sendIdleToHost();
        }
        isInIdleUpdate = false;
    }

    // FUNCTION: DELAYLAMA 0x10006860
    bool EditorBase::onMouseWheel(float wheelDelta) {
        if (this->window != nullptr) {
            POINT mousePos = {0, 0};
            return this->window->onMouseWheel(nullptr, &mousePos, wheelDelta);
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100068a0
    void EditorBase::sleep(DWORD milliseconds) {
        Sleep(milliseconds);
    }
}
}