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
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    
    bool loadURL(const juce::URL& url);
    
    void setFileDroppedCallback(std::function<void(const juce::URL&)> callback) //this is called in filesdropped and passed a url this is then assigned to dile droppedcallback which will then be passed to the editor
    {
        fileDroppedCallback = callback;
    }
    
    void setPlayheadPos(double pos, int bank);
    
    void setColours(std::vector<juce::Colour> colourVec);
    
    void drawLoopRegions(std::vector<Interval<float>> loopRegions, juce::Graphics& g);
    
    void sendParams(std::vector<juce::ADSR::Parameters*> parameters);
    
    void mouseDrag(const juce::MouseEvent& event) override;
    
    juce::AudioThumbnail* getThumbnail()
    {
        return &audioThumbnail;
    };
    
    
    double waveformStart = 0;
    double waveformEnd = 0;
    
    double controllerStartY;
    
    double startTime;
    double endTime;
    
    int selectorStart = 0;
    int selectorEnd = 900; //Need to change this to local bounds
    

private:
    
    //playhead positions
    double bank1Pos = 0.f;
    double bank2Pos = 0.f;
    double bank3Pos = 0.f;
    double bank4Pos = 0.f;
    double bank5Pos = 0.f;
    
    double myZoomFactor = 1.0f;
    double mousePositionX = 0.f;
    double centerTime = 0.0;
    
//    double startTime;
//    double endTime;
    
    int endWidth = 15;
    double controllerHeight;
    juce::Rectangle<int> area;
    
    //double controllerStartY;
    
//    juce::Rectangle<int> controllerArea = getLocalBounds();
    
    std::vector<double*>playheadPositions = {&bank1Pos, &bank2Pos, &bank3Pos, &bank4Pos, &bank5Pos};
    
    std::vector<juce::Colour> myColours;
    
    std::vector<juce::ADSR::Parameters*> adsrParameters;
    
    std::vector<std::vector<float>> loopVec2d;
    
    std::vector<bool>adsrDisplayToggle;
    
    std::function<void(const juce::URL&)> fileDroppedCallback;
    
    bool fileLoaded = false;
    
    juce::URL droppedFile;
    
    juce::AudioThumbnail audioThumbnail; //look up source samples per thumbnailsample to zoom
    juce::AudioThumbnail controllerThumbnail;
    
    bool runTest = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
