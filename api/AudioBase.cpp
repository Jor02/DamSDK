#include <cmath>
#include <cstdio>
#include <cstring>
#include "AudioBase.h"
#include "EditorBase.h"

using namespace DamSDK;

namespace DamSDK {
namespace Api {

    // FUNCTION: DELAYLAMA 0x10001000
    static intptr_t _dispatcher(DamPlugin * plugin, int32_t targetOperation, int32_t index, intptr_t value, void * data, float optional) {
        AudioBase* audioBase = (AudioBase*) plugin->object;
        if (targetOperation == 1) {
            audioBase->dispatchPluginCallback(1,index,value,data,optional);
            if (audioBase != nullptr) {
                delete audioBase;
            }
            return 1;
        }
        return audioBase->dispatchPluginCallback(targetOperation,index,value,data,optional);
    }

    // FUNCTION: DELAYLAMA 0x10001060
    static float _getParameter(DamPlugin* plugin, int32_t parameterId) {
        AudioBase* audioBase = (AudioBase*) plugin->object;
        return audioBase->getParameterValue(parameterId);
    }

    // FUNCTION: DELAYLAMA 0x10001080
    static void _setParameter(DamPlugin* plugin, int32_t parameterId, float value) {
        AudioBase* audioBase = (AudioBase*) plugin->object;
        audioBase->setParameterValue(parameterId, value);
    }

    // FUNCTION: DELAYLAMA 0x100010a0
    static void _process(DamPlugin* plugin, float** inputs, float** outputs, int32_t sampleFrames) {
        AudioBase* audioBase = (AudioBase*) plugin->object;
        audioBase->invokeAudioProcess(inputs, outputs, sampleFrames);
    }

    // FUNCTION: DELAYLAMA 0x100010c0
    static void _processFloat(DamPlugin* plugin, float** inputs, float** outputs, int32_t sampleFrames) {
        AudioBase* audioBase = (AudioBase*) plugin->object;
        audioBase->processAudio(inputs, outputs, sampleFrames);
    }

    // Same order as the VST SDK's AudioEffect constructor.
    // FUNCTION: DELAYLAMA 0x100010e0
    AudioBase::AudioBase(dispatchFunc hostCallback, uint32_t presetCount, uint32_t parameterCount) {
        this->hostCallback = hostCallback;
        this->editor = nullptr;
        this->presetCount = presetCount;
        this->parameterCount = parameterCount;
        this->currentPreset = 0;

        memset(&this->plugin, 0, sizeof(DamPlugin));
        this->plugin.magicNumber = 'VstP';
        this->plugin.dispatcherFunc = _dispatcher;
        this->plugin.processingFunc = _process;
        this->plugin.settingParameterFunc = _setParameter;
        this->plugin.gettingParameterFunc = _getParameter;
        this->plugin.presetCount = presetCount;
        this->plugin.parameterCount = parameterCount;
        this->plugin.inputChannelCount = 1;
        this->plugin.outputChannelCount = 2;
        this->plugin.flags = 0;
        this->plugin.reserved1 = 0;
        this->plugin.reserved2 = 0;
        this->plugin.pluginProcessingTime = 0;  // initialDelay
        this->plugin.zero = 0;                  // realQualities
        this->plugin.offQualities = 0;
        this->plugin.ioRatio = 1.0f;
        this->plugin.object = this;
        this->plugin.user = nullptr;
        this->plugin.id = 'NoEf';
        this->plugin.version = 1;
        this->plugin.processAudioFloat = _processFloat;

        this->sampleRate = 44100.0f;
        this->blockSize = 1024;
    }

    // FUNCTION: DELAYLAMA 0x100011c0
    AudioBase::~AudioBase() {
        if (this->editor != nullptr)
            delete this->editor;
    }

    // FUNCTION: DELAYLAMA 0x100011e0
    int32_t AudioBase::dispatchPluginCallback(int32_t targetOperation, int32_t index, int32_t value, void * data, float optional)
    {
        int32_t v = 0;
        switch(targetOperation) {
            case pluginInitialize:
                this->initializePlugin();
                break;
            case pluginShutdown:
                this->shutdownPlugin();
                break;
            case pluginSetPresetByIndex:
                if ((int)value < presetCount) {
                    this->loadPresetByIndex(value);
                }
                break;
            case pluginGetCurrentPreset:
                v = this->getActivePresetIndex();
                break;
            case pluginSetCurrentPresetName:
                this->setCurrentPresetName((char*)data);
                break;
            case pluginGetCurrentPresetName:
                this->getCurrentPresetName((char*)data);
                break;
            case pluginGetParameterUnitLabel:
                this->getParameterUnitLabel(index, (char*)data);
                break;
            case pluginGetParameterValueText:
                this->getParameterValueString(index, (char*)data);
                break;
            case pluginGetParameterName:
                this->getParameterName(index, (char*)data);
                break;
            case pluginSetSampleRate:
                this->setSampleRate(optional);
                break;
            case pluginSetMaxFramesPerProcess:
                this->setMaxFramesPerProcess(value);
                break;
            case pluginSetProcessingState:
                if (value == 0) {
                    this->disableAudioProcessing();
                }
                else
                {
                    this->enableAudioProcessing();
                }
                break;
            case pluginGetVolume:
                v = static_cast<int32_t>(this->getVolume() * 32767.);
                break;
            case pluginGetEditorRect:
                if (editor != nullptr) {
                    v = editor->getRect((Rect**)data);
                }
                break;
            case pluginOpenEditor:
                if (editor != nullptr) {
                    v = editor->open((HWND)data);
                }
                break;
            case pluginCloseEditor:
                if (editor != nullptr) {
                    editor->close();
                }
                break;
            case pluginEditorIdling:
                if (editor != nullptr) {
                    editor->onIdle();
                }
                break;
            case pluginGetIdentifier:
                v = 'NvEf'; // 0x4e764566
                break;
            case pluginGetPresetData:
                v = this->getPluginStateData(data, index != NULL);
                break;
            case pluginSetPresetData:
                v = this->setPluginStateData(data, value, index != NULL);
                break;
        }
        return v;
    }

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void AudioBase::initializePlugin() {}

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void AudioBase::shutdownPlugin() {}

    // FUNCTION: DELAYLAMA 0x10001590
    void AudioBase::processAudio(float* * inputs, float* * outputs, int32_t sampleFrames) {}

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void AudioBase::disableAudioProcessing() {}

    // FUNCTION: DELAYLAMA 0x100015b0 FOLDED
    void AudioBase::enableAudioProcessing() {}

    // -- Parameters / Automation --
    // FUNCTION: DELAYLAMA 0x10001540 FOLDED
    void AudioBase::setParameterValue(int32_t parameterId, float value) {}

    // FUNCTION: DELAYLAMA 0x10001550
    float AudioBase::getParameterValue(int32_t parameterId) { return 0.f; }

    // FUNCTION: DELAYLAMA 0x10001a10
    void AudioBase::automateHostParameter(int32_t parameterId, float value) {
        this->setParameterValue(parameterId, value);
        if (this->hostCallback != nullptr)
            this->hostCallback(&this->plugin, hostAutomateParameter, parameterId, 0, nullptr, value);
    }

    // FUNCTION: DELAYLAMA 0x100016b0 FOLDED
    void AudioBase::getParameterUnitLabel(int32_t parameterId, char* outText) { *outText = '\0'; }

    // FUNCTION: DELAYLAMA 0x100016b0 FOLDED
    void AudioBase::getParameterValueString(int32_t parameterId, char* outText) { *outText = '\0';}

    // FUNCTION: DELAYLAMA 0x100016b0 FOLDED
    void AudioBase::getParameterName(int32_t parameterId, char* outText) { *outText = '\0'; }

    // -- Presets --
    // FUNCTION: DELAYLAMA 0x10001630
    int32_t AudioBase::getActivePresetIndex() { return this->currentPreset; }
    
    // FUNCTION: DELAYLAMA 0x10001640
    void AudioBase::loadPresetByIndex(int32_t presetIndex) { this->currentPreset = presetIndex; }
    
    // FUNCTION: DELAYLAMA 0x10001650 FOLDED
    void AudioBase::setCurrentPresetName(char* newName) { *newName = '\0'; }
    
    // FUNCTION: DELAYLAMA 0x10001650 FOLDED
    void AudioBase::getCurrentPresetName(char* outText) { *outText = '\0'; }

    // FUNCTION: DELAYLAMA 0x10001900 FOLDED
    int32_t AudioBase::getPluginStateData(void* ptr, bool index) { return 0; }
    
    // FUNCTION: DELAYLAMA 0x10001910
    int32_t AudioBase::setPluginStateData(void* ptr, int32_t value, bool index) { return 0; }
    
    // -- IO --
    // FUNCTION: DELAYLAMA 0x10001970
    float AudioBase::getSampleRate() { return this->sampleRate; }
    
    // FUNCTION: DELAYLAMA 0x10001920
    void AudioBase::setSampleRate(float sampleRate) { this->sampleRate = sampleRate; }
    
    // FUNCTION: DELAYLAMA 0x10001980
    int32_t AudioBase::getMaxFramesPerProcess() { return this->blockSize; }
    
    // FUNCTION: DELAYLAMA 0x10001930
    void AudioBase::setMaxFramesPerProcess(int32_t blockSize) { this->blockSize = blockSize; }

    // FUNCTION: DELAYLAMA 0x10001950
    void AudioBase::setInputChannelCount(int32_t count) { this->plugin.inputChannelCount = count; }
    
    // FUNCTION: DELAYLAMA 0x10001960
    void AudioBase::setOutputChannelCount(int32_t count) { this->plugin.outputChannelCount = count;}
    
    // FUNCTION: DELAYLAMA 0x10001490
    bool AudioBase::isOutputChannelConnected(int32_t channel) {
        int32_t result = 0;
        if (this->hostCallback != nullptr)
            result = this->hostCallback(&this->plugin, hostIsInputConnected, channel, 1, nullptr, 0.0f);
        return result ? false : true;
    }
    
    // FUNCTION: DELAYLAMA 0x10001460
    bool AudioBase::isInputChannelConnected(int32_t channel) {
        int32_t result = 0;
        if (this->hostCallback != nullptr)
            result = this->hostCallback(&this->plugin, hostIsInputConnected, channel, 0, nullptr, 0.0f);
        return result ? false : true;
    }
    
    // -- Plugin Properties --
    // FUNCTION: DELAYLAMA 0x10001940
    void AudioBase::setPluginId(int32_t id) { this->plugin.id = id; }
    
    // FUNCTION: DELAYLAMA 0x100014c0
    void AudioBase::setReportsLoudnessToHost(bool reportsLoudness) {
        if (reportsLoudness) this->plugin.flags |= PluginFlags::ReportsLoudnessToHost;
        else                 this->plugin.flags &= ~PluginFlags::ReportsLoudnessToHost;
    }
    
    // FUNCTION: DELAYLAMA 0x100014e0
    void AudioBase::setHasClip(bool hasClip) {
        if (hasClip) this->plugin.flags |= PluginFlags::HasClip;
        else         this->plugin.flags &= ~PluginFlags::HasClip;
    }
    
    // FUNCTION: DELAYLAMA 0x10001520
    void AudioBase::setSupportsInPlaceProcessing(bool supportsInPlace) {
        if (supportsInPlace) this->plugin.flags |= PluginFlags::SupportsInPlaceProcessing;
        else                 this->plugin.flags &= ~PluginFlags::SupportsInPlaceProcessing;
    }
    
    // FUNCTION: DELAYLAMA 0x10001560
    void AudioBase::setProgramsAreChunks(bool programsAreChunks) {
        if (programsAreChunks) this->plugin.flags |= PluginFlags::ProgramChunks;
        else                   this->plugin.flags &= ~PluginFlags::ProgramChunks;
    }
    
    // FUNCTION: DELAYLAMA 0x10001500
    void AudioBase::setHasSoundOutput(bool hasOutput) {
        if (hasOutput) this->plugin.flags |= PluginFlags::CanMono;
        else           this->plugin.flags &= ~PluginFlags::CanMono;
    }

    // FUNCTION: DELAYLAMA 0x100015a0
    void AudioBase::setOfflineQualities(int32_t qualities) { this->plugin.offQualities = qualities; }
    
    // FUNCTION: DELAYLAMA 0x100015c0
    void AudioBase::setPluginProcessingTime(int32_t processingTime) { this->plugin.pluginProcessingTime = processingTime; }

    // FUNCTION: DELAYLAMA 0x10001700
    float AudioBase::getVolume() { return 0.f; }
    
    // -- Host Communication --
    // FUNCTION: DELAYLAMA 0x100013f0
    int32_t AudioBase::getHostApiVersion() {
        int32_t version = 1;
        if (this->hostCallback != nullptr) {
            version = this->hostCallback(&this->plugin, hostGetApiVersion, 0, NULL, nullptr, 0.0);
            if (version == 0)
                version = 1;
        }
        return version;
    }
    
    // FUNCTION: DELAYLAMA 0x10001420
    int32_t AudioBase::getHostUniqueId() {
        int32_t id = 0;
        if (this->hostCallback != nullptr)
            id = this->hostCallback(&this->plugin, hostGetHostId, 0, NULL, nullptr, 0.0);
        return id;
    }
    
    // FUNCTION: DELAYLAMA 0x10001440
    void AudioBase::sendIdleToHost() {
        if (this->hostCallback == nullptr)
            return;
        this->hostCallback(&this->plugin, hostProcessIdle, 0, NULL, nullptr, 0.0);
    }

    // -- String Formatting --
    // FUNCTION: DELAYLAMA 0x100015d0
    void AudioBase::formatFloatAsDecibelString(float value, char* text) {
        if (value <= 0)
            strcpy(text, "  -oo   ");
        else
            this->formatFloatToString((float)(20. * log10(value)), text);
    }
    
    // FUNCTION: DELAYLAMA 0x10001660
    void AudioBase::formatSamplesAsHzString(float sampleCount, char* outText) {
        float sampleRate = this->getSampleRate();

        if (sampleCount == HZ_THRESHOLD) {
            this->formatFloatToString(0.0f, outText);
        } else {
            float hz = sampleRate / sampleCount;
            this->formatFloatToString(hz, outText);
        }
    }
    
    // FUNCTION: DELAYLAMA 0x100016c0
    void AudioBase::formatSamplesAsMsString(float samples, char* text) {
        this->formatFloatToString((float)(samples * 1000. / this->getSampleRate()), text);
    }
    
    // Same algorithm as the VST SDK's float2string, including its quirk of
    // copying " Huge!  " into the local buffer instead of the output.
    // FUNCTION: DELAYLAMA 0x10001710
    void AudioBase::formatFloatToString(float value, char* text) {
        long c = 0, neg = 0;
        char string[32];
        char* s;
        double v, integ, i10, mantissa, m10, ten = 10.;

        v = (double)value;
        if (v < 0) {
            neg = 1;
            value = -value;
            v = -v;
            c++;
            if (v > 9999999.) {
                strcpy(string, " Huge!  ");
                return;
            }
        }
        else if (v > 99999999.) {
            strcpy(string, " Huge!  ");
            return;
        }

        s = string + 31;
        *s-- = 0;
        *s-- = '.';
        c++;

        integ = floor(v);
        i10 = fmod(integ, ten);
        *s-- = (char)((long)i10 + '0');
        integ /= ten;
        c++;
        while (integ >= 1. && c < 8) {
            i10 = fmod(integ, ten);
            *s-- = (char)((long)i10 + '0');
            integ /= ten;
            c++;
        }
        if (neg)
            *s-- = '-';
        strcpy(text, s + 1);
        if (c >= 8)
            return;

        s = string + 31;
        *s-- = 0;
        mantissa = fmod(v, 1.);
        mantissa *= pow(ten, (double)(8 - c));
        while (c < 8) {
            if (mantissa <= 0)
                *s-- = '0';
            else {
                m10 = fmod(mantissa, ten);
                *s-- = (char)((long)m10 + '0');
                mantissa /= 10.;
            }
            c++;
        }
        strcat(text, s + 1);
    }
    
    // FUNCTION: DELAYLAMA 0x10001990
    void AudioBase::formatIntToString(int32_t value, char* text) {
        char string[32];

        if (value >= 100000000) {
            strcpy(text, " Huge!  ");
            return;
        }
        sprintf(string, "%7d", value);
        string[8] = 0;
        strcpy(text, string);
    }

    // -- Unused --
    // FUNCTION: DELAYLAMA 0x10001580
    void AudioBase::setReservedValue(int32_t unusedValue) {
        this->plugin.zero = unusedValue;
    }
}
}