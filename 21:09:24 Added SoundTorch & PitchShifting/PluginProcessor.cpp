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
                       )
#endif
{
    formatManager.registerBasicFormats(); //format manager for waveform
    //masterPitchShifter.clear();
}

SampleChopperAudioProcessor::~SampleChopperAudioProcessor()
{
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i] = nullptr;
    }
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

    const int numSamples = buffer.getNumSamples();
   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    juce::AudioBuffer<float> tempBuffer(totalNumInputChannels, numSamples);

    if(numberOfLoadedFiles == bankList.size())
        {
                        juce::AudioSourceChannelInfo channelInfo = (juce::AudioSourceChannelInfo(buffer)); //allows to treat function as getNextAudioBlock
            mixerSource.getNextAudioBlock(channelInfo);
        }
    
}



//==============================================================================
bool SampleChopperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SampleChopperAudioProcessor::createEditor()
{
    return new SampleChopperAudioProcessorEditor (*this);
}

//==============================================================================
void SampleChopperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleChopperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SampleChopperAudioProcessor::loadURLS(juce::URL &url)
{
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i]->loadURL(url);
    }
    
    fileFilled = true;
    
    
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleChopperAudioProcessor();
}



