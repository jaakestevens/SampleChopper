/*
  ==============================================================================

    BankGUI.cpp
    Created: 7 Sep 2024 2:47:46pm
    Author:  Jake

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BankGUI.h"

//==============================================================================
BankGUI::BankGUI(Bank& bank, juce::AudioProcessorValueTreeState& vts) : valueTreeState(vts), bank(bank)
{
    
    testImage = ImageCache::getFromMemory(BinaryData::BankBackground_jpg, BinaryData::BankBackground_jpgSize);
    
    bankNumber = bank.getBankNumber();
    
    //Labels
    volumeLabel.setText("Volume", juce::dontSendNotification);
    attackLabel.setText("Fade In", juce::dontSendNotification);
    fadeLabel.setText("Fade Out", juce::dontSendNotification);
    pitchLabel.setText("Pitch", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);
    
    for(int i = 0; i < volumeLabels.size(); i++)
    {
        volumeLabels[i]->setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        volumeLabels[i]->attachToComponent(volumeSliders[i], false);
        volumeLabels[i]->setJustificationType(juce::Justification::centredTop);
        
    }
    
    pitchLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    pitchLabel.attachToComponent(&pitchSlider, false);
    pitchLabel.setJustificationType(juce::Justification::centredTop);
    
    speedLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centredTop);
    
    //Play
    addAndMakeVisible(playButton);
    playButton.setColour(juce::TextButton::ColourIds::buttonColourId, myColours[bankNumber - 1]);
    playButton.addListener(this);
    playButton.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    
    //Chop scrolling
    addAndMakeVisible(incrementChop);
    incrementChop.addListener(this);
    incrementChop.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    addAndMakeVisible(decrementChop);
    decrementChop.addListener(this);
    decrementChop.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    //reset
    addAndMakeVisible(resetButton);
    resetButton.addListener(this);
    
    
    //Panning
    addAndMakeVisible(panningSlider);
    panningSlider.addListener(this);
    panningSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(pitchSlider);
    pitchSlider.addListener(this);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    pitchSlider.setRange(0, 1.f);
    
    addAndMakeVisible(speedSlider);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    
    
    //Sliders
    for(int i = 0; i < volumeSliders.size(); i++)
    {
        addAndMakeVisible(volumeSliders[i]);
        volumeSliders[i]->addListener(this);
        volumeSliders[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    }
    
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 40, 20);
    volumeSlider.setNumDecimalPlacesToDisplay(2);
    volumeSlider.setValue(10.f);
    
    juce::ADSR::Parameters* params = bank.getAdsrParameters();
    
    float attackVal = params->attack * 2;
    
    attackSlider.setValue(attackVal);
   
    attackSlider.setRange(0.f, 5.f);
  
    fadeSlider.setRange(1.0f, 9.8f);
    fadeSlider.setValue(1.f);
    sliderValueChanged(&fadeSlider);
    
    panningSlider.setRange(-1.f, 1.f);
    panningSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    
    juce::String pitchID = "bank" + juce::String(bankNumber) + "Pitch";
    pitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, pitchID, pitchSlider));
    
    juce::String speedID = "bank" + juce::String(bankNumber) + "Speed";
    speedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, speedID, speedSlider));
    
    juce::String attackID = "bank" + juce::String(bankNumber) + "Attack";
    attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, attackID, attackSlider));
    
    juce::String releaseID = "bank" + juce::String(bankNumber) + "Release";
    releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, releaseID, fadeSlider));
    
    juce::String volumeID = "bank" + juce::String(bankNumber) + "Volume";
    volumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, volumeID, volumeSlider));
    
    juce::String panningID = "bank" + juce::String(bankNumber) + "Panning";
    panningAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, panningID, panningSlider));

    
}

BankGUI::~BankGUI()
{
    for(int i = 0; i < volumeLabels.size(); i++)
    {
        volumeLabels[i] = nullptr;
    }
   
    for(int i = 0; i < volumeSliders.size(); i++)
    {
        volumeSliders[i] = nullptr;
    }
    
    
}

void BankGUI::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::lightcyan);
    //g.drawImage(testImage, 0, 0, getWidth(), getHeight(), 0, 0, 100, 350);
    g.drawRect(0, 0, getWidth(), getHeight() - 30);
}

void BankGUI::resized()
{
   
    auto rowH = getHeight () / 16;
    
    playButton.setBounds(0, 0, getWidth(), rowH);
    
    decrementChop.setBounds(0 ,rowH,getWidth()/2, rowH);
    incrementChop.setBounds(getWidth() / 2,rowH,getWidth()/2, rowH);
    
    panningSlider.setBounds(0, rowH * 2.5, getWidth(), rowH * 1.5);
    
    volumeSlider.setBounds(0, 5.5 *rowH, getWidth(), rowH);
    
    attackSlider.setBounds(0, 8*rowH, getWidth() / 2, rowH);
    fadeSlider.setBounds(getWidth() / 2, 8*rowH, getWidth() / 2, rowH);
    
    pitchSlider.setBounds(0, 10* rowH, getWidth(), rowH);
    speedSlider.setBounds(0, 12 * rowH, getWidth(), rowH);
    
    resetButton.setBounds(getWidth()/ 4, 13*rowH, getWidth() / 2, rowH);
    
    
    
    

}

void BankGUI::buttonClicked(juce::Button *button)
{
    
    if(&playButton == button)
    {
        bank.play();
   
    }
    
    if(&resetButton == button)
    {
            speedSlider.setValue(0.0f);
            pitchSlider.setValue(0.0f);
    }
    
    if(&decrementChop == button)
    {
        float sampleLengthInSeconds = bank.getTransportSource()->getLengthInSeconds();
        
        float startStamp = bank.getLoopRegion().start() * sampleLengthInSeconds;
        float endStamp = bank.getLoopRegion().end() * sampleLengthInSeconds;
        
        float closestStart = 0;
        float end = 0;
        
        for(int i = 0; i < bank.getTransients().size(); i++)
        {
            if(startStamp > bank.getTransients()[i])
            {
                closestStart = bank.getTransients()[i];
                end = bank.getTransients()[i+1];
            }
        }
        
        DBG("Loop start time stamp: " + juce::String(startStamp));
        DBG("Loop end time stamp: " + juce::String(endStamp));
        
        DBG("New start time: " + juce::String(closestStart));
        DBG("New end time: " + juce::String(end));
        
        bank.setLoopRegion(closestStart / sampleLengthInSeconds, end / sampleLengthInSeconds);
        
    }
    
    if(&incrementChop == button)
    {
        float sampleLengthInSeconds = bank.getTransportSource()->getLengthInSeconds();
        float endStamp = bank.getLoopRegion().end() * sampleLengthInSeconds;
        
        float closestStart = 0;
        float end = 0;
       
        if(bank.getTransients().size() > 0)
        {
            for(int i = 0; i < bank.getTransients().size(); i++)
            {
                if(endStamp > bank.getTransients()[i])
                {
                    closestStart = bank.getTransients()[i+1];
                    end = bank.getTransients()[i + 2];
                }
            }
            
            bank.setLoopRegion(closestStart / sampleLengthInSeconds, end / sampleLengthInSeconds);
        }
       
        
    }
}

void BankGUI::sliderValueChanged(juce::Slider *slider)
{
    
    
    juce::ADSR::Parameters* adsrParams = bank.getAdsrParameters();
    
    if(&volumeSlider == slider)
    {
        float volume = slider->getValue() / 10;
        bank.setGain(volume);
        
    }
    
    //ADSR
    if(&attackSlider == slider) // slider val is 0-10
    {
        
        float attack = attackSlider.getValue() / 2;
        adsrParams->attack = attack;
        
        bank.setAdsrParameters(*adsrParams);
        
    }
    
    if(&fadeSlider == slider)
    {
        float invertedValue = 9.8 - fadeSlider.getValue() + 1.0f;
        bank.setFadeVal(invertedValue);
    }
    
    if(&panningSlider == slider)
    {
        bank.setPanning(panningSlider.getValue());
    }

    
    // Only respond to user interaction
       if (slider == &pitchSlider || slider == &speedSlider)
       {
           float pitchValue;
           float speedValue;
           
           if(pitchSlider.getValue() >  -0.02 && pitchSlider.getValue() < 0.02)
           {
               pitchValue = 0.0f;
           }else
           {
               pitchValue = pitchSlider.getValue();
           }
           
           if(speedSlider.getValue() > -0.02 && speedSlider.getValue() < 0.02)
           {
               speedValue = 0.0f;
           }else
           {
               speedValue = speedSlider.getValue();
           }
             bank.updatePitchAndSpeed(pitchValue, speedValue);
           
          
       }
       

}


