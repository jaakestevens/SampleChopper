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
BankGUI::BankGUI(Bank& bank) : bank(bank)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    
    //Labels
    volumeLabel.setText("Volume", juce::dontSendNotification);
    attackLabel.setText("Attack", juce::dontSendNotification);
    decayLabel.setText("Decay", juce::dontSendNotification);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    
    for(int i = 0; i < labels.size(); i++)
    {
        labels[i]->setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        labels[i]->attachToComponent(sliders[i], false);
        labels[i]->setJustificationType(juce::Justification::centredTop);
        
    }
    
    //Buttons
    addAndMakeVisible(playButton);
    addAndMakeVisible(panningSlider);
    
    panningSlider.addListener(this);
    panningSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(pitchSlider);
    pitchSlider.addListener(this);
    pitchSlider.setTextBoxStyle(juce::Slider::NoTextBox,false,0, 0);
    
    playButton.addListener(this);
    stopButton.addListener(this);
    
    
    //Sliders
    for(int i = 0; i < sliders.size(); i++)
    {
        addAndMakeVisible(sliders[i]);
        sliders[i]->addListener(this);
        sliders[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    }
    
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 40, 20);
    volumeSlider.setNumDecimalPlacesToDisplay(2);
    
    
    juce::ADSR::Parameters* params = bank.getAdsrParameters();
    
    float attackVal = params->attack * 2;
    float decayVal = params->decay * 2;
    float sustainVal = params->sustain * 10;
    
    attackSlider.setValue(attackVal);
    decaySlider.setValue(decayVal);
    sustainSlider.setValue(sustainVal);
    
    attackSlider.setRange(0.f, 5.f);
    decaySlider.setRange(0.f, 5.f);
    
    panningSlider.setRange(-1.f, 1.f);
    panningSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    
    pitchSlider.setRange(-0.1f, 0.1f);
    pitchSlider.setValue(0);
    //pitchSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    
}

BankGUI::~BankGUI()
{
    for(int i = 0; i < labels.size(); i++)
    {
        labels[i] = nullptr;
    }
   
    for(int i = 0; i < sliders.size(); i++)
    {
        sliders[i] = nullptr;
    }
    
}

void BankGUI::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void BankGUI::resized()
{
   
    auto rowH = getHeight () / 10;
    
    playButton.setBounds(0, 0, getWidth(), rowH);
    
    panningSlider.setBounds(0, rowH, getWidth(), rowH * 3);
    
    volumeSlider.setBounds(0, 5.5 *rowH, getWidth(), rowH);
    
    attackSlider.setBounds(0, 8*rowH, getWidth() / 2, rowH);
    decaySlider.setBounds(getWidth() / 2, 8*rowH, getWidth() / 2, rowH);
    sustainSlider.setBounds(0, 10*rowH, getWidth() / 2, rowH);
    
    pitchSlider.setBounds(getWidth() / 2, 10*rowH, getWidth() / 2, rowH);
    
    
    

}

void BankGUI::buttonClicked(juce::Button *button)
{
    
    if(&playButton == button)
    {
        bank.play();
   
    }
    
    else if(&stopButton == button)
    {
        bank.stop();
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
        
        //DBG("attack  : "  << attack);
        
    }
    if(&decaySlider == slider)
    {
        
        float decay = decaySlider.getValue() / 2;
        
        adsrParams->decay = decay;
        
        bank.setAdsrParameters(*adsrParams);
        
        DBG("decay  : "  << decay);
    }
    if(&sustainSlider == slider)
    {
        
        float sustain = sustainSlider.getValue() / 10; //between 0-1
        
        adsrParams->sustain = sustain;
        
        bank.setAdsrParameters(*adsrParams);
        
        DBG("sustain  : "  << sustain);
    }
    
    if(&panningSlider == slider)
    {
        bank.setPanning(panningSlider.getValue());
    }
    
    if(&pitchSlider == slider)
    {
        
    }
}


