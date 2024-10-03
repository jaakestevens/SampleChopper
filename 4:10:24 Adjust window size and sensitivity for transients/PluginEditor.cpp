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
    
    //Preview the sample
    addAndMakeVisible(listenerBankPlay);
    listenerBankPlay.addListener(this);
    addAndMakeVisible(listenerBankStop);
    listenerBankStop.addListener(this);
    
    //pitch slider that effects the entire track
    addAndMakeVisible(globalPitchSlider);
    globalPitchSlider.addListener(this);
    
    addAndMakeVisible(showTransientsButton);
    showTransientsButton.addListener(this);
    
    addAndMakeVisible(transientSensitivitySlider);
    transientSensitivitySlider.addListener(this);
    transientSensitivitySlider.setRange(0.5, 20.f);
    transientSensitivitySlider.setValue(10.f);
    transientSensitivitySlider.setNumDecimalPlacesToDisplay(1);
    
    addAndMakeVisible(transientWindowSizeSlider);
    transientWindowSizeSlider.addListener(this);
    transientWindowSizeSlider.setRange(10,100);
    transientWindowSizeSlider.setValue(20);
    transientWindowSizeSlider.setNumDecimalPlacesToDisplay(0);

    
    //waveform display
    addAndMakeVisible(waveformDisplay);
    
    waveformDisplay.setFileDroppedCallback([this](const juce::URL& fileURL) //this sets the editor as a listener and when it receives the return from the setfiledroppedcallback function inside waveform display, it will call filedroppedonwaveform
    {
        fileDroppedOnWaveform(fileURL); //callback function
    });
    waveformDisplay.setColours(myColours); //gives the waveform object the colours used for bank selection
    
    
    
    //Bank selector Buttons
    for(int i = 0; i < selectorList.size(); i++)
    {
        addAndMakeVisible(selectorList[i]);
        selectorList[i]->addListener(this);
        selectorList[i]->setButtonText("Bank " + juce::String::charToString('A' + i));
        selectorList[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    }
    
    std::vector<Bank*> banksList = audioProcessor.getBanksList();
    
    for(int i = 0; i < guiList.size(); i++) //adds adsr parameters from all banks to a list of parameters, it is a pointer to a juce::ADSR::Parameters
    {
        paramsList.push_back(banksList[i]->getAdsrParameters()); //pointer to memory addresses
    }
    
    //sends the adsr, unneeded for now
    waveformDisplay.sendParams(paramsList);
    
    //Timer used for playhead
    startTimer(20);
    
    //Adds a mouseListener to waveform display for the editor (for file callback)
    waveformDisplay.addMouseListener(this, false);
    
    Bank * listenerBank = audioProcessor.getListenerBank();
    listenerBank->makeBankListener(true);
    
    waveformDisplay.setBankSelected(bankSelected);
    
    addAndMakeVisible(sequencer);
    
    sequencer.triggerSample = ([this, banksList](int bank) //when the sequencer calls triggers sample it will play that bank from the editor
    {
        if(banksList[bank]->loopRegion.proper())
        {
            banksList[bank]->play();
        }
    });
    
    
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
    float column = getWidth() / 12;
    float row = getHeight() / 5;
    
    for(int i = 0; i < guiList.size(); i++)
    {
        guiList[i]->setBounds(column * i, (getHeight() / 10) * 3.5, column, row * 1.7);
    }
    
    for(int i = 0; i < selectorList.size(); i++)
    {
        selectorList[i]->setBounds(column * i, (getHeight() / 10) * 6.9, column, getHeight() / 30);
    }
    
    
    listenerBankPlay.setBounds(0, 0, getWidth() / 10, getHeight() / 20);
    listenerBankStop.setBounds(getWidth() / 10, 0, getWidth() / 10, getHeight() / 20);
    loadButton.setBounds((getWidth() / 14) * 7.5,0,(getWidth()/14) * 2, getHeight() / 20);
    globalPitchSlider.setBounds((getWidth() / 14) * 5, 0, (getWidth() / 14) * 2, getHeight() / 20);
    
    waveformDisplay.setBounds((getWidth() / 20) * 2,(getHeight() / 20) * 1.25, (getWidth() / 20) * 16, (getHeight() / 10) * 2.5);
    
    showTransientsButton.setBounds((getWidth() / 14) * 11, 0, (getWidth() / 14) * 2, getHeight() / 20);
    transientSensitivitySlider.setBounds((getWidth() / 20) * 18, (getHeight() / 20) * 1.75, (getWidth() / 20) * 2, (getHeight() / 20) * 2);
    transientWindowSizeSlider.setBounds((getWidth() / 20) * 18, (getHeight() / 20) * 3.75, (getWidth() / 20) * 2, (getHeight() / 20) * 2);
    
    float sequencerStartY = (getHeight() / 10) * 6.9 + (getHeight() / 15);
    sequencer.setBounds(0, sequencerStartY, getWidth(), getHeight() - sequencerStartY);

}

void SampleChopperAudioProcessorEditor::timerCallback()
{
    waveformDisplay.repaint();
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
            
            juce::File audioFile = url.getLocalFile();
            globalAudioFile = audioFile;
            waveformDisplay.detectTransients(audioFile);
        }
    }
    
    if(&showTransientsButton == button)
    {
        if(showTransientsFlag == false)
        {
            waveformDisplay.showTransients();
            showTransientsFlag = true;
            DBG("Showing Transients");
        }
        else if(showTransientsFlag == true)
            {
                waveformDisplay.hideTransients();
                showTransientsFlag = false;
                DBG("Hiding Transients");
            }
        }
    
    bool isSelectorButtonPressed;
   
    for(int i = 0; i < selectorList.size(); i++)
    {
        if(selectorList[i] == button)
        {
            isSelectorButtonPressed = true;
            break;
        }else
        {
            isSelectorButtonPressed = false;
        }
    }
    
    if(isSelectorButtonPressed)
    {
            for(int i = 0; i < selectorList.size(); i++)
        {
            selectorList[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
            
            if(selectorList[i] == button)
            {
                
                if(bankSelected != i + 1)
                {
                    bankSelected = i + 1;
                    waveformDisplay.setBankSelected(bankSelected);
                    sequencer.setCurrentBank(bankSelected);
                    selectorList[i]->setColour(juce::TextButton::ColourIds::buttonColourId, myColours[i]);
                    
                }else
                    {
                        bankSelected = 6;
                        waveformDisplay.setBankSelected(bankSelected);
                    
                            for(int i = 0; i < selectorList.size(); i++)
                        {
                            selectorList[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
                        }
                    }
                }
            }
        
    }
    
    DBG(bankSelected);
    
    
    //Bank used to listen to the audio (has no GUI)
    Bank * listenerBank = audioProcessor.getListenerBank();
   
    if(listenerBank->isURLLoaded())
    {
        if(&listenerBankPlay == button)
        {
            listenerBank->play();
        }
        if(&listenerBankStop == button)
        {
            listenerBank->stop();
        }
        
    }
    
}

void SampleChopperAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if(&globalPitchSlider == slider)
    {
        float speed = (globalPitchSlider.getValue() / 10) + 1;
        
        for(int i = 0; i < bankList.size(); i++)
        {
            bankList[i]->setSpeed(speed);
        }
    }
    
    if(&transientWindowSizeSlider == slider)
    {
        waveformDisplay.setTransientWindowSize(transientWindowSizeSlider.getValue()); //this wont work as need to pass the recent file loaded into that bank
        waveformDisplay.detectTransients(globalAudioFile);
        waveformDisplay.repaint();
    }
    if(&transientSensitivitySlider == slider)
    {
        waveformDisplay.setTransientSensitvity(transientSensitivitySlider.getValue());
        waveformDisplay.detectTransients(globalAudioFile);
        waveformDisplay.repaint();
    }
    
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
    
    //drawing loop regions
    waveformDisplay.drawLoopRegions(loopRegions, g);
    
    
    
    //drawing playheads
    std::vector<float>bankPositions;
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankPositions.push_back(audioProcessor.getBank(i + 1)->getPositionRelative());
    }
    
    Bank * listenerBank = audioProcessor.getListenerBank();
    float listenerBankCurrentPosition = 0;
    
    
    if(listenerBank->isURLLoaded())
    {
        if(listenerBank->getPositionRelative() > 0)
        {
            listenerBankCurrentPosition = listenerBank->getPositionRelative();
        }
        
        
        waveformDisplay.drawPlayheads(loopRegions, bankPositions, listenerBankCurrentPosition, g);
    }
    
    
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
            
            if(bankSelected < 6)
            {
                bankList[bankSelected - 1]->setLoopRegion(trueStart, trueEnd); //0-1 percentage of file
            }
            else if(bankSelected == 6)
            {
                bankList[5]->setPosition(trueEnd * thumbnail->getTotalLength());
            }
            
        }
    }
    
}

