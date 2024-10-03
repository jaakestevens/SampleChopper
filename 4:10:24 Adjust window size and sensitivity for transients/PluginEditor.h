/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BankGUI.h"
#include "WaveformDisplay.h"
#include "Sequencer.h"

//==============================================================================
/**
*/
class SampleChopperAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener, public juce::Slider::Listener, public juce::Timer
{
public:
    SampleChopperAudioProcessorEditor (SampleChopperAudioProcessor&);
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
    
    void setPlayheadPos(double pos, int bank);
    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleChopperAudioProcessor& audioProcessor; //passing the processor
    
    //Create bankGUIs and pass them the bank they are assigned to
    //As getBank returns a pointer to a bank, we must dereference to get what is insde the pointer
    BankGUI bankGUI1{*audioProcessor.getBank(1)};
    BankGUI bankGUI2{*audioProcessor.getBank(2)};
    BankGUI bankGUI3{*audioProcessor.getBank(3)};
    BankGUI bankGUI4{*audioProcessor.getBank(4)};
    BankGUI bankGUI5{*audioProcessor.getBank(5)};
    
    std::vector<juce::ADSR::Parameters*> paramsList;
    
    //Load button
    juce::TextButton loadButton{"Load Sample"};
    //SelectorButtons
    juce::TextButton selectorButton1, selectorButton2, selectorButton3, selectorButton4, selectorButton5;
    
    juce::TextButton listenerBankPlay{"Play"};
    juce::TextButton listenerBankStop{"Stop"};
    juce::TextButton sliceButton{"Slice"};
    
    //juce::Slider globalPitchSlider;
    juce::TextButton incrementSemiButton{"+ 1 Semitones"};
    juce::TextButton decrementSemiButton{"- 1 Semitones"};
    
    juce::Slider globalPitchSlider;
    
    juce::TextButton showTransientsButton{"Show Transients"};
    bool showTransientsFlag = true;
    juce::Slider transientWindowSizeSlider;
    juce::Slider transientSensitivitySlider;
    
    //Pointer Vectors - need to be set to nullptr in destructor
    std::vector<Bank*>bankList = {audioProcessor.getBank(1), audioProcessor.getBank(2), audioProcessor.getBank(3), audioProcessor.getBank(4), audioProcessor.getBank(5), audioProcessor.getBank(6)};
    
    std::vector<BankGUI*>guiList = {&bankGUI1, &bankGUI2, &bankGUI3, &bankGUI4, &bankGUI5};
    
    std::vector<juce::TextButton*> selectorList = {&selectorButton1, &selectorButton2, &selectorButton3, &selectorButton4, &selectorButton5};
    
    //current bankSelected 1-5 // 6 is listener bank
    int bankSelected = 6;
    
    //scrollbar
    float scrollBarincrement;
    
    //Waveform object
    WaveformDisplay waveformDisplay;
    
    //Last file loaded into the waveform
    juce::File globalAudioFile;
    
    //My colours for the UI
    std::vector<juce::Colour> myColours = {juce::Colours::navy, juce::Colours::darkred, juce::Colours::orange, juce::Colours::black, juce::Colours::purple};
    
    Sequencer sequencer;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleChopperAudioProcessorEditor)
};
