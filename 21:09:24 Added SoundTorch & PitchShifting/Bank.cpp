/*
  ==============================================================================

    Bank.cpp
    Created: 6 Sep 2024 6:17:24pm
    Author:  Jake

  ==============================================================================
*/

#include "Bank.h"
//#include "SoundTouch/SoundTouch.h"

Bank::Bank(juce::AudioFormatManager& afm) : formatManager(afm)
{
        transportSource.setSource(nullptr);
        readerSource = nullptr;
        
        adsrParams.attack = 0.0f;  // Fast attack
        adsrParams.decay = 0.0f;    // Short decay
        adsrParams.sustain = 1.0f;  // Sustain level
        adsrParams.release = 0.2f;  // Release time
    
        adsr.setParameters(adsrParams);
    
    pitchShifterL.setPitch(1.3);
    pitchShifterR.setPitch(1.3);
}

Bank::~Bank()
{
    transportSource.setSource(nullptr);
    readerSource = nullptr;
}

void Bank::prepareToPlay(int samplesPerBlockExpected , double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    pitchShifterL.setSampleRate(int(sampleRate));
    pitchShifterL.setChannels(1);
    
    pitchShifterR.setSampleRate(int(sampleRate));
    pitchShifterR.setChannels(1);
}

void Bank::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    if(readerSource == nullptr)
    {
        return;
    }
    
    auto l = loopRegion;
    
    if(l.proper())
    {
        auto p = getPositionRelative();
        
        if(p >= l.end()) //if reaches the end of loop region
        {
            setPositionRelative(l.start());
            adsr.noteOff();
            paused = true;
        }
        
        if(p < l.start()) //before loop region, skips to the loop region
        {
            setPositionRelative(l.start());
        }
       
    }
    
    if(!paused)
    {
        if(!readerSource.get())
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }
        
        const int numSamples = bufferToFill.buffer->getNumSamples();
        
        transportSource.getNextAudioBlock(bufferToFill);
        applyPan(bufferToFill, panPosition);
        
        float* inL = bufferToFill.buffer->getWritePointer(0);
        float* inR = bufferToFill.buffer->getWritePointer(1);
        
        pitchShifterL.putSamples(inL, numSamples);
        pitchShifterL.receiveSamples(inL, numSamples);
        
        pitchShifterR.putSamples(inR, numSamples);
        pitchShifterR.receiveSamples(inR, numSamples);
        
    
        
        if(adsr.isActive())
        
        {
            adsr.applyEnvelopeToBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
           
            
        }
        
    }else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void Bank::releaseResources()
{
    transportSource.releaseResources();
}

bool Bank::loadURL(const juce::URL& url)
{
    auto * reader = formatManager.createReaderFor(url.getLocalFile()); 
    
    if(reader)
    {
        
        std::unique_ptr <juce::AudioFormatReaderSource > newSource(new juce::AudioFormatReaderSource(reader , true));
        
        transportSource.setSource(newSource.get(), 0, nullptr , reader ->sampleRate); 
        
        readerSource.reset(newSource.release());
   
        fileLoaded = true;
        
        return true;
    }
    return false;
}

void Bank::play()
{
    if(isListenerBank)
    {
        DBG("start listener");
        adsr.reset();
        adsr.noteOn();
        
        //transportSource.setPosition(loopRegion.start());
        transportSource.start();
    }else
    {
        if(!isListenerBank && loopRegion.proper())
        {
            DBG("start loop");
            adsr.reset();
            adsr.noteOn();
            
            transportSource.setPosition(loopRegion.start());
            transportSource.start();
        }else
        {
            transportSource.stop();
        }
    }
        
        paused = false;
    
}
void Bank::stop()
{

    transportSource.stop();
 
}


void Bank::setPosition(double posInSecs)
{
    if(posInSecs < 0. || posInSecs > transportSource.getLengthInSeconds())
    {
        DBG("Set position incorrect");
        return;
    }
    
    transportSource.setPosition(posInSecs);
    
    if(transportSource.isPlaying() == true)
    {
        transportSource.start();
    }
    
}
void Bank::setGain(double gain)
{
    float volume = gain / 5;
    transportSource.setGain(volume);
}

bool Bank::isURLLoaded()
{
    return fileLoaded;
}

float Bank::getPositionRelative()
{
    auto length = transportSource.getLengthInSeconds();
    
    if(length > 0.f)
    {
        return transportSource.getCurrentPosition() / length;
    }else
    {
        return 0.f;
    }
}

void Bank::setPositionRelative(const double pos)
{
    auto posInSecs = pos * transportSource.getLengthInSeconds();
    
    setPosition(posInSecs);
}

void Bank::setLoopRegion(float start, float end)
{
        loopRegion.start(start); //between 0-1
        loopRegion.end(end);
        
    if(!loopRegion.proper())
        {
            //not stopping transport source due to the overhead
            DBG("loop stopped");
            setPosition(transportSource.getLengthInSeconds());
        }
    
}

void Bank::setAdsrParameters(juce::ADSR::Parameters myParams)
{
    
    adsr.setParameters(myParams);
}

juce::ADSR::Parameters* Bank::getAdsrParameters() //returning address
{
    return &adsrParams;
}

void Bank::setAdsrDisplay(bool state)
{
    adsrDisplayToggle = state;
}

bool Bank::getAdsrDisplay()
{
    return adsrDisplayToggle;
}

void Bank::setPanning(float panValue)
{
    panPosition = panValue;
    DBG("Pan position = " << panPosition);
}

void Bank::applyPan(const juce::AudioSourceChannelInfo &bufferToFill, float panningVal)
{
    if(bufferToFill.buffer->getNumChannels() < 2) return;
    
    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    
    const float leftGain = panningVal <= 0.0f ? 1.0f : 1.0f - panningVal; // more pan to the right, lower the left gain
    const float rightGain = panningVal >= 0.0f ? 1.0f : 1.0f + panningVal;
    
    for(int sample = 0; sample < bufferToFill.numSamples; sample++)
    {
        leftChannel[sample] *= leftGain;
        rightChannel[sample] *= rightGain;
    }
}
