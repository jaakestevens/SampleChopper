/*
  ==============================================================================

    Sequencer.cpp
    Created: 27 Sep 2024 10:05:28pm
    Author:  Jake

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Sequencer.h"

//==============================================================================
Sequencer::Sequencer()
{
    addAndMakeVisible(seqStart);
    seqStart.addListener(this);
    
    addAndMakeVisible(increaseStepsButton);
    increaseStepsButton.addListener(this);
    
    addAndMakeVisible(decreaseStepsButton);
    decreaseStepsButton.addListener(this);
    
    // Create toggle buttons for different speed multipliers
    
    for(int i = 0; i < speedButtonVector.size(); i++)
    {
        speedButtonVector[i]->setRadioGroupId(1);
        addAndMakeVisible(speedButtonVector[i]);
        
        speedButtonVector[i]->onClick = [this, i] //captures i by value
        {
            currentSpeedIndex = i;
            speedChange = true;
        };
    }
            buttonHalf.setButtonText("1/2");
            buttonQuarter.setButtonText("1/4");
            buttonEighth.setButtonText("1/8");
            buttonSixteenth.setButtonText("1/16");
            buttonThirtySecond.setButtonText("1/32");
    
    buttonSixteenth.setToggleState(true, juce::dontSendNotification);
    
            currentSpeedIndex = 3;
            
}

Sequencer::~Sequencer()
{
    for(int i = 0; i < speedButtonVector.size(); i++)
    {
        speedButtonVector[i] = nullptr;
    }
}

void Sequencer::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    float startingPoint = (this->getWidth() / 8) * 4.5;
    float width = this->getWidth() - startingPoint;
    float increment = width / 16;

    for(int i = 0; i < 16; i++)
    {
        juce::Rectangle<float> stepRect(startingPoint + (increment * i), 0, increment - 5, 20);
        g.setColour(i == currentStep ? juce::Colours::green : juce::Colours::grey);
        g.fillRect(stepRect);
    }
}

void Sequencer::resized()
{
    int column = this->getWidth() / 8;
    seqStart.setBounds(0,0,column, this->getHeight() / 2);
    increaseStepsButton.setBounds(column,0,column * 2,this->getHeight() / 2);
    decreaseStepsButton.setBounds(column,this->getHeight() / 2,column * 2,this->getHeight() / 2);
    
    float radioButtonHeight = this->getHeight() / 6;
    
    buttonHalf.setBounds(column * 3, 0, column * 2, radioButtonHeight);
    buttonQuarter.setBounds(column * 3, radioButtonHeight * 1, column * 2, radioButtonHeight);
    buttonEighth.setBounds(column * 3, radioButtonHeight * 2, column * 2, radioButtonHeight);
    buttonSixteenth.setBounds(column * 3, radioButtonHeight * 3, column * 2, radioButtonHeight);
    buttonThirtySecond.setBounds(column * 3, radioButtonHeight * 4, column * 2, radioButtonHeight);

}

void Sequencer::updateTimerInterval()
{
    double quarterNoteDuratinMs = 60000.0 / bpm;
    
    stepDurationMs = quarterNoteDuratinMs / speedValues[currentSpeedIndex];
    
    DBG(stepDurationMs);
}

void Sequencer::timerCallback()
{
    
    if(currentStep < stepsPerSequence - 1)
    {
        currentStep++;
    }else
    {
        currentStep = 0;
    }
    
    if(speedChange == true)
    {
        start();
        speedChange = false;
    }
    
    repaint();
}

void Sequencer::buttonClicked(juce::Button *button)
{
    if(&seqStart == button)
    {
        if(sequencePlaying == false)
        {
            start();
            sequencePlaying = true;
            
        }else
        {
            stop();
            sequencePlaying = false;
        }
    }
    
    if(&increaseStepsButton == button)
    {
        if(stepsPerSequence < 16)
        {
            stepsPerSequence++;
            DBG("Steps : " << stepsPerSequence);
        }
    }
    
    if(&decreaseStepsButton == button)
    {
        if(stepsPerSequence > 1)
        {
            stepsPerSequence--;
            DBG("Steps : " << stepsPerSequence);
        }
    }
}

void Sequencer::sliderValueChanged(juce::Slider *slider)
{

}

void Sequencer::start()
{
    updateTimerInterval();
    int d = static_cast<int>(stepDurationMs);
    DBG(d);
    startTimer(d);
}

void Sequencer::stop()
{
    stopTimer();
}

int Sequencer::calculateIntervalMs()
{
    double quarterNoteDuration = 60000.0 / bpm;
    
    double stepDuration = quarterNoteDuration / speedValues[currentSpeedIndex];
    
    return stepDuration;
}


