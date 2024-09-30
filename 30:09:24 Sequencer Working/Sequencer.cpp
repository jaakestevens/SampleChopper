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
    
    for(int i = 0; i < stepButtons.size(); i++)
    {
        addAndMakeVisible(stepButtons[i]);
        stepButtons[i]->addListener(this);
        stepButtons[i]->setToggleable(true);
        stepButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
            
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
    float column = this->getWidth() / 8;
    
    float startingPoint = (column * 3) - column * 0.2;
    float spaceLeft = this->getWidth() - startingPoint;
    float increment = spaceLeft / 16;

    for(int i = 0; i < 16; i++)
    {
        
        g.setColour(i == currentStep ? juce::Colours::red : juce::Colours::grey);
        g.fillEllipse(startingPoint + (increment * i), 0, 10, 10);
    }
}

void Sequencer::resized()
{
    int column = this->getWidth() / 8;
    seqStart.setBounds(0,0,column, this->getHeight() / 2);
    increaseStepsButton.setBounds(column,0,column,this->getHeight() / 4);
    decreaseStepsButton.setBounds(column,this->getHeight() / 4,column,this->getHeight() / 4);
    
    float radioButtonHeight = this->getHeight() / 6;
    
    buttonHalf.setBounds(column * 2, 0, column, radioButtonHeight);
    buttonQuarter.setBounds(column * 2, radioButtonHeight * 1, column , radioButtonHeight);
    buttonEighth.setBounds(column * 2, radioButtonHeight * 2, column, radioButtonHeight);
    buttonSixteenth.setBounds(column * 2, radioButtonHeight * 3, column, radioButtonHeight);
    buttonThirtySecond.setBounds(column * 2, radioButtonHeight * 4, column, radioButtonHeight);
    
    
    //for the sequencer
    float startingPoint = (column * 3) - column * 0.2;
    float spaceLeft = this->getWidth() - startingPoint;
    float increment = spaceLeft / 16;
    
    for(int i = 0; i < stepButtons.size(); i++)
    {
        stepButtons[i]->setBounds(startingPoint + (increment * i), this->getHeight() / 2, increment / 2, this->getHeight() / 8);
    }

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
    
    //if current step is equal to number in sequence
    //bank1 should trigger sample (using bankGUI playbutton as editor is middleman)
    for(int i = 0; i < 5; i++)
    {
        int bank = i;
        if(sequence[bank][currentStep] == 1)
        {
            triggerSample(bank);
        }
    }
//    if(sequence[][currentStep] == 1)
//    {
//        triggerSample();
//    }
    
    repaint();
}

void Sequencer::buttonClicked(juce::Button *button)
{
    
    for(int i = 0; i < stepButtons.size(); i++)
    {
        if(stepButtons[i] == button)
        {
            juce::TextButton * stepButton = stepButtons[i];
            
            if(currentBank < 6)
            {
                if(sequence[currentBank - 1][i] == 1)
                {
                    stepButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
                    sequence[currentBank - 1][i] = 0;
                }else
                {
                    stepButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                    sequence[currentBank - 1][i] = 1;
                }
            }
            
        }
    }
    
    
    
    
    
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

void Sequencer::setCurrentBank(int bank) //1 - 5
{
    currentBank = bank;
    
    for(int i = 0; i < stepButtons.size(); i++)
    {
        stepButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        if(sequence[currentBank - 1][i] == 1)
        {
            stepButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        }
    }
    DBG(bank);
}


