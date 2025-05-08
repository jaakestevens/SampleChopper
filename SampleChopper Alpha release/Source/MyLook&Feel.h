/*
  ==============================================================================

    MyLook&Feel.h
    Created: 22 Mar 2023 4:27:24pm
    Author:  Jake

  ==============================================================================
*/
#pragma once
using namespace juce;
class MyLookAndFeel : public juce::LookAndFeel_V4
{
    
    Image testImage = ImageCache::getFromMemory(BinaryData::panning3_jpg, BinaryData::panning3_jpgSize);

    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        

        if (slider.isBar())
        {
            g.setColour (slider.findColour (Slider::trackColourId));
            g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                              : Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)));

            drawLinearSliderOutline (g, x, y, width, height, style, slider);
        }
        else
        {
            auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
            auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

            auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

            Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                     slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

            Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                                   slider.isHorizontal() ? startPoint.y : (float) y);

            Path backgroundTrack;
            backgroundTrack.startNewSubPath (startPoint);
            backgroundTrack.lineTo (endPoint);
            g.setColour (slider.findColour (Slider::backgroundColourId));
            g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            Path valueTrack;
            Point<float> minPoint, maxPoint, thumbPoint;

            if (isTwoVal || isThreeVal)
            {
                minPoint = { slider.isHorizontal() ? minSliderPos : (float) width * 0.5f,
                             slider.isHorizontal() ? (float) height * 0.5f : minSliderPos };

                if (isThreeVal)
                    thumbPoint = { slider.isHorizontal() ? sliderPos : (float) width * 0.5f,
                                   slider.isHorizontal() ? (float) height * 0.5f : sliderPos };

                maxPoint = { slider.isHorizontal() ? maxSliderPos : (float) width * 0.5f,
                             slider.isHorizontal() ? (float) height * 0.5f : maxSliderPos };
            }
            else
            {
                auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
                auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

                minPoint = startPoint;
                maxPoint = { kx, ky };
            }

            auto thumbWidth = getSliderThumbRadius (slider);

            valueTrack.startNewSubPath (minPoint);
            valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
            g.setColour (slider.findColour (Slider::trackColourId));
            g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            if (! isTwoVal)
            {
                g.setColour (slider.findColour (Slider::thumbColourId));
                g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
               
            }

            if (isTwoVal || isThreeVal)
            {
                auto sr = jmin (trackWidth, (slider.isHorizontal() ? (float) height : (float) width) * 0.4f);
                auto pointerColour = slider.findColour (Slider::thumbColourId);

                if (slider.isHorizontal())
                {
                    drawPointer (g, minSliderPos - sr,
                                 jmax (0.0f, (float) y + (float) height * 0.5f - trackWidth * 2.0f),
                                 trackWidth * 2.0f, pointerColour, 2);

                    drawPointer (g, maxSliderPos - trackWidth,
                                 jmin ((float) (y + height) - trackWidth * 2.0f, (float) y + (float) height * 0.5f),
                                 trackWidth * 2.0f, pointerColour, 4);
                }
                else
                {
                    drawPointer (g, jmax (0.0f, (float) x + (float) width * 0.5f - trackWidth * 2.0f),
                                 minSliderPos - trackWidth,
                                 trackWidth * 2.0f, pointerColour, 1);

                    drawPointer (g, jmin ((float) (x + width) - trackWidth * 2.0f, (float) x + (float) width * 0.5f), maxSliderPos - sr,
                                 trackWidth * 2.0f, pointerColour, 3);
                }
            }

            if (slider.isBar())
                drawLinearSliderOutline (g, x, y, width, height, style, slider);
        }
    };
        
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
//        auto radius = (float) juce::jmin(width / 2, height / 2) - 4.0f; //jmin returns the smaller of 2 values
//        auto centreX = (float) x + (float) width * 0.5f; //brackets are for typecasting
//        auto centreY = (float) y + (float) height * 0.5f;
//        auto rx = centreX - radius; //left
//        auto ry = centreY - radius; //top
//        auto rw = radius * 2.0f; //width
//        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle); //angle at which dial will point
//        
//        g.setColour(juce::Colours::white);
//        g.fillEllipse(rx, ry, rw, rw);
//        
//        g.setColour(juce::Colours::lightblue);
//        g.drawEllipse(rx, ry, rw, rw, 1.0f);
//        
//        juce::Path p;
//        
//        auto pointerLength = radius * 0.7f; //pointer as in point on the dial
//        auto pointerThickness = 3.0f;
//        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
//        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
//        
//        g.setColour(juce::Colours::black);
//        g.fillPath(p);
        // Calculate bounds of the dial
            auto diameter = juce::jmin(width, height);
            auto radius = diameter * 0.5f;
            auto centreX = x + width * 0.5f;
            auto centreY = y + height * 0.5f;

            juce::Rectangle<float> dialArea(centreX - radius, centreY - radius, diameter, diameter);

            // Rotation angle based on slider value
            float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

            // Save current graphics state
            g.saveState();

            // Move to center, apply rotation, then move back
            g.addTransform(juce::AffineTransform::rotation(angle, centreX, centreY));

            // Draw the image (assuming yourImage is already loaded and square)
            g.drawImage(testImage, dialArea);

            // Restore graphics state to avoid affecting later drawing
            g.restoreState();
        
    }
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        auto edge = 4;
        
        buttonArea.removeFromLeft(edge);
        buttonArea.removeFromTop(edge);
        
        //shadow
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        g.fillRect(buttonArea);
        auto offset = isButtonDown ? -edge / 2 : -edge; //removes more if button is dowm
        buttonArea.translate(offset, offset);
        
        g.setColour(juce::Colours::slategrey);
        if(isButtonDown)
        {
            g.setColour(juce::Colours::yellow);
            g.setOpacity(1.f);
        }

        g.fillRect(buttonArea);
        
    }
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        g.setColour(juce::Colours::black);
        
        auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
        auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
        
        auto leftIndent = 4;
        auto rightIndent = 4;
        auto textWidth = button.getWidth() - leftIndent - rightIndent;
        
        auto edge = 4;
        auto offset = isButtonDown ? edge / 2 : 0;
        
        if(textWidth > 0)
        {
            g.drawFittedText(button.getButtonText(), leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2,juce::Justification::centred, 2);
            
            g.setColour(juce::Colours::darkgrey.withAlpha(0.2f));
            
            g.drawFittedText(button.getButtonText(), leftIndent + offset * 3 + 4, yIndent + offset * 3 + 4, textWidth, button.getHeight() - yIndent * 2,juce::Justification::centred, 2);
            
        }
    }
    
private:
    
};
