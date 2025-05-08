/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleChopperAudioProcessorEditor::SampleChopperAudioProcessorEditor (SampleChopperAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts), waveformDisplay(*audioProcessor.getFormatManager(), audioProcessor.thumbCache)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1100, 700);
    
    //Creates the bank GUI's
    for(int i = 0; i < guiList.size(); i++)
    {
        addAndMakeVisible(guiList[i]);
        guiList[i]->addMouseListener(this, false);
    }
    
    
    //waveform display
    
    //Loading audio file into waveform
    juce::String stringFromTree = valueTreeState.state.getProperty("currentFilePath");
    juce::URL fromTree{stringFromTree};
    
    if(fromTree.isLocalFile() && fromTree.isWellFormed())
    {
        waveformDisplay.loadURL(fromTree);
        juce::File file = fromTree.getLocalFile();
        
        transients = waveformDisplay.detectTransients(file);
        
        for(int i = 0; i < bankList.size(); i++)
        {
            bankList[i]->setTransients(transients);
        }
    }
    else
    {
        DBG("Editor not found audio file");
        
    }
    
    //waveform display
    addAndMakeVisible(waveformDisplay);
    
    waveformDisplay.setFileDroppedCallback([this](const juce::URL& fileURL) //this sets the editor as a listener and when it receives the return from the setfiledroppedcallback function inside waveform display, it will call filedroppedonwaveform
    {
        fileDroppedOnWaveform(fileURL); //callback function
    });
    
    waveformDisplay.setColours(myColours); //gives the waveform object the colours used for bank selection
    waveformDisplay.setBankSelected(bankSelected);
    
    //Loadbutton
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    
    
    //Preset
    addAndMakeVisible(presetGUI);
    
    
   // pitch slider that effects the entire track
//    addAndMakeVisible(globalPitchSlider);
//    globalPitchSlider.addListener(this);
//    pitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "globalPitch", globalPitchSlider));
//    sliderValueChanged(&globalPitchSlider);
    
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    volumeLabel.attachToComponent(&volumeSlider, false);
    volumeLabel.setJustificationType(juce::Justification::left);
    
    pitchLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    pitchLabel.attachToComponent(&pitchSlider, false);
    pitchLabel.setJustificationType(juce::Justification::centredTop);
//
    
    speedLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centredTop);
//
//
    
    
    addAndMakeVisible(volumeSlider);
    volumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "masterVolume", volumeSlider));
    volumeSlider.addListener(this);
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(volumeLabel);
    volumeLabel.setJustificationType(juce::Justification::centredTop);
    
  
//
    addAndMakeVisible(pitchSlider);
    pitchSlider.addListener(this);
    pitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "pitch", pitchSlider));
    
    addAndMakeVisible(speedSlider);
    speedSlider.addListener(this);
    speedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "speed", speedSlider));

    addAndMakeVisible(showTransientsButton);
    showTransientsButton.addListener(this);
    
    addAndMakeVisible(transientSensitivitySlider);
    transientSensitivitySlider.addListener(this);
    transientSensitivitySlider.setRange(0.5, 60.f);
    transientSensitivitySlider.setValue(60.f);
    transientSensitivitySlider.setNumDecimalPlacesToDisplay(1);
    
    addAndMakeVisible(transientWindowSizeSlider);
    transientWindowSizeSlider.addListener(this);
    transientWindowSizeSlider.setRange(10,200);
    transientWindowSizeSlider.setValue(10);
    transientWindowSizeSlider.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(bankScrollSlider);
    bankScrollSlider.addListener(this);
    bankScrollSlider.setRange(0, 1);
    bankScrollSlider.setValue(0.f);
    
    addAndMakeVisible(randomiseLoops);
    randomiseLoops.addListener(this);
    
//    globalPitchSlider.setRange(0.5, 2.0f);
//    
    addAndMakeVisible(clearLoopsButton);
    clearLoopsButton.addListener(this);
    
    monoAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "mono", monoToggle));
    addAndMakeVisible(monoToggle);
    monoToggle.addListener(this);
    
    
    //Bank selector Buttons
    for(int i = 0; i < selectorButtons.size(); i++)
    {
        addAndMakeVisible(selectorButtons[i]);
        selectorButtons[i]->addListener(this);
        selectorButtons[i]->setButtonText("Select");
        selectorButtons[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
        selectorButtons[i]->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    }
    
    std::vector<Bank*> banksList = audioProcessor.getBanksList();
    
    
    //Timer used for playhead
    startTimer(20);
    
    //Adds a mouseListener to waveform display for the editor (for file callback)
    waveformDisplay.addMouseListener(this, false);
    
    
    Bank * listenerBank = audioProcessor.getListenerBank();
    listenerBank->makeBankListener(true);
    
//    addAndMakeVisible(globalControlsBorder);
//    globalControlsBorder.setToggleable(false);
//    globalControlsBorder.set
//    globalControlsBorder.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::lightblue);
//    globalControlsBorder.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::lightblue);
    
}

SampleChopperAudioProcessorEditor::~SampleChopperAudioProcessorEditor()
{
    
    //Assigning pointer vectors to nullptr - freeing memory
    for(int i = 0; i < guiList.size(); i++)
    {
        guiList[i] = nullptr;
    }
    
    for(int i = 0; i < selectorButtons.size(); i++)
    {
        selectorButtons[i] = nullptr;
    }
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankList[i] = nullptr;
    }
    
//    pitchAttachment = nullptr;
    stopTimer();
    
}

//==============================================================================
void SampleChopperAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    
    //globalControlsBorder.setBounds((getWidth() / 20) * 16.2, waveformStartY + getHeight() / 4, (getWidth() / 20) * 3.5, (getHeight() / 20) * 13);
    
    
    //global controls
    g.drawRect((getWidth() / 20) * 16.2, getHeight() / 20 * 11.25, (getWidth() / 20) * 3.5, (getHeight() / 20) * 8);
    
    //chop manipulation
    g.drawRect((getWidth() / 20) * 16.2, getHeight() / 24, (getWidth() / 20) * 3.5, getHeight() / 4);
    
    
    
}

void SampleChopperAudioProcessorEditor::resized()
{
    
    float waveformStartX = getWidth() / 10 * 1.5;
    float waveformStartY = getHeight() / 20 * 1.25;
    float waveformWidth = (getWidth()/ 20) * 13;
    
    float waveformEndX = waveformStartX + waveformWidth + (getHeight() / 30);
    
    loadButton.setBounds(waveformEndX,waveformStartY,waveformStartX, getHeight() / 20);
//    globalPitchSlider.setBounds(waveformEndX, waveformStartY + (getHeight() / 20), waveformStartX, getHeight() / 20);
    clearLoopsButton.setBounds(waveformEndX, waveformStartY + (getHeight()/ 20),waveformStartX, getHeight() / 20);
    showTransientsButton.setBounds(waveformEndX, waveformStartY + (getHeight()/ 20) * 2, waveformStartX, getHeight() / 20);
    monoToggle.setBounds(waveformEndX, waveformStartY + (getHeight() / 20) * 3, waveformStartX, getHeight() / 20);
    
    presetGUI.setBounds(waveformStartX,0, waveformWidth, waveformStartY);
    waveformDisplay.setBounds(waveformStartX,waveformStartY, waveformWidth, (getHeight() / 10) * 2.5);
    
    volumeSlider.setBounds((getWidth() / 20) * 17.2 , getHeight() / 4 * 2.5, (getWidth() / 10), getHeight() / 3);
    
    //globalControlsBorder.setBounds((getWidth() / 20) * 16.2, waveformStartY + getHeight() / 4, (getWidth() / 20) * 3.5, (getHeight() / 20) * 13);
    //transientWindowSizeSlider.setBounds((getWidth() / 20) * 18, (getHeight() / 20) * 3.75, (getWidth() / 20) * 2, (getHeight() / 20) * 2);
    
    
    

    
    float column = waveformWidth / guiList.size();
    float row = getHeight() / 10;
    
    for(int i = 0; i < guiList.size(); i++)
    {
        guiList[i]->setBounds((waveformStartX) + column * i, (getHeight() / 10) * 3.25, column, row * 7);
    }
    
    for(int i = 0; i < selectorButtons.size(); i++)
    {
        selectorButtons[i]->setBounds(((waveformStartX) + column * i) + column / 4, (getHeight() / 14) * 13, column / 2, getHeight() / 28);
    }
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
            
            juce::String urlString = url.toString(false);
        
            audioProcessor.apvts.state.setProperty("currentFilePath", urlString, nullptr);
        
            audioProcessor.loadURLS(url);
            waveformDisplay.loadURL(url);
            
            juce::File audioFile = url.getLocalFile();
            selectedAudioFile = audioFile;
            transients = waveformDisplay.detectTransients(audioFile);
            
            for(int i = 0; i < bankList.size(); i++)
            {
                bankList[i]->setTransients(transients);
            }
        }
    }
    
    if(&showTransientsButton == button)
    {
        if(showTransientsFlag == false)
        {
            juce::String stringFromTree = valueTreeState.state.getProperty("currentFilePath");
            juce::URL fromTree{stringFromTree};
            
            if(fromTree.isLocalFile() && fromTree.isWellFormed())
            {
                juce::File file = fromTree.getLocalFile();
                waveformDisplay.detectTransients(file);
                
                waveformDisplay.showTransients();
                showTransientsFlag = true;
                DBG("Showing Transients");
            }
        }
        else if(showTransientsFlag == true)
            {
                waveformDisplay.hideTransients();
                showTransientsFlag = false;
                DBG("Hiding Transients");
            }
        }
    
    if(&clearLoopsButton == button)
    {
        audioProcessor.clearLoops();
    }
    
    if(&randomiseLoops == button)
    {
        DBG("randomising loops");
    }
    
    
    
    if(&monoToggle == button)
    {
        bool mono = monoToggle.getToggleState();
        
        audioProcessor.setMonoState(mono);
       
        DBG("Mono: " + std::to_string(mono));
    }
    
    
    for(int i = 0; i < selectorButtons.size(); i++)
    {
        if(selectorButtons[i] == button)
        {
            for(int i = 0; i < selectorButtons.size(); i ++)
            {
                selectorButtons[i]->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey); //turn them all off
            }
            bankSelected = i + 1;
            waveformDisplay.setBankSelected(bankSelected);
            selectorButtons[i]->setColour(juce::TextButton::ColourIds::buttonColourId, myColours[i]);
        }
    }
    
}

void SampleChopperAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if(&volumeSlider == slider)
    {
        for(int i = 0; i < bankList.size(); i++)
        {
            bankList[i]->setMasterGain(volumeSlider.getValue());
        }
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
    
    std::vector<Interval<float>> loopRegions = {audioProcessor.getBank(1)->getLoopRegion(), audioProcessor.getBank(2)->getLoopRegion(), audioProcessor.getBank(3)->getLoopRegion(), audioProcessor.getBank(4)->getLoopRegion(), audioProcessor.getBank(5)->getLoopRegion(), audioProcessor.getBank(6)->getLoopRegion()};
    
    std::vector<bool>banksPlaying = {audioProcessor.getBank(1)->getTransportSource()->isPlaying(), audioProcessor.getBank(2)->getTransportSource()->isPlaying(), audioProcessor.getBank(3)->getTransportSource()->isPlaying(), audioProcessor.getBank(4)->getTransportSource()->isPlaying(), audioProcessor.getBank(5)->getTransportSource()->isPlaying(), audioProcessor.getBank(6)->getTransportSource()->isPlaying()};
    
    //drawing loop regions
    waveformDisplay.drawLoopRegions(loopRegions, g);
    
    //drawing playheads
    std::vector<float>bankPositions;
    
    for(int i = 0; i < bankList.size(); i++)
    {
        bankPositions.push_back(audioProcessor.getBank(i + 1)->getPositionRelative()); //getting position
    }
    
    Bank * listenerBank = audioProcessor.getListenerBank();
    float listenerBankCurrentPosition = 0; //if no file loaded
    
    if(listenerBank->isURLLoaded())
    {
        if(listenerBank->getPositionRelative() > 0)
        {
            listenerBankCurrentPosition = listenerBank->getPositionRelative();
        }
        
        
        waveformDisplay.drawPlayheads(loopRegions, bankPositions, banksPlaying, listenerBankCurrentPosition, g);
    }
    
    
}

void SampleChopperAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if(&waveformDisplay == event.eventComponent)
    {
        mouseDrag(event);
        return;
    }
    
    
}

void SampleChopperAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent &event, juce::MouseWheelDetails &wheel)
{
    DBG("Mouse Wheel Moved");
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

            
            //pseudo code
            //if true start or new end based off of new input matches any of the banks loop start or end points
            //debug
            //rather than setting the loop region, adjust the start point and keep the end point the same. can get end point
            //with get loopRegion method
            
            
            if(bankSelected < 7) //chane to 9
            {
                bankList[bankSelected - 1]->setLoopRegion(trueStart, trueEnd); //0-1 percentage of file
            }
            
            
        }
    }
    
}

void SampleChopperAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    
}

