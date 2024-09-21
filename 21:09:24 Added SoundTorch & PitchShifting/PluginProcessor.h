/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <strings.h>
#include "Bank.h"
#include "SoundTouch/SoundTouch.h"

//==============================================================================
/**
*/
class SampleChopperAudioProcessor  : public juce::AudioProcessor
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
    void loadURLS(juce::URL& url);
    
    juce::AudioFormatManager* getFormatManager();
    
    //receives a specific bank number
    Bank* getBank(int bank);
    
    //receives a list of pointers to all banks
    std::vector<Bank*> getBanksList();
    
    juce::AudioThumbnailCache thumbCache{20}; //used for the waveform
    
    Bank* getListenerBank()
    {
        Bank * listenerBankPointer = &listenerBank;
        return listenerBankPointer;
    }
    

private:
    
    int numberOfBanks = 6;
    
    std::vector<Bank*> bankList = {&bank1, &bank2, &bank3, &bank4, &bank5, &listenerBank}; //used to avoid repeating code
    
    Bank bank1{formatManager};
    Bank bank2{formatManager};
    Bank bank3{formatManager};
    Bank bank4{formatManager};
    Bank bank5{formatManager};
    
    Bank listenerBank{formatManager};
    
    juce::URL mainSample;
    
    juce::AudioFormatManager formatManager;
    
    juce::MixerAudioSource mixerSource;
    
    juce::String fileName;
    
    bool playing;
    
    bool fileFilled;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleChopperAudioProcessor)
};
