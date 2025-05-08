/*
  ==============================================================================

    PresetPanel.cpp
    Created: 21 Apr 2025 10:43:42pm
    Author:  Jake

  ==============================================================================
*/

#include "PresetGUI.h"

PresetGUI::PresetGUI(PresetManager& presetManager, juce::AudioProcessorValueTreeState& vts, WaveformDisplay& waveform) : presetManager(presetManager), valueTreeState(vts), waveformDisplay(waveform)
{
    
    
    configureButton(saveButton, "Save");
    configureButton(deleteButton, "Delete");
    configureButton(previousPresetButton, "<");
    configureButton(nextPresetButton, ">");
    
    presetList.setTextWhenNothingSelected("No Preset Selected");
    presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(presetList);
    presetList.addListener(this);
    
    loadPresetList();
    
}

PresetGUI::~PresetGUI()
{
    saveButton.removeListener(this);
    deleteButton.removeListener(this);
    previousPresetButton.removeListener(this);
    nextPresetButton.removeListener(this);
    presetList.removeListener(this);
}

void PresetGUI::resized()
{
    
    float column = getWidth() / 10;
    
    
    saveButton.setBounds(0,0,column *  2, getHeight());
    previousPresetButton.setBounds(column * 2, 0, column, getHeight());
    presetList.setBounds(column * 3,0, column * 4, getHeight());
    nextPresetButton.setBounds(column * 7, 0, column, getHeight());
    deleteButton.setBounds(column * 8, 0, column * 2, getHeight());
}

void PresetGUI::buttonClicked(juce::Button * button)
{
    if(button == &saveButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Please enter name of preset to save",
            PresetManager::defaultDirectory,
            "*." + PresetManager::extension);
        
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
        {
            const auto result = chooser.getResult();
            presetManager.save(result.getFileNameWithoutExtension());
            loadPresetList();
        });
    }
    
    if(&nextPresetButton == button)
    {
        const auto index = presetManager.nextPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    
    if(&previousPresetButton == button)
    {
        const auto index = presetManager.previousPreset();
        presetList.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    
    if(button == &deleteButton)
    {
        bool userConfirmed = juce::AlertWindow::showOkCancelBox(juce::AlertWindow::WarningIcon, "Confirm Delete", "Are you sure you want to delete this preset?", "Yes","Cancel");
        
        if(userConfirmed)
        {
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
        }else
        {
            return;
        }
    }
    
    
    juce::String path = valueTreeState.state.getProperty("currentFilePath");
    juce::URL url{path};
    
    if(waveformDisplay.getURL() == url)
    {
        DBG("Same URL");
        return;
    }
    
    DBG("Changing waveform display URL");
    waveformDisplay.loadURL(url);
    
}
void PresetGUI::comboBoxChanged(juce::ComboBox * comboBox)
{
    if(comboBox == &presetList)
    {
        presetManager.load(presetList.getItemText(presetList.getSelectedItemIndex()));
    }
    
    juce::String path = valueTreeState.state.getProperty("currentFilePath");
    juce::URL url{path};
    
    waveformDisplay.loadURL(url);
}

void PresetGUI::configureButton(juce::Button& button, juce::String buttonText) //default is no words(can call without string
{
    
    button.setButtonText(buttonText);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(button);
    button.addListener(this);
}

void PresetGUI::loadPresetList()
{
    presetList.clear(juce::dontSendNotification); //will not trigger comboBoxchanged function
    
    const auto allPresets = presetManager.getAllPresets(); //juce::stringArray
    const auto currentPreset = presetManager.getCurrentPreset();
  
    presetList.addItemList(allPresets, 1);
    presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
    
    
    
}
