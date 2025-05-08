/*
  ==============================================================================

    Bank.cpp
    Created: 6 Sep 2024 6:17:24pm
    Author:  Jake

  ==============================================================================
*/

#include "Bank.h"

Bank::Bank(juce::AudioFormatManager& afm, juce::AudioProcessorValueTreeState &vts, int bankNumber) : formatManager(afm), valueTreeState(vts), bankNumber(bankNumber)
{
    
        transportSource.setSource(nullptr);
        readerSource = nullptr;
    
        transportPosPercent = 0.0f;
        
    //Envelope initialisation
        adsrParams.attack = 0.05f;  // Fast attack
        adsrParams.decay = 0.0f;    // Short decay
        adsrParams.sustain = 1.0f;  // Sustain level
        adsrParams.release = 0.2f;  // Release time
        adsr.setParameters(adsrParams);
    
        pitchShifter.clear();
        pitchShifter.setPitch(1.0f);
        pitchShifter.setTempo(1.0f);
    
    
    
    
}

Bank::~Bank()
{
   
        transportSource.setSource(nullptr);
        readerSource = nullptr;
}

void Bank::prepareToPlay(int samplesPerBlockExpected , double sampleRate)
{
  
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
        resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
        interleavedBuffer.resize(samplesPerBlockExpected * 2);
        
        pitchShifter.clear();
    
        pitchShifter.setSampleRate(int(sampleRate));
        pitchShifter.setChannels(2);
        pitchShiftLoaded = true;
        
}

void Bank::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    if(readerSource == nullptr)
    {
        return;
    }
    
    auto l = loopRegion;
    
    if(l.proper()) //if there is a loop region
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
        
        
        //Buffer from sample
        resampleSource.getNextAudioBlock(bufferToFill);
        
        //Pitch Shift
        bufferPitchShift(bufferToFill);
        
        //Panning
        applyPan(bufferToFill, panPosition); //panning
        
        //Short FadeOut
        applyShortFadeOut(bufferToFill); //short fade at the end of the sample
        
        
        //Fade in
        if(adsr.isActive())
        {
            adsr.applyEnvelopeToBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples); //applying envelope
           
        }
        
    }else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void Bank::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

bool Bank::loadURL(const juce::URL& url)
{
    
    auto * reader = formatManager.createReaderFor(url.getLocalFile());
    
    pitchShifter.clear();
    
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
        adsr.reset();
        adsr.noteOn();
        transportSource.start();
    }
    
    else
    {
        if(!isListenerBank && loopRegion.proper())
        {
            DBG("start loop");
            adsr.reset();
            transportSource.setPosition(loopRegion.start());
            
            adsr.noteOn();
            if(sampleTriggeredCallback) sampleTriggeredCallback(bankNumber); //1-6
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
    //mainly for listener bank
    transportSource.stop();
 
}


void Bank::setPosition(double posInSecs)
{
    if(posInSecs < 0. || posInSecs > transportSource.getLengthInSeconds())
    {
        DBG("Set position incorrect"); //within bounds of the sample
        return;
    }
    
    transportSource.setPosition(posInSecs);
    
    if(transportSource.isPlaying() == true)
    {
        transportSource.start();
    }
    
}

void Bank::setMasterGain(float masterGain)
{
    masterVolume = masterGain;
    transportSource.setGain(bankGain * masterGain);
}

void Bank::setGain(double gain)
{
    bankGain = gain;
    transportSource.setGain(bankGain * masterVolume);
}

void Bank::updatePitchAndSpeed(float pitch,float speed) //-0.5
{
    float resampleRatio = speed + 1.0f; //remaps to 0.5 to 2
    float pitchRatio = 1 + pitch;
    
    float compensatedPitch = pitchRatio / resampleRatio;
    
    resampleSource.setResamplingRatio(resampleRatio);
    bankPitch = compensatedPitch;
    
}

void Bank::initialiseParams(float pitch, float speed, float vtsAttack, float release, float volume, float panning)
{
    setGain(volume);
    adsrParams.attack = vtsAttack;
    setPanning(panning);
    float invertedValue = 9.8 - release + 1.0f;
    setFadeVal(invertedValue);
    updatePitchAndSpeed(pitch, speed);
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
    
    if(!isListenerBank) //if not listener bank
    {
        
        //Storing the loop region to value treeState
        juce::String loopStartID = ("loop" + juce::String(bankNumber) + "Start");
        juce::String loopEndID = ("loop" + juce::String(bankNumber)+ "End");
        
        valueTreeState.state.setProperty(loopStartID, start, nullptr);
        valueTreeState.state.setProperty(loopEndID, end, nullptr);
        
        
        loopRegion.start(valueTreeState.state.getProperty(loopStartID)); //between 0-1
        loopRegion.end(valueTreeState.state.getProperty(loopEndID));

        if(!loopRegion.proper())
        {
            //not stopping transport source due to the overhead
            DBG("loop stopped");
            setPosition(transportSource.getLengthInSeconds()); //send to the end of the sample?
        }
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

void Bank::setPanning(float panValue)
{
    panPosition = panValue; //value from slider
}

void Bank::applyPan(const juce::AudioSourceChannelInfo &bufferToFill, float panningVal)
{
    //runs on its on function as it takes a buffer and needs to constantly apply
    //is called in getNextAudioBlock
    
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

void Bank::setFadeVal(float val)
{
    fadeOutMultiple = val; //this is called in BankGUI
}

void Bank::applyShortFadeOut(const juce::AudioSourceChannelInfo &bufferToFill)
{
    //Called in getNextAudioBlock
    //This function actually applies the fade out to the buffer
        if(bufferToFill.buffer->getNumChannels() < 2) return;
        
        auto * leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto * rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        
        //calculated in calculate fade
        float gain = fadeOutIncrement;
    
    
        for(int sample= 0; sample < bufferToFill.numSamples; sample++)
        {
                leftChannel[sample] *= gain;
                rightChannel[sample] *= gain;
        }
}

void Bank::calculateFade()
{
    
    //this is where the fade is calculated
    
    if(!transportSource.isPlaying())return;
    
    
    else
    {
        float transportPercent = transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
        
        float start = getLoopRegion().start();
        float end = getLoopRegion().end();
        
        float fadeOutFlag = start + ((end - start) / 10) * fadeOutMultiple; //mutiple controlled by slider in bankGUI
        
        if(transportPercent >= fadeOutFlag) //if passes a percent in file(the flag)
        {
            float fadeFactor = 1.0f - ((transportPercent - fadeOutFlag) / (end - fadeOutFlag)); //how far into the fadeout region(1-...inverts this)
            
            fadeFactor = std::max(0.0f, std::min(1.0f, fadeFactor));//clamps between 0 and 1
            
            fadeOutIncrement = fadeFactor; //applies this to fadeFactor
        }else
        {
            fadeOutIncrement = 1.0f; //doesn't change the volume in buffer function
        }
    }
}

void Bank::bufferPitchShift(const juce::AudioSourceChannelInfo& bufferToFill)
{
        float pitchChange = bankPitch;
   
        pitchShifter.setPitch(pitchChange); //pitch has to be set every block as far as i can tell, maybe change this?
       
        
        float * inL = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample); // pointer to the left channel
        float* inR = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample); //pointer to right channel
        const int numSamples = bufferToFill.numSamples; //Number of samples in the buffer
        
        if((int)interleavedBuffer.size() < numSamples * 2)
        {
            interleavedBuffer.resize(numSamples * 2); //making sure the interleaved buffer is double the size. This is because we are combining the left and right channel to be put into one soundtouch object
        }
        
        for (int i = 0; i < numSamples; ++i)
        {
            interleavedBuffer[i * 2] = inL[i]; //iterates through the samples if you work out, the times 2 is so that the left goes 0, 2, 4,
                                                //while the right goes 1, 3, 5, etc;
            interleavedBuffer[i * 2 + 1] = inR[i];
        }
        
        const int blockSize = numSamples / 4;//block size is a quarter of the full buffer
        for(int i = 0; i < numSamples; i+= blockSize)
        {
            const int chunkSize = std::min(blockSize, numSamples - i); //find the min between buffer left and block size
            // Process the interleaved stereo buffer
            pitchShifter.putSamples(interleavedBuffer.data() + i * 2, chunkSize); //puts a chunk size of the interlaved buffer data into the pitchshifter
            pitchShifter.receiveSamples(interleavedBuffer.data() + i * 2, chunkSize);
            //remember 'i' is going up in chunksize(it is multiplied by 2 as the buffer is interleaved with both channels
        }
        
        // Deinterleave back into separate left and right channels
        for (int i = 0; i < numSamples; ++i)
        {
            inL[i] = interleavedBuffer[i * 2];
            inR[i] = interleavedBuffer[i * 2 + 1];
        }
}
