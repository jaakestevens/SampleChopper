/*
  ==============================================================================

    WaveformDisplay.h
    Created: 7 Sep 2024 6:55:38pm
    Author:  Jake

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Interval.h"

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component, public juce::FileDragAndDropTarget,  public juce::ChangeListener
{
public:
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void changeListenerCallback(juce::ChangeBroadcaster * source) override;
    
    
    //My functions
    bool loadURL(const juce::URL& url);
    
    void setFileDroppedCallback(std::function<void(const juce::URL&)> callback) //this is called in filesdropped and passed a url this is then assigned to dile droppedcallback which will then be passed to the editor
    {
        fileDroppedCallback = callback;
    }
    
    void setColours(std::vector<juce::Colour> colourVec);
    
    void drawLoopRegions(std::vector<Interval<float>> loopRegions, juce::Graphics& g);
    
    void drawPlayheads(std::vector<Interval<float>> loopRegions, std::vector<float>bankPositions, juce::Graphics& g);
    
    void sendParams(std::vector<juce::ADSR::Parameters*> parameters);
    
    void mouseDrag(const juce::MouseEvent& event) override;
    
    juce::AudioThumbnail* getThumbnail()
    {
        return &audioThumbnail;
    };
    
    double getWaveformStart()
    {
        return waveformStart;
    }
    
    double getWaveformEnd()
    {
        return waveformEnd;
    }
    
    double getScrollerStartY()
    {
        return scrollerStartY;
    }
    

private:
    
    //Rectangles used to resize the waveform
    int selectorStart = 0; //the initial starting point of selectors
    int selectorEnd = 900; //Need to change this to local bounds
    int endWidth = 15; //width of the selectors
    double scrollerStartY; //where this component starts on the Y axis
    
    
    double waveformStart = 0; //at which point the waveform begins on its thumbnail display in relation to full file (0-1)
    double waveformEnd = 0;
    
    
    std::vector<juce::Colour> myColours; //UI Colours
    
    std::vector<juce::ADSR::Parameters*> adsrParameters; //ADSR parameters for each bank
    
    std::function<void(const juce::URL&)> fileDroppedCallback; //callback function for editor
    
    bool fileLoaded = false; //if a file is loaded in
    
    juce::URL droppedFile; //file dropped 
    
    juce::AudioThumbnail audioThumbnail; //look up source samples per thumbnailsample to zoom
    juce::AudioThumbnail controllerThumbnail; // thumbnail for controller
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
