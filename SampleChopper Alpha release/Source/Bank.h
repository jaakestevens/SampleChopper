/*
  ==============================================================================

    Bank.h
    Created: 6 Sep 2024 6:17:24pm
    Author:  Jake

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Interval.h"
#include "SoundTouch/SoundTouch.h"

class Bank : public juce::AudioSource
{
public:
    Bank(juce::AudioFormatManager& afm, juce::AudioProcessorValueTreeState& vts, int bankNumber);
    ~Bank();
    bool loadURL(const juce::URL& url);
    void play(); void stop();
    void setPosition(double posInSecs);
    void setPositionRelative(const double pos);
    void setGain(double gain);
    float getPositionRelative();
    
    void prepareToPlay(int samplesPerBlockExpected , double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    bool isURLLoaded();
    
    //ADSR
    void setAdsrParameters(juce::ADSR::Parameters myParams);
    juce::ADSR::Parameters* getAdsrParameters();
   
    //Panning
    void setPanning(float panValue);
    
    //bankType
    float getBankNumber(){return bankNumber;}
    void makeBankListener(bool listenerBank){isListenerBank = listenerBank;}
    
    
    //Loop region
    void setLoopRegion(float start, float end);
    Interval<float> getLoopRegion(){return loopRegion;}
    
    //Fade
    void setFadeVal(float inc);
    void applyShortFadeOut(const juce::AudioSourceChannelInfo& bufferToFill);
    void calculateFade();
    
    //Pitch and speed
    void changeSoundTouchPitch(float change ){bankPitch = 1 + change;}
    void updatePitchAndSpeed(float pitch, float speed);
    
    //Returns transport source
    juce::AudioTransportSource * getTransportSource(){return &transportSource;};
    
    void initialiseParams(float pitch, float speed, float vtsAttack, float release, float volume, float panning);
    
    std::function<void(int)> sampleTriggeredCallback;
    
    bool isBankPlaying(){return transportSource.isPlaying();};
    
    void setTransients(std::vector<float> detectedTransients){transients = detectedTransients;};
    
    std::vector<float> getTransients(){return transients;};
    
    void setMasterGain(float masterGain);

    

    private:
    
    //Audio Processing
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager& formatManager;
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr <juce::AudioFormatReaderSource > readerSource;
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
    
    std::vector<float> transients; //time stamps in seconds;
    
    //Loop Region
    Interval<float> loopRegion;
    
    //Fade in
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    //Fade Out
    float fadeOutIncrement = 1.0f;
    float fadeOutMultiple;
    bool fading = false;
    
    //Info
    float transportPosPercent;
    bool paused;
    bool fileLoaded = false;
    
    //BankType
    bool isListenerBank = false;
    int bankNumber;
    
    //Panning
    float panPosition = 0;
    void applyPan(const juce::AudioSourceChannelInfo& bufferToFill, float panningVal);
    
    //pitchShifting
    void bufferPitchShift(const juce::AudioSourceChannelInfo& bufferToFill);
    soundtouch::SoundTouch pitchShifter;
    std::vector<float>interleavedBuffer; //declared on stack rather than created in audio thread
    bool pitchShiftLoaded;
    float bankPitch = 1.0f;
    juce::String pitchID = "bank" + juce::String(bankNumber) + "Pitch";
    float pitchChangeDueToSpeed = 0.f;
    
    //Volume
    float masterVolume = 1.0f;
    float bankGain = 1.0f;
    
    
};
