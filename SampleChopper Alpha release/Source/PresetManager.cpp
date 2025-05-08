/*
  ==============================================================================

    PresetManager.cpp
    Created: 22 Apr 2025 12:04:00am
    Author:  Jake

  ==============================================================================
*/

#include "PresetManager.h"

const juce::File PresetManager::defaultDirectory{juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory).getChildFile(ProjectInfo::companyName).getChildFile(ProjectInfo::projectName)};

const juce::String PresetManager::extension{"preset"};

const juce::String PresetManager::presetNameProperty{"presetName"};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& valueTreeState) : valueTreeState(valueTreeState)
{
    if(!defaultDirectory.exists())
    {
        const auto result = defaultDirectory.createDirectory();
        if(result.failed())
        {
            DBG("Could not create preset directory");
            jassertfalse;
        }
    }
    valueTreeState.state.addListener(this);
    currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
    
    for(int i = 0; i < numberOfBanks; i++)
    {
        
        float start = valueTreeState.state.getProperty("loop" + juce::String(i+1) + "Start");
        float end = valueTreeState.state.getProperty("loop" + juce::String(i+1) + "End");
        
        loopRegionsVec.push_back(start);
        loopRegionsVec.push_back(end);
        
        DBG(start);
        DBG(end);
    }
}

PresetManager::~PresetManager()
{
    
}

void PresetManager::save(const juce::String& presetName)
{
    if(presetName.isEmpty()) return;
    
    const auto xml = valueTreeState.copyState().createXml(); //returns unique pointer of xml
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
   
    if(!xml->writeTo(presetFile))
    {
        DBG("Could not create preset");
        jassertfalse;
            
    }
    
    currentPreset.setValue(presetName);
  
}

void PresetManager::deletePreset(const juce::String& presetName)
{
    if(presetName.isEmpty()) return;
    
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
   
    if(!presetFile.existsAsFile())
    {
        DBG("Preset doesn't exist, cannot delete");
        jassertfalse;
        return;
    }
    if(!presetFile.deleteFile())
    {
        DBG("Problem deleting file");
        jassertfalse;
        return;
    }
    
    currentPreset.setValue("");
}

void PresetManager::load(const juce::String& presetName)
{
    if(presetName.isEmpty()) return;
    
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    
    if(!presetFile.existsAsFile())
       {
           DBG("Preset file" + presetFile.getFullPathName() +  "doesn't exist");
           jassertfalse;
           return;
       }
    
    juce::XmlDocument xmlDoc{presetFile}; //
    
    const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDoc.getDocumentElement());
    
    valueTreeState.replaceState(valueTreeToLoad);
    
    
    
    currentPreset.setValue(presetName);
    
    if(urlLoaded)
    {
        juce::String filePath = valueTreeState.state.getProperty("currentFilePath");
        urlLoaded(filePath);
    }
    
   
    loopRegionsVec.clear();
   
    for(int i = 0; i < numberOfBanks; i++)
    {
        float start = valueTreeState.state.getProperty("loop" + juce::String(i+1) + "Start");
        float end = valueTreeState.state.getProperty("loop" + juce::String(i+1) + "End");
        
        
        loopRegionsVec.push_back(start);
        loopRegionsVec.push_back(end);
        
        DBG(start);
        DBG(end);
    }
    
    loopRegions(loopRegionsVec);
    
}

int PresetManager::nextPreset()
{
    const auto allPresets = getAllPresets();
    if(allPresets.isEmpty())return - 1;
    
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1; //if greater than length of presets loop back to one if not increment
    
    load(allPresets.getReference(nextIndex));
    
    return nextIndex;
}

int PresetManager::previousPreset()
{
    const auto allPresets = getAllPresets();
    
    if(allPresets.isEmpty())return - 1;
    
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
    
    load(allPresets.getReference(previousIndex));
    
    return previousIndex;
}

juce::StringArray PresetManager::getAllPresets()const
{
    juce::StringArray presets;
    const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
    
    for(const auto& file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }
    
    return presets;
}

void PresetManager::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{
    currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
}

