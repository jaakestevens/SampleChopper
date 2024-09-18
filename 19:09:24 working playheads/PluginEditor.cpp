/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleChopperAudioProcessorEditor::SampleChopperAudioProcessorEditor (SampleChopperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), waveformDisplay(*audioProcessor.getFormatManager(), audioProcessor.thumbCache)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (900, 600);
    
    //Creates the bank GUI's
    for(int i = 0; i < guiList.size(); i++)
    {
        addAndMakeVisible(guiList[i]);
    }
    
    //Loadbutton
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    
    //waveform display
    addAndMakeVisible(waveformDisplay);
    waveformDisplay.setFileDroppedCallback([this](const juce::URL& fileURL) //this sets the editor as a listener and when it receives the return from the setfiledroppedcallback function inside waveform display, it will call filedroppedonwaveform
    {
        fileDroppedOnWaveform(fileURL); //callback function
    });
    waveformDisplay.setColours(myColours); //gives the waveform object the colours used for bank selection
    
    
    
    //Selector Buttons
    for(int i = 0; i < selectorList.size(); i++)
    {
        addAndMakeVisible(selectorList[i]);
        selectorList[i]->addListener(this);
        selectorList[i]->setButtonText("Bank" + juce::String::charToString('A' + i));
        selectorList[i]->setColour(juce::TextButton::ColourIds::buttonColourId, myColours[i]);
    }
    
    std::vector<Bank*> banksList = audioProcessor.getBanksList();
    
    for(int i = 0; i < banksList.size(); i++) //adds adsr parameters from all banks to a list of parameters, it is a pointer to a juce::ADSR::Parameters
    {
        paramsList.push_back(banksList[i]->getAdsrParameters()); //pointer to memory addresses
    }
    
    
    waveformDisplay.sendParams(paramsList);
    
    //Timer used for playhead
    startTimer(20);
    
    //Adds a mouseListener to waveform display
    waveformDisplay.addMouseListener(this, false);
    
    
}

SampleChopperAudioProcessorEditor::~SampleChopperAudioProcessorEditor()
{
    
    //Assigning pointer vectors to nullptr - freeing memory
    for(int i = 0; i < guiList.size(); i++)
    {
        guiList[i] = nullptr;
    }
    
    for(int i = 0; i < selectorList.size(); i++)
    {
        selectorList[i] = nullptr;
    }
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i] = nullptr;
    }
    
    stopTimer();
    
    /////////////////////
}

//==============================================================================
void SampleChopperAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    
}

void SampleChopperAudioProcessorEditor::resized()
{
    float column = getWidth() / 5;
    float row = getHeight() / 5;
    
    for(int i = 0; i < guiList.size(); i++)
    {
        guiList[i]->setBounds(column * i, 0, column, row * 2);
    }
    
    for(int i = 0; i < selectorList.size(); i++)
    {
        selectorList[i]->setBounds(column * i, row * 2, column, row / 2);
    }
    
    
    loadButton.setBounds(0,(getHeight() / 5) * 3,getWidth()/3, getHeight() / 5);
    waveformDisplay.setBounds(0,(getHeight() / 5) * 4, getWidth(), getHeight() / 5);

}

void SampleChopperAudioProcessorEditor::timerCallback()
{
    std::vector<Bank*> bankList = audioProcessor.getBanksList(); //access members within the banks
    
    paramsList.clear(); //clears the current parameter list and refreshes it
    
    for(int i = 0; i < bankList.size(); i++)
    {
        paramsList.push_back(bankList[i]->getAdsrParameters());
    }
    
    waveformDisplay.sendParams(paramsList); //send the parameters to waveform display
    waveformDisplay.repaint(); //repaints component
}

void SampleChopperAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    
    if(&loadButton == button)
    {
        DBG("loadbutton");
        juce::FileChooser chooser("Select a sound file...");
        if(chooser.browseForFileToOpen())
        {
            auto url = juce::URL(chooser.getResult());
            audioProcessor.loadURLS(url);
            waveformDisplay.loadURL(url);
        }
    }
    
    for(int i = 0; i < selectorList.size(); i++)
    {
        if(selectorList[i] == button)
        {
            bankSelected = i + 1;
        }
    }
    
    DBG(bankSelected);
    
}

void SampleChopperAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    //to be used later master controls = volume filter etc.
}

void SampleChopperAudioProcessorEditor::fileDroppedOnWaveform(const juce::URL &fileURL) //this is what happens when the editor receives from wavefom
{
    //when dropped on waveform
    
    juce::File file = fileURL.getLocalFile();
    DBG(fileURL.getFileName());
    juce::URL url(file);
    waveformDisplay.loadURL(url);
    audioProcessor.loadURLS(url);
}

void SampleChopperAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    
    std::vector<Interval<float>> loopRegions = {audioProcessor.getBank(1)->loopRegion, audioProcessor.getBank(2)->loopRegion, audioProcessor.getBank(3)->loopRegion, audioProcessor.getBank(4)->loopRegion, audioProcessor.getBank(5)->loopRegion};
    
    waveformDisplay.drawLoopRegions(loopRegions, g);
    
    std::vector<float>bankPositions;
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankPositions.push_back(audioProcessor.getBank(i + 1)->getPositionRelative());
    }
    
    waveformDisplay.drawPlayheads(loopRegions, bankPositions, g);
    
}

void SampleChopperAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if(&waveformDisplay == event.eventComponent)
    {
        mouseDrag(event);
    }
    
}

void SampleChopperAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
    if(&waveformDisplay == event.eventComponent)
    {
        
       //Changing the length of the loop region in the processor(audio)
        if(event.getMouseDownY() < waveformDisplay.getScrollerStartY())
        {
            float invw = 1.f / waveformDisplay.getWidth(); //normalising width
            
            auto start = event.getMouseDownX() * invw;
            auto end = event.getPosition().getX() * invw;
            
            juce::AudioThumbnail * thumbnail = waveformDisplay.getThumbnail();
            double fileLength = thumbnail->getTotalLength();
            
            double normalisedStart = waveformDisplay.getWaveformStart() / fileLength; //proportion of the waveform
            double normalisedEnd = waveformDisplay.getWaveformEnd() / fileLength;
            
            float trueStart = normalisedStart + start * (normalisedEnd - normalisedStart);
            float trueEnd = normalisedStart + end * (normalisedEnd - normalisedStart);
            
            bankList[bankSelected - 1]->setLoopRegion(trueStart, trueEnd); //0-1 percentage of file
            
        }
        //    }
    }
    
}
