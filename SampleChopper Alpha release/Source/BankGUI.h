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
#include "MyLook&Feel.h"
//==============================================================================
/*
*/
class BankGUI  : public juce::Component, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    BankGUI(Bank& bank, juce::AudioProcessorValueTreeState&);
    ~BankGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override;
    void sliderValueChanged(juce::Slider *slider) override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::Label volumeLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label fadeLabel;
    juce::Label pitchLabel;
    juce::Label speedLabel;
    
    
    juce::TextButton playButton{"Start"};
    juce::TextButton resetButton{"reset"};
    juce::TextButton incrementChop{">"};
    juce::TextButton decrementChop{"<"};
    
    juce::Slider volumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment; //below slider
    
    juce::Slider attackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment; //below slider
    
    juce::Slider panningSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panningAttachment; //below slider
    
    juce::Slider fadeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment; //below slider
   
    juce::Slider pitchSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment; //below slider
    
    juce::Slider speedSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    
    //pointer vectors
    std::vector<juce::Label*> volumeLabels = {&volumeLabel, &attackLabel, &fadeLabel};
    std::vector<juce::Slider*> volumeSliders = {&volumeSlider, &attackSlider, &fadeSlider};
    
    std::vector<juce::Colour> myColours = {juce::Colours::navy, juce::Colours::darkred, juce::Colours::orange, juce::Colours::black, juce::Colours::purple, juce::Colours::green, juce::Colours::green, juce::Colours::blueviolet};
    
    
    Bank& bank;
    
    int bankNumber;
   // float previousSpeed; //change this when linking to valuetreestate
    bool pitchSpeedReset = false;
    
    juce::Image testImage;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BankGUI)
};
