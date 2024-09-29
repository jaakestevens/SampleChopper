/*
  ==============================================================================

    Sequencer.h
    Created: 27 Sep 2024 10:05:28pm
    Author:  Jake

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Sequencer  : public juce::Component, public juce::Timer,  public juce::Button::Listener, public juce::Slider::Listener
{
public:
    Sequencer();
    ~Sequencer() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override;
    void sliderValueChanged(juce::Slider * slider) override;
    void updateTimerInterval();
    void timerCallback() override;

    void start();
    
    void stop();
    
    std::function<void()>triggerSample;
private:
    
    juce::TextButton seqStart{"Start"};
    juce::TextButton increaseStepsButton{"+1 steps"};
    juce::TextButton decreaseStepsButton{"-1 steps"};
   
    juce::Slider stepCountSlider;
    std::vector<int> stepValues = {4, 8, 16, 32, 64};
    
    juce::Slider speedSlider;
    std::vector<double> speedValues = {0.5, 1.0, 2.0, 4.0, 8.0, 16.0};
    int currentSpeedIndex;
    
    juce::ToggleButton buttonHalf, buttonQuarter, buttonEighth, buttonSixteenth, buttonThirtySecond;
    std::vector<juce::ToggleButton*> speedButtonVector = {&buttonHalf, &buttonQuarter, &buttonEighth, &buttonSixteenth, &buttonThirtySecond};
    
    juce::TextButton step1, step2, step3, step4, step5, step6, step7, step8, step9, step10, step11, step12, step13, step14, step15, step16;
   
    std::vector<juce::TextButton*>stepButtons = {&step1, &step2, &step3, &step4, &step5, &step6, &step7, &step8, &step9, &step10, &step11, &step12, &step13, &step14, &step15, &step16};
    
    std::vector<int> sequence = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    int bpm = 120;
    int stepsPerSequence = 16;
    double stepDurationMs;
    int currentStep = 0;
    bool speedChange = false;
    
    bool sequencePlaying = true;
    
    int calculateIntervalMs();
    void processStep(int step);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sequencer)
};
