/*
  ==============================================================================

    Bank.cpp
    Created: 6 Sep 2024 6:17:24pm
    Author:  Jake

  ==============================================================================
*/

#include "Bank.h"

Bank::Bank(juce::AudioFormatManager& afm) : formatManager(afm)
{
        transportSource.setSource(nullptr);
        readerSource = nullptr;
        
        adsrParams.attack = 0.5f;  // Fast attack
        adsrParams.decay = 0.3f;    // Short decay
        adsrParams.sustain = 0.7f;  // Sustain level
        adsrParams.release = 0.2f;  // Release time
    
        adsr.setParameters(adsrParams);
}

Bank::~Bank()
{
    transportSource.setSource(nullptr);
    readerSource = nullptr;
}

void Bank::prepareToPlay(int samplesPerBlockExpected , double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //globalSampleRate = sampleRate;
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
        
        transportSource.getNextAudioBlock(bufferToFill);
        
        if(adsr.isActive())
        {
            //When something expects a reference(&) pass it the object not the pointer
            
            //In JUCE, the AudioSourceChannelInfo structure has a member buffer, which is a pointer to an AudioSampleBuffer (or juce::AudioBuffer<float> in more recent versions). This buffer points to the actual audio data stored in memory.

           // When you access bufferToFill.buffer, you are retrieving a pointer to an AudioBuffer<float>. However, applyEnvelopeToBuffer() requires a reference to the AudioBuffer<float>, not a pointer.
            
           
            //Pointer (bufferToFill.buffer): A pointer stores the memory address of an object.
           // Dereferenced Pointer (*bufferToFill.buffer): This gives you access to the object the pointer points to (the actual AudioBuffer<float> in this case).
            
            adsr.applyEnvelopeToBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
           
            //adsr.applyEnvelopeToBuffer(AudioBuffer<FloatType> &buffer, int startSample, int numSamples)
            
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
    
        DBG("start");
        adsr.reset();
        adsr.noteOn();
        
        transportSource.setPosition(loopRegion.start());
        transportSource.start();
        
        paused = false;
    
}
void Bank::stop()
{

    transportSource.stop();
    adsr.reset();
 
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
    loopRegion.start(start);
    loopRegion.end(end);
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
