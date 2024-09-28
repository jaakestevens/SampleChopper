/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 7 Sep 2024 6:55:38pm
    Author:  Jake

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse) : audioThumbnail(1000, formatManagerToUse, cacheToUse) , controllerThumbnail(1000,formatManagerToUse, cacheToUse)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumbnail.addChangeListener(this);
    controllerThumbnail.addChangeListener(this);
    
    waveformEnd = audioThumbnail.getTotalLength();
    DBG(waveformEnd);

}

WaveformDisplay::~WaveformDisplay()
{
    
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    
    
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colours::orange);
    
    
    if(fileLoaded)
    {
        
        juce::Rectangle<int> area;
        
        //Main thumbnail
        area = getLocalBounds();
        area.setHeight(area.getHeight() / 1.5);
         
        int channelNum = 0;
        double verticalZoomFactor = 2.0f;
        
        double waveformStartDisplay = 0.f;
        double waveformEndDisplay = getWidth();
        
        g.drawRect(waveformStartDisplay,0,waveformEndDisplay, getHeight() / 1.5);
        
        double normalisedStart = static_cast<double>(selectorStart) / getWidth(); //between 0-1
        double normalisedEnd = static_cast<double>(selectorEnd) / getWidth();
        
        waveformStart = audioThumbnail.getTotalLength() * normalisedStart; //between 0-1
        waveformEnd = audioThumbnail.getTotalLength() * normalisedEnd;
        
        audioThumbnail.drawChannel(g, area, waveformStart, waveformEnd, channelNum, verticalZoomFactor);
        
        
        
        //controller border and thumbnail
        scrollerStartY = area.getY() + area.getHeight();
        double controllerHeight = getHeight() / 4;
        g.drawRect(waveformStartDisplay, scrollerStartY, getWidth(), controllerHeight);
        
        juce::Rectangle<int> controllerArea = getLocalBounds();
        controllerArea.setY(scrollerStartY);
        controllerArea.setHeight(controllerHeight);
        
        g.setColour(juce::Colours::white);
        
        controllerThumbnail.drawChannel(g, controllerArea, 0, controllerThumbnail.getTotalLength(), channelNum, verticalZoomFactor);
        
        
        
        //ends
        
        if(selectorStart < 0.01 && selectorStart > 0)
        {
            selectorStart = 0;
        }
        
        if(selectorEnd >= 0.99 && selectorEnd < 1)
        {
            selectorEnd = 1;
        }
        
        g.setColour(juce::Colours::black);
        g.setFillType(juce::FillType(juce::Colours::black));
        g.drawRect(selectorStart, scrollerStartY, endWidth, controllerHeight);
        g.drawRect(selectorEnd - endWidth, scrollerStartY, endWidth, controllerHeight);
        
        auto bounds = getBounds();
        g.setFillType(juce::FillType(juce::Colours::turquoise));
        g.setOpacity(0.8f);
        bounds.setBounds(selectorStart + endWidth, scrollerStartY, ((normalisedEnd - normalisedStart) * getWidth() - (endWidth * 2)), controllerHeight);
        g.fillRect(bounds);
        
    }
    
    else
    { // draw some placeholder text g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(), juce::Justification::centred, true);
    }
}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    DBG("WFD: change received");
    repaint();
}

bool WaveformDisplay::loadURL(const juce::URL &url)
{
    audioThumbnail.clear();
    controllerThumbnail.clear();
    
    fileLoaded = audioThumbnail.setSource(new juce::URLInputSource(url));
    controllerThumbnail.setSource(new juce::URLInputSource(url));
    return fileLoaded;
}

bool WaveformDisplay::isInterestedInFileDrag(const juce::StringArray& files)
{
    return true;
}

void WaveformDisplay::filesDropped(const juce::StringArray& files, int x, int y)
{
    if(fileDroppedCallback && files.size() > 0)
    {
        juce::URL fileURL {juce::File(files[0])}; //convert the file path to URL
        fileDroppedCallback(fileURL);
    }
}

void WaveformDisplay::setColours(std::vector<juce::Colour> colourVec)
{
    myColours = colourVec;
}

void WaveformDisplay::drawLoopRegions(std::vector<Interval<float>> loopRegions, juce::Graphics& g)
{
    float loopOpacity = 0.5f;
    
    float displayStart = waveformStart / audioThumbnail.getTotalLength();
    float displayEnd = waveformEnd / audioThumbnail.getTotalLength();
    
    
    for(int i = 0; i < loopRegions.size(); i++)
    {
        
        if(loopRegions[i].proper())
        {
            g.setColour(myColours[i]);
            g.setOpacity(loopOpacity);
            
            auto start = loopRegions[i].start();
            auto end = loopRegions[i].end();
        
            if(end < displayStart || start > displayEnd)
            {
                continue; //if part of it is in range
            }
            
            float visibleRange = displayEnd - displayStart;
            
            float loopStartRelative = (start - displayStart) / visibleRange;
            float loopEndRelative = (end - displayStart) / visibleRange;
            
            loopStartRelative = juce::jlimit(0.0f,1.0f, loopStartRelative);
            loopEndRelative = juce::jlimit(0.0f, 1.0f, loopEndRelative);
            
            float rectX = loopStartRelative * this->getWidth();
            float rectWidth = (loopEndRelative - loopStartRelative) * this->getWidth();
    
            float controllerHeight = this->getHeight() - scrollerStartY;
            
            g.fillRect(rectX, float(this->getY()), rectWidth, float(this->getHeight() - controllerHeight));
            
        }
        }
    repaint();
    }

void WaveformDisplay::sendParams(std::vector<juce::ADSR::Parameters*> parameters)
{
    adsrParameters.clear();
    
    for(int i = 0; i < parameters.size(); i++)
    {
        adsrParameters.push_back(parameters[i]);
        
        }
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent &event)
{
    
    int mouseX = event.getPosition().getX();
    //int mouseY = event.getPosition().getY();
    
    int mousePressedY = event.getMouseDownY();
    
    //MOVING ENDS OF SLIDERS
    
        if(mouseX < getWidth() && mouseX > 0)
        {
            if (mousePressedY > scrollerStartY)
            {
                
                if(mouseX < selectorStart + (endWidth * 2) && mouseX > selectorStart - (endWidth * 2))
                {
                    if(mouseX < selectorEnd - safetyZone)
                    {
                        selectorStart = mouseX;
                    }
                }
                
                if(mouseX < selectorEnd + (endWidth * 2) && mouseX > selectorEnd - (endWidth * 2))
                {
                    if(mouseX > selectorStart + safetyZone)
                    {
                        selectorEnd = mouseX;
                    }
                }
                
                repaint();
                
            }
            
        }
    
    
    
    }

void WaveformDisplay::drawPlayheads(std::vector<Interval<float>> loopRegions, std::vector<float> bankPositions, float listenerBankPosition, juce::Graphics& g)
{
    
    float displayStart = waveformStart / audioThumbnail.getTotalLength();
    float displayEnd = waveformEnd / audioThumbnail.getTotalLength();
    float visibleRange = displayEnd - displayStart;
    
    for(int i = 0; i < bankPositions.size(); i++)
    {
     
        if(loopRegions[i].proper())
        {
            float playheadPosition = bankPositions[i];
            
            if(playheadPosition < displayStart || playheadPosition > displayEnd)
            {
                continue;
            }
            
            float playheadRelative = (playheadPosition - displayStart) / visibleRange;
            
            playheadRelative = juce::jlimit(0.0f, 1.0f, playheadRelative);
            
            float playheadX = playheadRelative * this->getWidth();
            g.setColour(myColours[i]);
            
            if(playheadPosition > 0)
            {
                int playheadWidth = 2;
                g.drawRect(int(playheadX),int(this->getY()), playheadWidth,int(this->getHeight() - scrollerStartY) * 2);
                
                
            }
        }
        
    }
    
    if(currentBankSelected == 6)
    {
        float playheadRelative = (listenerBankPosition - displayStart) / visibleRange;
        
        playheadRelative = juce::jlimit(0.0f, 1.0f, playheadRelative);
        
        float playheadX = playheadRelative * this->getWidth();
        
        g.setColour(juce::Colours::black);
        g.drawRect(int(playheadX),int(this->getY()), 3,int(this->getHeight() - scrollerStartY) * 2);
        
        masterPlayheadPos = playheadX;
        
        
        juce::Path trianglePathLocal;
        //Drawing the playhead
        int playheadWidth = 16;
        int playheadHeight = 16;
        
        //juce::Path trianglePath;
        float playheadBaseY = this->getY();
        float playheadTipY = playheadBaseY + playheadHeight;
        
        trianglePathLocal.startNewSubPath(playheadX, playheadTipY);
        trianglePathLocal.lineTo(playheadX - playheadWidth / 2, playheadBaseY);
        trianglePathLocal.lineTo(playheadX + playheadWidth / 2, playheadBaseY);
        trianglePathLocal.closeSubPath();
        
        g.fillPath(trianglePathLocal);
    }

}





