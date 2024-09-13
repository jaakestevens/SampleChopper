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

class Bank : public juce::AudioSource
{
public:
    Bank(juce::AudioFormatManager& afm);
    ~Bank();
    bool loadURL(const juce::URL& url);
    void play(); void stop();
    void setPosition(double posInSecs); 
    void setPositionRelative(const double pos);
    void setGain(double gain);
    
    void prepareToPlay(int samplesPerBlockExpected , double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    bool isURLLoaded();
    float getPositionRelative();
    void setLoopRegion(float start, float end);
    void setAdsrParameters(juce::ADSR::Parameters myParams);
    juce::ADSR::Parameters* getAdsrParameters();
    void setAdsrDisplay(bool state);
    bool getAdsrDisplay();

    
    Interval<float> loopRegion;
    
    
    private:
    
    juce::AudioFormatManager& formatManager;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    bool paused;
    bool fileLoaded = false;
    std::unique_ptr <juce::AudioFormatReaderSource > readerSource;
    juce::AudioTransportSource transportSource;
    
    bool adsrDisplayToggle = true;
};
