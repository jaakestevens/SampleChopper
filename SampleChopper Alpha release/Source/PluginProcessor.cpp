/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleChopperAudioProcessor::SampleChopperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
apvts(*this, nullptr, "PARAMETERS",{
    createParameterLayout()
})
#endif
{
    formatManager.registerBasicFormats(); //format manager for waveform
    
    juce::URL url;
    
    if(!apvts.state.hasProperty("currentFilePath"))
    {
        juce::String filePath = "file:///Users/User/Desktop/Reason/paradox.wav";
        apvts.state.setProperty("currentFilePath", filePath, nullptr);
        
        url = juce::URL{apvts.state.getProperty("currentFilePath").toString()};
    }
    else
    {
        url = {apvts.state.getProperty("currentFilePath")};
    }
    
    
    for(int i = 0; i < bankList.size(); i++)
    {
        
        bankList[i]->sampleTriggeredCallback = [this](int bank) //this sets the editor as a listener and when it receives the return from the setfiledroppedcallback function inside waveform display, it will call filedroppedonwaveform
        {
           monoProcessing(bank); //callback function
        };
        
        
        float loopStart;
        float loopEnd;
        
        juce::String loopStartID = ("loop" + juce::String(i + 1) + "Start");
        juce::String loopEndID = ("loop" + juce::String(i + 1) + "End");
        
        if(!apvts.state.hasProperty(loopStartID))
        {
            apvts.state.setProperty(loopStartID, 0, nullptr); //get rnadom value
            apvts.state.setProperty(loopEndID, 0, nullptr);
            
            loopStart = apvts.state.getProperty(loopStartID);
            loopEnd = apvts.state.getProperty(loopEndID);
        }
        else
        {
            loopStart = apvts.state.getProperty(loopStartID);
            loopEnd = apvts.state.getProperty(loopEndID);
        }
        
        if(i < bankList.size() - 1)
        {
            bankList[i]->setLoopRegion(loopStart, loopEnd); //dont loop listener
            
        }
        
    }
    
    
    loadURLS(url);
    
    apvts.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);
    
    presetManager = std::make_unique<PresetManager>(apvts);
    
    
    presetManager->urlLoaded = [this](const juce::String& url) //when load is called
    {
        DBG("Loading preset" + url);
        
        for(int i = 0; i < bankList.size(); i++)
        {
            bankList[i]->loadURL(url);
        }
    };
    
    
    presetManager->loopRegions = [this](std::vector<float> loopRegionsVec)
    {
        for(int i = 0; i < bankList.size(); i++)
        {
            bankList[i]->setLoopRegion(loopRegionsVec[i * 2], loopRegionsVec[(i *2) + 1]);
        }
        
    };
    
    monoState = apvts.getRawParameterValue("mono");
    
    // globalPitchParameter = apvts.getRawParameterValue("globalPitch");
    
}

SampleChopperAudioProcessor::~SampleChopperAudioProcessor()
{
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i] = nullptr;
    }
    
    undoManagerPointer = nullptr;
    
    
}

//==============================================================================
const juce::String SampleChopperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleChopperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleChopperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SampleChopperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SampleChopperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleChopperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleChopperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleChopperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SampleChopperAudioProcessor::getProgramName (int index)
{
    return {};
}

void SampleChopperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SampleChopperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    for(int i = 0; i < bankList.size(); i++)
    {
        mixerSource.addInputSource(bankList[i], false);
    }
    
    mixerSource.prepareToPlay(samplesPerBlock, sampleRate); //prepare to play
   
}

void SampleChopperAudioProcessor::releaseResources()
{
    mixerSource.removeAllInputs(); //releasing resources
    mixerSource.releaseResources();
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i]->releaseResources();
    }

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleChopperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SampleChopperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    int numberOfLoadedFiles = 0;
    
    for(int i = 0; i < bankList.size(); i++)
    {
       if(bankList[i]->isURLLoaded())
       {
           numberOfLoadedFiles++;
       }
    }
   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    
    if(numberOfLoadedFiles == bankList.size())
        {
        
            juce::AudioSourceChannelInfo channelInfo = (juce::AudioSourceChannelInfo(buffer)); //allows to treat function as getNextAudioBlock
            mixerSource.getNextAudioBlock(channelInfo);
            
        }
    
    for(int i = 0; i < bankList.size() - 1; i++)
    {
        
        bankList[i]->calculateFade();
    }
    
    
    splitBufferByEvents(buffer, midiMessages);

    
}

void SampleChopperAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    int bufferOffset = 0;
    
    for(const auto metadata: midiMessages)
    {
        //render the audio that happens before the MIDI event
        int sampleThisSegment = metadata.samplePosition - bufferOffset;
        if(sampleThisSegment > 0)
        {
            render(buffer, sampleThisSegment, bufferOffset);
            bufferOffset += sampleThisSegment;
        }
        
        //Handle the event. ignore midi messages such as sysex
        if(metadata.numBytes <= 3)
        {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }
    
    //render the audio after the last MIDI event. if there were no MIDI events at all, this renders the entire buffer
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if(samplesLastSegment > 0)
    {
        render(buffer, samplesLastSegment, bufferOffset);
    }
    
    midiMessages.clear();
}

void SampleChopperAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    
    uint8_t onOff = data0 & 0x7F;
    uint8_t note = data1 & 0x7F;
    
    std::vector<int>notes = {60, 62, 64, 65, 67, 69};
    
    
    for(int i = 0; i < bankList.size(); i++)
    {
        if(onOff > 0 && note == notes[i])
        {
            if(bankList[i]->getLoopRegion().proper())
            {
                bankList[i]->play();
            }
        }
    }
}

void SampleChopperAudioProcessor::render(juce::AudioBuffer<float> &buffer, int sampleCount, int bufferOffset)
{
    
}



//==============================================================================
bool SampleChopperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SampleChopperAudioProcessor::createEditor()
{
    return new SampleChopperAudioProcessorEditor (*this, apvts); //passing the editor
}

//==============================================================================
void SampleChopperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SampleChopperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if(tree.isValid())
    {
        
        apvts.replaceState(tree);
        auto filePath = apvts.state.getProperty("currentFilePath").toString();
        juce::URL loadedURL{filePath};
        loadURLS(loadedURL);
        
        
        for(int i = 0; i < bankList.size() - 1; i++)
        {
            
            juce::String loopStartID = "loop" + juce::String(i+1) + "Start";
            juce::String loopEndID = "loop" + juce::String(i+1) + "End";
            juce::String pitchID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Pitch";
            juce::String speedID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Speed";
            juce::String attackID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Attack";
            juce::String releaseID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Release";
            juce::String panningID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Panning";
            juce::String volumeID = "bank" + juce::String(bankList[i]->getBankNumber()) + "Volume";
            
            float loopStart = apvts.state.getProperty(loopStartID);
            float loopEnd = apvts.state.getProperty(loopEndID);
            float pitch = apvts.getRawParameterValue(pitchID)->load();
            float speed = apvts.getRawParameterValue(speedID)->load();
            float attack = apvts.getRawParameterValue(attackID)->load();
            float release = apvts.getRawParameterValue(releaseID)->load();
            float panning = apvts.getRawParameterValue(panningID)->load();
            float volume = apvts.getRawParameterValue(volumeID)->load();
            
            
            bankList[i]->setLoopRegion(loopStart, loopEnd);
            bankList[i]->initialiseParams(pitch, speed, attack, release, volume, panning);
        }
    }
    
}

void SampleChopperAudioProcessor::loadURLS(juce::URL &url)
{
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i]->loadURL(url);
    }
    
    
    fileFilled = true;
    
    
}

void SampleChopperAudioProcessor::setMonoState(bool mono)
{
    DBG("Audio Processor setting to mono");
    
}

void SampleChopperAudioProcessor::monoProcessing(int bank)
{
    if(monoState->load() == 1)
    {
        for(int i = 0; i < bankList.size() - 1; i++) //change this when removing listener bank
        {
            DBG("Mono bank playing: " + juce::String(bank));
            int bankPlayingIndex = bank - 1;
            
            if(i != bankPlayingIndex)
            {
                if(bankList[i]->isBankPlaying())
                {
                    Interval<float> bankLoop = bankList[i]->getLoopRegion();
                    bankList[i]->setLoopRegion(0, 0);
                    bankList[i]->setLoopRegion(bankLoop.start(), bankLoop.end());
                }
            }
        }
    }
}

juce::AudioFormatManager* SampleChopperAudioProcessor::getFormatManager()
{
    juce::AudioFormatManager* formatManagerPointer = &formatManager;
    
    return formatManagerPointer;
    
}

Bank* SampleChopperAudioProcessor::getBank(int bank)
{
    if(numberOfBanks == bankList.size())
    {
        Bank* bankPointer = bankList[bank - 1];
        
        return bankPointer;
    }
    
    DBG("Bank doesn't exist");
    return 0;
}

std::vector<Bank*> SampleChopperAudioProcessor::getBanksList()
{
    return bankList;
}

void SampleChopperAudioProcessor::clearLoops()
{
    for(int i = 0; i < bankList.size() - 1; i++)
    {
        bankList[i]->setLoopRegion(0.0f, 0.0f);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SampleChopperAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"globalPitch", 1}, //paramID
//                                                          "Global Pitch", //Name
//                                                          0.0f, //min
//                                                          10.0f, // max
//                                                           1.0f));
    
    //Volume
    juce::String volumeID = "masterVolume";
    auto masterVolumeRange = juce::NormalisableRange<float>(0.0001,1.f, 0.0001f);
    masterVolumeRange.setSkewForCentre(0.8f);
    
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{volumeID, 1},
                                                               "Master Volume",
                                                               masterVolumeRange, //increment
                                                               1.0f));
    
        //Pitch
        juce::String pitchID = "pitch";
        auto masterPitchRange = juce::NormalisableRange<float>(-0.5,1.f, 0.01f);
        masterPitchRange.setSkewForCentre(0.0f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{pitchID, 1},
                                                                   "Master Pitch",
                                                                   masterPitchRange,
                                                                   0.0f));
    
    //speed
    juce::String speedID = "speed";
    auto masterSpeedRange = juce::NormalisableRange<float>(-0.5,1.f, 0.01f);
    masterSpeedRange.setSkewForCentre(0.0f);
    
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{speedID, 1},
                                                               "Master Speed",
                                                               masterSpeedRange, //increment
                                                               0.0f));
    
    
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"mono", 1}, "Mono", false));

    
    
    
    
    //Banks
    for(int i = 0; i < 6; i++) //cannot use banksList as it has not been created yet
    {
        
        //Pitch
        juce::String pitchID = "bank" + juce::String(i+1) + "Pitch";
        auto pitchRange = juce::NormalisableRange<float>(-0.5,1.f, 0.01f);
        pitchRange.setSkewForCentre(0.0f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{pitchID, 1},
                                                                   "Bank " + juce::String(i+1) + " Pitch",
                                                                   pitchRange,
                                                                   0.0f));
                                                    
        //speed
        juce::String speedID = "bank" + juce::String(i+1) + "Speed";
        auto speedRange = juce::NormalisableRange<float>(-0.5,1.f, 0.01f);
        speedRange.setSkewForCentre(0.0f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{speedID, 1},
                                                                   "Bank " + juce::String(i+1) + " Speed",
                                                                   speedRange, //increment
                                                                   0.0f));
        
        //Attack
        juce::String attackID = "bank" + juce::String(i+1) + "Attack";
        auto attackRange = juce::NormalisableRange<float>(0.001,0.5f, 0.001f);
        attackRange.setSkewForCentre(0.2f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{attackID, 1},
                                                                   "Bank " + juce::String(i+1) + " Attack",
                                                                   attackRange, //increment
                                                                   0.001f));
        
        //Release
        juce::String releaseID = "bank" + juce::String(i+1) + "Release";
        auto releaseRange = juce::NormalisableRange<float>(1.0f,9.8f, 0.1f);
        releaseRange.setSkewForCentre(2.0f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{releaseID, 1},
                                                                   "Bank " + juce::String(i+1) + " Release",
                                                                   releaseRange, //increment
                                                                   1.0f));
        
        //Volume
        juce::String volumeID = "bank" + juce::String(i+1) + "Volume";
        auto volumeRange = juce::NormalisableRange<float>(0.1,10.f, 0.1f);
        volumeRange.setSkewForCentre(8.f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{volumeID, 1},
                                                                   "Bank " + juce::String(i+1) + " Volume",
                                                                   volumeRange, //increment
                                                                   8.0f));
        //Panning
        juce::String panningID = "bank" + juce::String(i+1) + "Panning";
        auto panningRange = juce::NormalisableRange<float>(-1.f,1.f, 0.01f);
        panningRange.setSkewForCentre(0.0f);
        
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{panningID, 1},
                                                                   "Bank " + juce::String(i+1) + " Speed",
                                                                   panningRange, //increment
                                                                   0.0f));
    }
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleChopperAudioProcessor();
}



