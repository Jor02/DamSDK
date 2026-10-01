#include "AudioBaseExtended.h"
#include "EditorBase.h"

namespace DamSDK {
namespace Api {
    // FUNCTION: DELAYLAMA 0x10001a50
    AudioBaseExtended::AudioBaseExtended(dispatchFunc hostCallback, int32_t presetCount, int32_t parameterCount) : AudioBase(hostCallback, presetCount, parameterCount) {}
    
    // FUNCTION: DELAYLAMA 0x10001aa0
    AudioBaseExtended::~AudioBaseExtended() {}

    // FUNCTION: DELAYLAMA 0x10001ab0
    int32_t AudioBaseExtended::dispatchPluginCallback(int32_t targetOperation, int32_t index, int32_t value, void *data, float optional) {
        // Same shape as the VST 2.x AudioEffectX dispatcher: one result, cases in opcode order.
        int32_t v = 0;
        switch (targetOperation) {
            case pluginProcessEvents:
                v = this->processEvents(data);
                break;
            case pluginParameterSupportsAutomation:
                v = this->canParameterBeAutomated(index) ? 1 : 0;
                break;
            case pluginGetParameterNameByIndex:
                v = this->getParameter(index, (char*)data) ? 1 : 0;
                break;
            case pluginGetPresetCategoryCount:
                v = this->getPresetCategories();
                break;
            case pluginGetPresetNameByIndex:
                v = this->getPresetNameByIndex(value, index, (char*)data) ? 1 : 0;
                break;
            case pluginCopyPreset:
                v = this->copyPreset(index) ? 1 : 0;
                break;
            case pluginConnectInputChannel:
                this->connectInputBus(index, value != 0);
                v = 1;
                break;
            case pluginConnectOutputChannel:
                this->connectOutputBus(index, value != 0);
                v = 1;
                break;
            case pluginGetPluginInputSettings:
                v = this->getInputBusProperties(index, (char*)data) ? 1 : 0;
                break;
            case pluginGetPluginOutputSettings:
                v = this->getOutputBusProperties(index, (char*)data) ? 1 : 0;
                break;
            case pluginGetPluginCategory:
                v = this->getPluginCategory();
                break;
            case pluginGetCurrentSamplePosition:
                v = this->getTransportInfo();
                break;
            case pluginGetOutputBuffer:
                v = this->getOutputBuffer();
                break;
            case pluginNotifyOffline:
                v = this->onOfflineNotify(data, value, index != 0);
                break;
            case pluginPrepareOfflineRun:
                v = this->prepareOffline(data, value);
                break;
            case pluginRunOffline:
                v = this->runOffline(data, value);
                break;
            case pluginSetSpeakerArrangement:
                v = this->setOutputSpeakerArrangement(value, data) ? 1 : 0;
                break;
            case pluginProcessVarIo:
                v = this->processVarIo(data) ? 1 : 0;
                break;
            case pluginSetAudioSettings:
                this->setAudioSettings(value, optional);
                v = 1;
                break;
            case pluginSetBypassState:
                v = this->setBypass(value != 0) ? 1 : 0;
                break;
            case pluginGetPluginName:
                v = this->getPluginName((char*)data) ? 1 : 0;
                break;
            case pluginGetErrorText:
                v = this->getErrorText((char*)data) ? 1 : 0;
                break;
            case pluginGetCompanyName:
                v = this->getCompanyName((char*)data) ? 1 : 0;
                break;
            case pluginGetProductName:
                v = this->getProductName((char*)data) ? 1 : 0;
                break;
            case pluginGetCompanyVersion:
                v = this->getCompanyVersion();
                break;
            case pluginSpecific:
                v = this->companySpecific(index, value, data, optional);
                break;
            case pluginSupportsFeature:
                v = this->pluginSupports((char*)data);
                break;
            case pluginGetIcon:
                v = (int32_t)this->getIcon();
                break;
            case pluginSetViewPosition:
                v = this->setViewPosition(index, value) ? 1 : 0;
                break;
            case pluginGetTailLength:
                v = this->getTailLengthSamples();
                break;
            case pluginIdling:
                v = this->processIdle();
                break;
            case pluginGetParameterSettings:
                v = this->getParameterProperties(index, data) ? 1 : 0;
                break;
            case pluginRequiresKeys:
                v = this->editorRequiresKeystroke() ? 0 : 1;
                break;
            case pluginGetDamVersion:
                v = this->getDamVersion();
                break;
            case pluginEditorKeyDownEvent:
                if (this->editor != nullptr) {
                    KeyCode keyCode;
                    keyCode.asciiCharacter = index;
                    keyCode.vkValue = (unsigned char)value;
                    keyCode.modifiers = (unsigned char)optional;
                    v = this->editor->keyDown(&keyCode);
                }
                break;
            case pluginEditorKeyUpEvent:
                if (this->editor != nullptr) {
                    KeyCode keyCode;
                    keyCode.asciiCharacter = index;
                    keyCode.vkValue = (unsigned char)value;
                    keyCode.modifiers = (unsigned char)optional;
                    v = this->editor->keyUp(&keyCode);
                }
                break;
            case pluginSetKnobMode:
                if (this->editor != nullptr)
                    v = this->editor->setKnobMode(value);
                break;
            case pluginGetMidiPresetName:
                v = this->getMidiProgramName(index, (char*)data);
                break;
            case pluginGetMidiProgram:
                v = this->getMidiProgram(index, data);
                break;
            case pluginGetCategoryOfMidiProgram:
                v = this->getMidiProgramCategory(index, (char*)data);
                break;
            case pluginHasMidiProgramChanged:
                v = this->hasMidiProgramChanged(index) ? 1 : 0;
                break;
            case pluginNameOfMidiKey:
                v = this->getMidiKeyName(index, (char*)data) ? 1 : 0;
                break;
            case pluginStartSettingMidiProgram:
                v = this->beginSetMidiProgram() ? 1 : 0;
                break;
            case pluginStopSettingMidiProgram:
                v = this->endSetMidiProgram() ? 1 : 0;
                break;
            default:
                v = AudioBase::dispatchPluginCallback(targetOperation, index, value, data, optional);
                break;
        }
        return v;
    }

    // --- Editor & UI ---
    // FUNCTION: DELAYLAMA 0x10002040
    EditorBase* AudioBaseExtended::getEditor() { return this->editor; }

    // FUNCTION: DELAYLAMA 0x100026d0
    int32_t AudioBaseExtended::openWindow(HWND windowHandle) {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostOpenPluginEditor, 0, NULL, windowHandle, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002700
    bool AudioBaseExtended::closePluginEditorOnHost(void* windowHandle) {
        if (this->hostCallback != nullptr) {
            int32_t output = this->hostCallback(&this->plugin, hostClosePluginEditor, 0, NULL, windowHandle, 0.0f);
            return output != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10002750
    bool AudioBaseExtended::requestHostEditorRefresh() {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostRequestEditorRefresh, 0, 0, nullptr, 0.0f) ? true : false;
        return false;
    }
    
    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::setViewPosition(int32_t x, int32_t y) { return false; }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    intptr_t AudioBaseExtended::getIcon() { return 0; }

    // --- MIDI & Events ---
    // FUNCTION: DELAYLAMA 0x10002060
    void AudioBaseExtended::requestMidiSupport(int32_t value) {
        if (this->hostCallback == nullptr)
            return;
        this->hostCallback(&this->plugin, hostSupportsMidiInput, 0, value, nullptr, 0.0f);
    }

    // FUNCTION: DELAYLAMA 0x10006750 FOLDED
    int32_t AudioBaseExtended::processEvents(void * data) { return 0; }

    // FUNCTION: DELAYLAMA 0x10002380
    int32_t AudioBaseExtended::getMidiProgram(int32_t index, void* out) { return -1; }

    // FUNCTION: DELAYLAMA 0x10001900 FOLDED
    int32_t AudioBaseExtended::getMidiProgramName(int32_t index,char* outText) { return 0; }

    // FUNCTION: DELAYLAMA 0x10001900 FOLDED
    int32_t AudioBaseExtended::getMidiProgramCategory(int32_t index, char* outText) { return 0; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::hasMidiProgramChanged(int32_t index) { return false; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::getMidiKeyName(int32_t index, char* outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10002300 FOLDED
    bool AudioBaseExtended::beginSetMidiProgram() { return false; }

    // FUNCTION: DELAYLAMA 0x10002300 FOLDED
    bool AudioBaseExtended::endSetMidiProgram() { return false; }

    // --- Advanced Host Communication ---
    // FUNCTION: DELAYLAMA 0x100020a0
    int32_t AudioBaseExtended::getHostTransportTime(int32_t flags) {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetTransportTimeInfo, 0, flags, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x100020d0
    int32_t AudioBaseExtended::getHostTempoAtSample(int32_t pos) {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetTempoAtSamplePosition, 0, pos, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002730
    char* AudioBaseExtended::getHostWorkingDirectory() {
        if (this->hostCallback != nullptr) {
            return (char *)this->hostCallback(&this->plugin, hostGetHostWorkingDirectory, 0, NULL, nullptr, 0.0f);
        }
        return nullptr;
    }

    // FUNCTION: DELAYLAMA 0x100027e0
    bool AudioBaseExtended::openFileDialogOnHost(char* outText) {
        if (this->hostCallback != nullptr && outText != nullptr)
            return this->hostCallback(&this->plugin, hostOpenFileDialog, 0, 0, outText, 0.0f) ? true : false;
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100026b0
    int32_t AudioBaseExtended::getHostLanguage() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetHostLanguage, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002590
    bool AudioBaseExtended::getHostCompanyString(char* outText) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostGetHostCompanyName, 0, NULL, outText, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100025c0
    bool AudioBaseExtended::getHostProductString(char* outText) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostGetHostProductName, 0, NULL, outText, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100025f0
    int32_t AudioBaseExtended::getHostCompanyVersion() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetHostVersion, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002640
    int32_t AudioBaseExtended::hostSupports(char* target) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostSupportsFeature, 0, 0, target, 0.0f) != 0;
        return 0;
    }

    // -- Advanced Parameters --
    // FUNCTION: DELAYLAMA 0x10002150
    int32_t AudioBaseExtended::getHostAutomatableParameterCount() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetAutomatableParameterCount, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002170
    int32_t AudioBaseExtended::getParameterStepSize() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetParameterStepSize, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10004670 FOLDED
    bool AudioBaseExtended::canParameterBeAutomated(int32_t parameterId) { return true; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::getParameter(int32_t index, char* text) { return false; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::getParameterProperties(int32_t parameterId, void * data) { return false; }

    // FUNCTION: DELAYLAMA 0x10002780
    bool AudioBaseExtended::notifyHostClientBeginningParameterEdit(int32_t parameterId) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostBeginParameterEdit, parameterId, 0, nullptr, 0.0f) ? true : false;
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100027b0
    bool AudioBaseExtended::notifyHostClientEndingParameterEdit(int32_t parameterId) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostEndParameterEdit, parameterId, 0, nullptr, 0.0f) ? true : false;
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100023d0
    int32_t AudioBaseExtended::getAutomationState() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetAutomationPlaybackState, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // -- Advanced Presets --
    // FUNCTION: DELAYLAMA 0x10002080 FOLDED
    int32_t AudioBaseExtended::getPresetCategories() { return 1; }

    // FUNCTION: DELAYLAMA 0x10002090 FOLDED
    bool AudioBaseExtended::getPresetNameByIndex(int32_t category, int32_t index, char *outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::copyPreset(int32_t presetIndex) { return false; }

    // -- Routing --
    // FUNCTION: DELAYLAMA 0x10002190
    bool AudioBaseExtended::notifyHostIoConfigurationChanged() {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostNotifyIOConfigurationChanged, 0, NULL, nullptr, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100022b0
    int32_t AudioBaseExtended::getHostInputLatencySamples() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetInputLatencySamples, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x100022e0
    int32_t AudioBaseExtended::getHostOutputLatencySamples() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetOutputLatencySamples, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10001540 FOLDED
    void AudioBaseExtended::connectInputBus(int32_t index, bool connected) {}

    // FUNCTION: DELAYLAMA 0x10001540 FOLDED
    void AudioBaseExtended::connectOutputBus(int32_t index, bool connected) {}

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::getInputBusProperties(int32_t index,char *properties) { return false; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::getOutputBusProperties(int32_t index,char *properties) { return false; }

    // FUNCTION: DELAYLAMA 0x10002560
    bool AudioBaseExtended::getOutputSpeakerArrangement(int32_t arrangement, void* speakers) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostGetOutputChannelLayout, 0, arrangement, speakers, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::setOutputSpeakerArrangement(int32_t arrangement, void* speakers) { return false; }

    // FUNCTION: DELAYLAMA 0x100021c0
    void AudioBaseExtended::setAudioSettings(int32_t hostBlockSize, float sampleRate) {
        this->blockSize = hostBlockSize;
        this->sampleRate = sampleRate;
    }

    // FUNCTION: DELAYLAMA 0x10002540
    void AudioBaseExtended::setHostOutputSampleRate(float sampleRate) {
        if (this->hostCallback == nullptr)
            return;
        this->hostCallback(&this->plugin, hostSetOutputSampleRate, 0, NULL, nullptr, sampleRate);
    }

    // FUNCTION: DELAYLAMA 0x10002240
    double AudioBaseExtended::getHostSampleRate() {
        // Ask the host to update our sample rate, then return it.
        if (this->hostCallback != nullptr)
            this->hostCallback(&this->plugin, hostGetSampleRate, 0, 0, nullptr, 0.0f);
        return this->sampleRate;
    }

    // -- Offline Processing --
    // FUNCTION: DELAYLAMA 0x10002410
    void AudioBaseExtended::setOfflineProcessingCapability(bool enable) {
        if (enable) this->plugin.flags |= PluginFlags::SupportsOfflineProcessing;
        else        this->plugin.flags &= ~PluginFlags::SupportsOfflineProcessing;
    }

    // FUNCTION: DELAYLAMA 0x100024a0
    bool AudioBaseExtended::startOfflineProcessing(void* audioFiles, int32_t numAudioFiles, int32_t numNewAudioFiles) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostBeginOfflineProcessing, numNewAudioFiles, numAudioFiles, audioFiles, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10002430
    bool AudioBaseExtended::offlineRead(float** audioBuffers, int32_t sampleFrames, bool readSource) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostOfflineReadAudio, readSource, sampleFrames, audioBuffers, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10002470
    bool AudioBaseExtended::offlineWrite(float** audioBuffers, int32_t option) {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostOfflineWriteAudio, 0, option, audioBuffers, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10002090 FOLDED
    bool AudioBaseExtended::onOfflineNotify(void *data, int32_t value, bool index) { return false; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::prepareOffline(void *data, int32_t value) { return false; }

    // FUNCTION: DELAYLAMA 0x100022d0 FOLDED
    bool AudioBaseExtended::runOffline(void *data, int32_t value) { return false; }

    // FUNCTION: DELAYLAMA 0x100024e0
    int32_t AudioBaseExtended::getCurrentPass() {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostGetOfflineProcessingPassIndex, 0, 0, nullptr, 0.0f);
            return result != NULL;
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002510
    int32_t AudioBaseExtended::getCurrentMetaPass() {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostGetOfflineProcessingSubPassIndex, 0, 0, nullptr, 0.0f);
            return result != NULL;
        }
        return 0;
    }

    // -- Plugin Metadata --
    // FUNCTION: DELAYLAMA 0x10002100
    int32_t AudioBaseExtended::getPluginCategory() {
        // Return 2 for synthesizer, 0 for audio effect
        return (this->plugin.flags & PluginFlags::IsSynthesizer) ? 2 : 0;
    }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::getPluginName(char* outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::getErrorText(char* outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::getCompanyName(char* outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::getProductName(char* outText) { return false; }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getCompanyVersion() { return 0; }

    // FUNCTION: DELAYLAMA 0x10002340
    int32_t AudioBaseExtended::getDamVersion() { return 2200; }

    // FUNCTION: DELAYLAMA 0x10006750 FOLDED
    int32_t AudioBaseExtended::pluginSupports(char* target) { return 0; }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getOfflinePassCount() { return 0; }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getOfflineMetaPassCount() { return 0; }
    
    // FUNCTION: DELAYLAMA 0x10002670
    void AudioBaseExtended::setIsSynthesizer(bool isSynthesizer) {
        if (isSynthesizer) this->plugin.flags |= PluginFlags::IsSynthesizer;
        else               this->plugin.flags &= ~PluginFlags::IsSynthesizer;
    }

    // FUNCTION: DELAYLAMA 0x10002690
    void AudioBaseExtended::setNoTail(bool noTail) {
        if (noTail) this->plugin.flags |= PluginFlags::NoSoundInStop;
        else        this->plugin.flags &= ~PluginFlags::NoSoundInStop;
    }

    // FUNCTION: DELAYLAMA 0x100023f0
    void AudioBaseExtended::setOverwritingCapability(bool canOverwrite) {
        if (canOverwrite) this->plugin.flags |= PluginFlags::CanOverwrite;
        else              this->plugin.flags &= ~PluginFlags::CanOverwrite;
    }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::setBypass(bool bypaWss) { return false; }

    // FUNCTION: DELAYLAMA 0x10002390
    int32_t AudioBaseExtended::willReplaceOrAccumulate() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostWillProcessInPlace, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x100023b0
    int32_t AudioBaseExtended::getCurrentProcessLevel() {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostGetProcessingContextLevel, 0, NULL, nullptr, 0.0f);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getTailLengthSamples() { return 0; }

    // -- Workflow --
    // FUNCTION: DELAYLAMA 0x10002210
    bool AudioBaseExtended::sizeWindow(int32_t width, int32_t height) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostResizeEditorWindow, width, height, nullptr, 0.0f) != 0;
        return false;
    }

    // FUNCTION: DELAYLAMA 0x100021e0
    bool AudioBaseExtended::needIdle() {
        if (this->hostCallback != nullptr) {
            int32_t result = this->hostCallback(&this->plugin, hostRequestIdleProcessing, 0, 0, nullptr, 0.0f);
            return result != NULL;
        }
        return false;
    }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::processIdle() { return 0; }

    // FUNCTION: DELAYLAMA 0x10002300 FOLDED
    bool AudioBaseExtended::editorRequiresKeystroke() { return false; }

    // FUNCTION: DELAYLAMA 0x10002310
    int32_t AudioBaseExtended::getPreviousPlugin(int32_t input) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostGetPreviousPluginInstance, 0, 0, nullptr, 0.0f);
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002350
    int32_t AudioBaseExtended::getNextPlugin(int32_t output) {
        if (this->hostCallback != nullptr)
            return this->hostCallback(&this->plugin, hostGetNextPluginInstance, 0, 0, nullptr, 0.0f);
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getTransportInfo() { return 0; }

    // FUNCTION: DELAYLAMA 0x10002140 FOLDED
    int32_t AudioBaseExtended::getOutputBuffer() { return 0; }

    // FUNCTION: DELAYLAMA 0x10006760 FOLDED
    bool AudioBaseExtended::processVarIo(void *) { return false; }

    // FUNCTION: DELAYLAMA 0x10002270
    int32_t AudioBaseExtended::companySpecific(int32_t index, int32_t value, void *data, float optional) { return 0; }

    // FUNCTION: DELAYLAMA 0x10002610
    intptr_t AudioBaseExtended::callCompanySpecific(int32_t index, int32_t valueHigh, float valueLow, void* context) {
        if (this->hostCallback != nullptr) {
            return this->hostCallback(&this->plugin, hostHandleCompanySpecific, index, valueHigh, context, valueLow);
        }
        return 0;
    }

    // FUNCTION: DELAYLAMA 0x10002280
    uint32_t AudioBaseExtended::getBlockSize() {
        if (this->hostCallback != nullptr)
            this->hostCallback(&this->plugin, hostGetMaxFramesPerProcess, 0, 0, nullptr, 0.0f);
        return this->blockSize;
    }

    // AudioEffectX::getChannelParameter (not used by Delay Lama)
    // FUNCTION: DELAYLAMA 0x10002050
    float AudioBaseExtended::getChannelParameter(int32_t channel, int32_t index) { return 0.f; }
}
}