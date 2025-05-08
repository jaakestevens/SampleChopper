/*
  ==============================================================================

    PresetManager.h
    Created: 22 Apr 2025 12:04:00am
    Author:  Jake

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener
{
public:
    
    static const juce::File defaultDirectory;
    static const juce::String extension; //at the end of preset file (.preset) or .sc for sample chopper
    static const juce::String presetNameProperty;
    
    PresetManager(juce::AudioProcessorValueTreeState&);
    ~PresetManager();
    
    void save(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void load(const juce::String& presetName);
    int nextPreset();
    int previousPreset();
    
    juce::StringArray getAllPresets()const;
    juce::String getCurrentPreset() const{return currentPreset.toString();};
    
    std::function<void(const juce::String&)> urlLoaded; //callback function
    
    std::function<void(const std::vector<float>)>loopRegions; //callback function
    
    
private:
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)override;
    
    int numberOfBanks = 6;
    
    std::vector<float> loopRegionsVec;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Value currentPreset;
    
};


