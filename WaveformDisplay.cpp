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
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse) : audioThumbnail(1000, formatManagerToUse, cacheToUse)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumbnail.addChangeListener(this);

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
        
        //drawing the playheads
        audioThumbnail.drawChannel(g, getLocalBounds(), 0,
                                   audioThumbnail.getTotalLength(), 0, 1.0f);
        
        
        int w = 2; // width of playhead indicator in pixels
        
        for(int i = 0; i < playheadPositions.size(); i++) //drawing playhead positions
        {
            g.setColour(myColours[i]);
            g.drawRect(*playheadPositions[i] * getWidth() - w/2, 0, w, getHeight());
            
            
            
            //Drawing ADSR curve
            if(loopVec2d[i][0] > 0) //if start of the loop is greater than 0
            {
                
                float loopStart = loopVec2d[i][0];
                float loopEnd = loopVec2d[i][1];
                g.setColour(juce::Colours::white);
                
                float attack = adsrParameters[i]->attack;
                float decay = adsrParameters[i]->decay;
                float sustain = adsrParameters[i]->sustain;
                
                
                
                float xValPerSecond = getWidth() / audioThumbnail.getTotalLength();
                //Drawing ADSR curve
                if(loopVec2d[i][0] > 0) //if start of the loop is greater than 0
                {
                    
                    //0,0,is the top and left
                    float attackLineEndX = (loopStart * getWidth()) + (attack * xValPerSecond);
                    
                    float decayLineEndX = attackLineEndX + (decay * xValPerSecond);
                    float decayLineEndY = getHeight() * (1 - sustain);
                    
                    if(attackLineEndX > loopEnd * getWidth())
                    {
                        g.drawLine(loopStart * getWidth() - w/2, getHeight(),loopEnd * getWidth(), 0);
                    }else
                    {
                        
                        g.drawLine(loopStart * getWidth() - w/2, getHeight(),attackLineEndX, 0);
                        
                        if(decayLineEndX > loopEnd * getWidth())
                        {
                            //do nothing
                        }else
                        {
                            //decay line
                            g.drawLine(attackLineEndX, 0, decayLineEndX, decayLineEndY);
                            
                            //sustain Line
                            g.drawLine(decayLineEndX, decayLineEndY, loopEnd * getWidth(), decayLineEndY);
                        }
                    }
                }
                
                
            }
            
            
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
    fileLoaded = audioThumbnail.setSource(new juce::URLInputSource(url));
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
    float loopOpacity = 0.4;
    
    loopVec2d.clear();
    
    loopVec2d.resize(loopRegions.size(), std::vector<float>(2,0)); //loopregions.size() amount of rows and two columns
    
    
    for(int i = 0; i < loopRegions.size(); i++)
    {
        
        if(loopRegions[i].proper())
        {
            g.setColour(myColours[i]);
            g.setOpacity(loopOpacity);
            auto start = loopRegions[i].start();
            auto length = loopRegions[i].length();
            auto b = getBounds();
            b.setBounds(start * b.getWidth() + b.getX(), b.getY(), length * b.getWidth(), b.getHeight());
            g.fillRect(b);
            
            loopVec2d[i][0] = loopRegions[i].start();
            loopVec2d[i][1] = loopRegions[i].end();
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
