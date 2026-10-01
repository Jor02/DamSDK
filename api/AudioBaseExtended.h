#pragma once
#include <Windows.h>
#include <windef.h>
#include <damsdk/utils/portable_stdint.h>
#include "AudioBase.h"
#include "EditorBase.h"

namespace DamSDK {
namespace Api {

    // VTABLE: DELAYLAMA 0x1000b250
    class AudioBaseExtended : public AudioBase {
    public:
        AudioBaseExtended(dispatchFunc hostCallback, int32_t presetCount, int32_t parameterCount);
        ~AudioBaseExtended();

        virtual int32_t dispatchPluginCallback(int32_t targetOperation, int32_t index, int32_t value, void * data, float optional) override;

        // New virtual functions, declared in the original vtable order (VTABLE 0x1000b250),
        // which follows the VST 2.x AudioEffectX class. Do not reorder.

        // -- Events / Time --
        virtual EditorBase* getEditor();
        virtual void requestMidiSupport(int32_t value);
        virtual int32_t getHostTransportTime(int32_t flags);
        virtual int32_t getHostTempoAtSample(int32_t pos);
        virtual int32_t processEvents(void* eventList);

        // -- Parameters / Presets --
        virtual int32_t getHostAutomatableParameterCount();
        virtual int32_t getParameterStepSize();
        virtual bool canParameterBeAutomated(int32_t parameterId);
        virtual bool getParameter(int32_t index, char* text);
        virtual float getChannelParameter(int32_t channel, int32_t index);
        virtual int32_t getPresetCategories();
        virtual bool getPresetNameByIndex(int32_t category, int32_t index, char *outText);
        virtual bool copyPreset(int32_t presetIndex);

        // -- Host Connection --
        virtual bool notifyHostIoConfigurationChanged();
        virtual bool needIdle();
        virtual bool sizeWindow(int32_t width, int32_t height);
        virtual double getHostSampleRate();
        virtual uint32_t getBlockSize();
        virtual int32_t getHostInputLatencySamples();
        virtual int32_t getHostOutputLatencySamples();
        virtual int32_t getPreviousPlugin(int32_t input);
        virtual int32_t getNextPlugin(int32_t output);

        // -- Routing --
        virtual void connectInputBus(int32_t index, bool connected);
        virtual void connectOutputBus(int32_t index, bool connected);
        virtual bool getInputBusProperties(int32_t index,char *properties);
        virtual bool getOutputBusProperties(int32_t index,char *properties);
        virtual int32_t getPluginCategory();
        virtual int32_t willReplaceOrAccumulate();
        virtual int32_t getCurrentProcessLevel();
        virtual int32_t getAutomationState();
        virtual void setOverwritingCapability(bool canOverwrite);
        virtual void setOfflineProcessingCapability(bool enable);
        virtual int32_t getTransportInfo();
        virtual int32_t getOutputBuffer();

        // -- Offline Processing --
        virtual bool offlineRead(float** audioBuffers, int32_t sampleFrames, bool readSource);
        virtual bool offlineWrite(float** audioBuffers, int32_t option);
        virtual bool startOfflineProcessing(void* audioFiles, int32_t numAudioFiles, int32_t numNewAudioFiles);
        virtual int32_t getCurrentPass();
        virtual int32_t getCurrentMetaPass();
        virtual bool onOfflineNotify(void *data, int32_t value, bool isStarting);
        virtual bool prepareOffline(void *data, int32_t value);
        virtual bool runOffline(void *data, int32_t value);
        virtual int32_t getOfflinePassCount();
        virtual int32_t getOfflineMetaPassCount();
        virtual void setHostOutputSampleRate(float sampleRate);
        virtual bool getOutputSpeakerArrangement(int32_t arrangement, void* speakers);

        // -- Host Information --
        virtual bool getHostCompanyString(char* outText);
        virtual bool getHostProductString(char* outText);
        virtual int32_t getHostCompanyVersion();
        virtual intptr_t callCompanySpecific(int32_t index, int32_t valueHigh, float valueLow, void* context);
        virtual int32_t hostSupports(char* target);
        virtual void setIsSynthesizer(bool isSynthesizer);
        virtual void setNoTail(bool noTail);
        virtual int32_t getHostLanguage();
        virtual int32_t openWindow(HWND windowHandle);
        virtual bool closePluginEditorOnHost(void* windowHandle);
        virtual char* getHostWorkingDirectory();
        virtual bool requestHostEditorRefresh();

        // -- Plugin Information --
        virtual bool processVarIo(void *);
        virtual bool setOutputSpeakerArrangement(int32_t arrangement, void* speakers);
        virtual void setAudioSettings(int32_t hostBlockSize, float sampleRate);
        virtual bool setBypass(bool bypass);
        virtual bool getPluginName(char* outText);
        virtual bool getErrorText(char* outText);
        virtual bool getCompanyName(char* outText);
        virtual bool getProductName(char* outText);
        virtual int32_t getCompanyVersion();
        virtual int32_t companySpecific(int32_t index, int32_t value, void *data, float optional);
        virtual int32_t pluginSupports(char* target);
        virtual intptr_t getIcon();
        virtual bool setViewPosition(int32_t x, int32_t y);
        virtual int32_t getTailLengthSamples();
        virtual int32_t processIdle();
        virtual bool getParameterProperties(int32_t parameterId, void * data);
        virtual bool editorRequiresKeystroke();
        virtual int32_t getDamVersion();

        // -- MIDI Programs --
        virtual int32_t getMidiProgramName(int32_t index, char* outText);
        virtual int32_t getMidiProgram(int32_t index, void* out);
        virtual int32_t getMidiProgramCategory(int32_t index, char* outText);
        virtual bool hasMidiProgramChanged(int32_t index);
        virtual bool getMidiKeyName(int32_t index, char* outText);
        virtual bool beginSetMidiProgram();
        virtual bool endSetMidiProgram();

        // -- Parameter Editing / Files --
        virtual bool notifyHostClientBeginningParameterEdit(int32_t parameterId);
        virtual bool notifyHostClientEndingParameterEdit(int32_t parameterId);
        virtual bool openFileDialogOnHost(char* outText);
    };
}
}