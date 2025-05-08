/*
  ==============================================================================

    PresetPanel.h
    Created: 21 Apr 2025 10:43:42pm
    Author:  Jake

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetManager.h"
#include "WaveformDisplay.h"

class PresetGUI : public juce::Component, public juce::Button::Listener, public::juce::ComboBox::Listener
{
public:
    PresetGUI(PresetManager&, juce::AudioProcessorValueTreeState&, WaveformDisplay&);
    ~PresetGUI();
    
private:
    
    
    
    void resized() override;
    
    void buttonClicked(juce::Button * button) override;
    void configureButton(juce::Button &button, juce::String buttonText = " ");
    void loadPresetList();
    void comboBoxChanged(juce::ComboBox * comboBox)override;
    
    juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    juce::ComboBox presetList;
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    PresetManager& presetManager;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    WaveformDisplay& waveformDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetGUI);
};
