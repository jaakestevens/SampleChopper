/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveformDisplay.h"
#include "BankGUI.h"
#include "PresetGUI.h"

//==============================================================================
/**
*/
class SampleChopperAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener, public juce::Slider::Listener, public juce::Timer
{
public:
    SampleChopperAudioProcessorEditor (SampleChopperAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~SampleChopperAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override; //button virtual
    void sliderValueChanged(juce::Slider * slider) override; //slider virtual
    void timerCallback() override;
    
    void fileDroppedOnWaveform(const juce::URL& fileURL); //this is the function
    void setSelectorCallback(const int bankSelected);
    
    void paintOverChildren(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, juce::MouseWheelDetails &wheel);
    void mouseDoubleClick(const juce::MouseEvent& event)override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleChopperAudioProcessor& audioProcessor; //passing the processor
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    //Create bankGUIs and pass them the bank they are assigned to
    //As getBank returns a pointer to a bank, we must dereference to get what is insde the pointer
    BankGUI bankGUI1{*audioProcessor.getBank(1), valueTreeState};
    BankGUI bankGUI2{*audioProcessor.getBank(2), valueTreeState};
    BankGUI bankGUI3{*audioProcessor.getBank(3), valueTreeState};
    BankGUI bankGUI4{*audioProcessor.getBank(4), valueTreeState};
    BankGUI bankGUI5{*audioProcessor.getBank(5), valueTreeState};
    BankGUI bankGUI6{*audioProcessor.getBank(6), valueTreeState};
    
    //Waveform object
    
    WaveformDisplay waveformDisplay;
    
    PresetGUI presetGUI{audioProcessor.getPresetManager(), valueTreeState, waveformDisplay};
    
    
    //std::vector<juce::ADSR::Parameters*> paramsList;
    
    //Load button
    juce::TextButton loadButton{"Load Sample"};
    //SelectorButtons
    juce::TextButton selectorButton1, selectorButton2, selectorButton3, selectorButton4, selectorButton5, selectorButton6;
    
    juce::TextButton clearLoopsButton{"Clear loops"};

    juce::TextButton incrementSemiButton{"+ 1 Semitones"};
    juce::TextButton decrementSemiButton{"- 1 Semitones"};
    juce::TextButton showTransientsButton{"Toggle Transients"};
    juce::TextButton randomiseLoops{"Random Chop"};
    
    juce::TextButton globalControlsBorder;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoAttachment;
    juce::ToggleButton monoToggle{"Mono"};
    
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment; //maps pitch to apvts
//    juce::Slider globalPitchSlider;
    
    
    juce::Slider volumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    juce::Label volumeLabel;
    
    juce::Slider pitchSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment; //maps pitch to apvts
    juce::Label pitchLabel;
    
    juce::Slider speedSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    juce::Label speedLabel;
    
    //To be used later for testing when pre-cutting loops
    juce::Slider transientWindowSizeSlider;
    juce::Slider transientSensitivitySlider;
    
    juce::Slider bankScrollSlider;
    
    bool showTransientsFlag = true;
    
    //Pointer Vectors - need to be set to nullptr in destructor
    
    std::vector<Bank*>bankList = {audioProcessor.getBank(1), audioProcessor.getBank(2), audioProcessor.getBank(3), audioProcessor.getBank(4), audioProcessor.getBank(5), audioProcessor.getBank(6), audioProcessor.getBank(7)};
    
    std::vector<BankGUI*>guiList = {&bankGUI1, &bankGUI2, &bankGUI3, &bankGUI4, &bankGUI5, &bankGUI6};
    
    std::vector<juce::TextButton*> selectorButtons = {&selectorButton1, &selectorButton2, &selectorButton3, &selectorButton4, &selectorButton5, &selectorButton6};
    
    //current bankSelected 1-5 // 6 is listener bank
    int bankSelected = 1;
    
    //scrollbar
    float scrollBarincrement;
    
    
    //File to be displayed
    juce::File selectedAudioFile;
    
    std::vector<float> transients;
    
    //My colours for the UI
    std::vector<juce::Colour> myColours = {juce::Colours::navy, juce::Colours::darkred, juce::Colours::orange, juce::Colours::black, juce::Colours::purple, juce::Colours::green, juce::Colours::green, juce::Colours::blueviolet};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleChopperAudioProcessorEditor)
};
