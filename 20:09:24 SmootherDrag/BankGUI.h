/*
  ==============================================================================

    BankGUI.h
    Created: 7 Sep 2024 2:47:46pm
    Author:  Jake

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Bank.h"

//==============================================================================
/*
*/
class BankGUI  : public juce::Component, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    BankGUI(Bank& bank);
    ~BankGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override;
    void sliderValueChanged(juce::Slider *slider) override;
    
   
    

private:
    
    juce::Label volumeLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    
    
    juce::TextButton playButton{"Start"};
    juce::TextButton stopButton{"Stop"};
    
    juce::Slider volumeSlider;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider panningSlider;
    
    //pointer vectors
    std::vector<juce::Label*> labels = {&volumeLabel, &attackLabel, &decayLabel, &sustainLabel};
    std::vector<juce::Slider*> sliders = {&volumeSlider, &attackSlider, &decaySlider, &sustainSlider};
    
    
    Bank& bank;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BankGUI)
};
