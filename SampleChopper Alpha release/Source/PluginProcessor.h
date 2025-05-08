/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SoundTouch.h"
#include <strings.h>
#include "Bank.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "PresetManager.h"

namespace ParameterID
{
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 2);

PARAMETER_ID(globalPitch);

    #undef PARAMETER_ID
}

//==============================================================================
/**
*/
class SampleChopperAudioProcessor  : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    //==============================================================================
    SampleChopperAudioProcessor();
    ~SampleChopperAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void setAsMainSample(juce::URL url); //sets the other banks with a single sample
    
   //banks
   // void loadURLS(juce::URL& url);
    
    juce::AudioFormatManager* getFormatManager();
    
    //banks
    void loadURLS(juce::URL& url); //accessed by editor from waveformdropped 
    
    //receives a specific bank number
    Bank* getBank(int bank);
    
    PresetManager& getPresetManager(){return *presetManager;};
    
    //receives a list of pointers to all banks
    std::vector<Bank*> getBanksList();
    
    juce::AudioThumbnailCache thumbCache{20}; //used for the waveform
    
    Bank* getListenerBank(){ Bank * listenerBankPointer = &listenerBank; return listenerBankPointer;};
    
    float getRawParameterValue(juce::String paramID);
    
    void setMonoState(bool mono);
    
    void clearLoops();
    
    juce::AudioProcessorValueTreeState apvts;
private:
    
    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages); //midibuffer and audio buffer
    void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2); //takes midi arguements as input
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);
    
    void monoProcessing(int bank);

    std::atomic<float>* globalPitchParameter = nullptr;
    std::atomic<float>* monoState = nullptr;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    
    
    //Banks
    int numberOfBanks = 7; //change to 9 (include listener)
    
    std::vector<Bank*> bankList = {&bank1, &bank2, &bank3, &bank4, &bank5, &bank6, &listenerBank}; //put listener at the end
    
    Bank bank1{formatManager, apvts, 1};
    Bank bank2{formatManager, apvts, 2};
    Bank bank3{formatManager, apvts, 3};
    Bank bank4{formatManager, apvts, 4};
    Bank bank5{formatManager, apvts, 5};
    Bank bank6{formatManager, apvts, 6};
    
    std::unique_ptr<PresetManager> presetManager; //by mkaing a unique pointer can defer intialisation to a later poiny
    

    Bank listenerBank{formatManager, apvts, 0};
    
    //Samples
    juce::URL mainSample;
    
    //Audio
    juce::AudioFormatManager formatManager;
    juce::MixerAudioSource mixerSource;
    
    juce::String fileName;
    
    bool playing;
    
    bool fileFilled;
    
    juce::UndoManager undoManager;
    juce::UndoManager * undoManagerPointer = &undoManager;
    
    juce::AudioParameterFloat* globalPitch;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleChopperAudioProcessor)
};
