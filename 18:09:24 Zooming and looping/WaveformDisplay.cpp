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
    for(int i = 0; i < playheadPositions.size(); i++)
    {
        playheadPositions[i] = nullptr;
    }
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    
    
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colours::orange);
    
    if(fileLoaded)
    {
        
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
        controllerStartY = area.getY() + area.getHeight();
        double controllerHeight = getHeight() / 4;
        g.drawRect(waveformStartDisplay, controllerStartY, getWidth(), controllerHeight);
        
        juce::Rectangle<int> controllerArea = getLocalBounds();
        controllerArea.setY(controllerStartY);
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
        g.drawRect(selectorStart, controllerStartY, endWidth, controllerHeight);
        g.drawRect(selectorEnd - endWidth, controllerStartY, endWidth, controllerHeight);
        
        auto bounds = getBounds();
        g.setFillType(juce::FillType(juce::Colours::turquoise));
        g.setOpacity(0.8f);
        bounds.setBounds(selectorStart + endWidth, controllerStartY, ((normalisedEnd - normalisedStart) * getWidth() - (endWidth * 2)), controllerHeight);
        g.fillRect(bounds);
        //g.drawRect(selectorStart + endWidth, controllerStartY, (normalisedEnd - normalisedStart) * getWidth() , controllerHeight);
        
//
        
        
        
        
        
        
        
        
        
        
        
        for(int i = 0; i < playheadPositions.size(); i++) //drawing playhead positions
        {
//            g.setColour(myColours[i]);
//            g.drawRect(*playheadPositions[i] * getWidth() - w/2, 0, w, getHeight());
//            
            
            
//            
//            
//            //Drawing ADSR curve
//            if(loopVec2d[i][0] > 0) //if start of the loop is greater than 0
//            {
//                
//                float loopStart = loopVec2d[i][0];
//                float loopEnd = loopVec2d[i][1];
//                
//                g.setColour(juce::Colours::white);
//                
//                float attack = adsrParameters[i]->attack;
//                float decay = adsrParameters[i]->decay;
//                float sustain = adsrParameters[i]->sustain;
//                
//                
//                
//                float xValPerSecond = getWidth() / audioThumbnail.getTotalLength();
////                
                
                
                
                
                //Drawing ADSR curve
//                if(loopVec2d[i][0] > 0) //if start of the loop is greater than 0
//                {
//                    
//                    //0,0,is the top and left
//                    float attackLineEndX = (loopStart * getWidth()) + (attack * xValPerSecond);
//                    
//                    float decayLineEndX = attackLineEndX + (decay * xValPerSecond);
//                    float decayLineEndY = getHeight() * (1 - sustain);
//                    
//                    if(attackLineEndX > loopEnd * getWidth())
//                    {
//                        g.drawLine(loopStart * getWidth() - w/2, getHeight(),loopEnd * getWidth(), 0);
//                    }else
//                    {
//                        
//                        g.drawLine(loopStart * getWidth() - w/2, getHeight(),attackLineEndX, 0);
//                        
//                        if(decayLineEndX > loopEnd * getWidth())
//                        {
//                            //do nothing
//                        }else
//                        {
//                            //decay line
//                            g.drawLine(attackLineEndX, 0, decayLineEndX, decayLineEndY);
//                            
//                            //sustain Line
//                            g.drawLine(decayLineEndX, decayLineEndY, loopEnd * getWidth(), decayLineEndY);
//                        }
//                    }
//                }
                
                
            //}
            
            
        }
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

void WaveformDisplay::setPlayheadPos(double pos, int bank)
{
    for(int i = 0; i < playheadPositions.size(); i++)
    {
       if(i == (bank - 1))
       {
           if(pos >= 0.f && pos != *playheadPositions[i])
           {
               *playheadPositions[i] = pos;
               repaint();
           }
       }
    }
}

void WaveformDisplay::setColours(std::vector<juce::Colour> colourVec)
{
    myColours = colourVec;
}

void WaveformDisplay::drawLoopRegions(std::vector<Interval<float>> loopRegions, juce::Graphics& g)
{
    float loopOpacity = 0.5f;
    
    loopVec2d.clear();
    
    loopVec2d.resize(loopRegions.size(), std::vector<float>(2,0)); //loopregions.size() amount of rows and two columns
    
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
    
            float controllerHeight = this->getHeight() - controllerStartY;
            
            g.fillRect(rectX, float(this->getY()), rectWidth, float(this->getHeight() - controllerHeight));
            
            
            repaint();
        }
        }
    }

void WaveformDisplay::sendParams(std::vector<juce::ADSR::Parameters*> parameters)
{
    adsrParameters.clear();
    
    for(int i = 0; i < parameters.size(); i++)
    {
        adsrParameters.push_back(parameters[i]);
        
        }
}

void WaveformDisplay::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
//
    repaint();
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent &event)
{
    
    int mouseX = event.getPosition().getX();
    //int mouseY = event.getPosition().getY();
    
    int mousePressedY = event.getMouseDownY();
    
    int safetyZone = 100; //pixels away from each other
    
    //MOVING ENDS OF SLIDERS
    
        if(mouseX < getWidth() && mouseX > 0)
        {
            if (mousePressedY > controllerStartY)
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



