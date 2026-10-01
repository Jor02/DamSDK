#pragma once
#include <damsdk/utils/portable_stdint.h>
#include "DamPlugin.h"
#include "AudioBase.h"

namespace DamSDK {
namespace Api {
    // VTABLE: DELAYLAMA 0x1000b148
    class AudioBase {
    public:
        float sampleRate;                // 0x04
        class EditorBase *editor;        // 0x08
        Api::dispatchFunc hostCallback;  // 0x0c
        int32_t presetCount;             // 0x10
        int32_t parameterCount;          // 0x14
        int32_t currentPreset;           // 0x18
        int32_t blockSize;               // 0x1c
        struct Api::DamPlugin plugin;    // 0x20
    public:
        AudioBase(Api::dispatchFunc hostCallback, uint32_t presetCount, uint32_t parameterCount);

        // Virtual functions are declared in the original vtable order (VTABLE 0x1000b148),
        // which follows the VST 2.x AudioEffect class. Do not reorder.
        virtual ~AudioBase();

        // -- Parameters / Automation --
        virtual void setParameterValue(int32_t parameterId, float value);
        virtual float getParameterValue(int32_t parameterId);
        virtual void automateHostParameter(int32_t parameterId, float value);

        // -- Core --
        virtual void invokeAudioProcess(float* * inputs, float* * outputs, int32_t sampleFrames) = 0;
        virtual void processAudio(float* * inputs, float* * outputs, int32_t sampleFrames);
        virtual int32_t dispatchPluginCallback(int32_t targetOperation, int32_t index, int32_t value, void * data, float optional);
        virtual void initializePlugin();
        virtual void shutdownPlugin();

        // -- Presets --
        virtual int32_t getActivePresetIndex();
        virtual void loadPresetByIndex(int32_t presetIndex);
        virtual void setCurrentPresetName(char* newName);
        virtual void getCurrentPresetName(char* outText);

        virtual void getParameterUnitLabel(int32_t parameterId, char* outText);
        virtual void getParameterValueString(int32_t parameterId, char* outText);
        virtual void getParameterName(int32_t parameterId, char* outBuffer);

        virtual float getVolume();

        virtual int32_t getPluginStateData(void* ptr, bool index);
        virtual int32_t setPluginStateData(void* ptr, int32_t value, bool index);

        // -- IO --
        virtual void setSampleRate(float sampleRate);
        virtual void setMaxFramesPerProcess(int32_t blockSize);

        virtual void disableAudioProcessing();
        virtual void enableAudioProcessing();

        // -- Plugin Properties --
        virtual void setPluginId(int32_t id);
        virtual void setInputChannelCount(int32_t count);
        virtual void setOutputChannelCount(int32_t count);
        virtual void setReportsLoudnessToHost(bool reportsLoudness);
        virtual void setHasClip(bool hasClip);
        virtual void setHasSoundOutput(bool hasOutput);
        virtual void setSupportsInPlaceProcessing(bool supportsInPlace);
        virtual void setProgramsAreChunks(bool programsAreChunks);
        virtual void setReservedValue(int32_t unusedValue);
        virtual void setOfflineQualities(int32_t qualities);
        virtual void setPluginProcessingTime(int32_t processingTime);

        virtual float getSampleRate();
        virtual int32_t getMaxFramesPerProcess();

        // -- Host Communication --
        virtual int32_t getHostApiVersion();
        virtual int32_t getHostUniqueId();
        virtual void sendIdleToHost();
        virtual bool isInputChannelConnected(int32_t channel);
        virtual bool isOutputChannelConnected(int32_t channel);

        // -- String Formatting --
        virtual void formatFloatAsDecibelString(float linearValue, char* outText);
        virtual void formatSamplesAsHzString(float sampleCount, char* outText);
        virtual void formatSamplesAsMsString(float sampleCount, char* outText);
        virtual void formatFloatToString(float value, char* outText);
        virtual void formatIntToString(int32_t value, char* text);

    };

    // formatSamplesAsHzString
#define HZ_THRESHOLD ((float)(0.0f))
}
}