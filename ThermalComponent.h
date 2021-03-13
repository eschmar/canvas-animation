#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "TrackpadComponent.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class ThermalComponent : public TrackpadComponent {
public:
    ThermalComponent() : ThermalComponent(512) {}
    ThermalComponent(
        int size_,
        int inset_ = 96,
        int fps_ = 60,
        float stepSize_ = 36.0,
        float blobSize_ = 38.0,
        juce::Colour gradientFrom_ = juce::Colour(255, 155, 0),
        juce::Colour gradientTo_ = juce::Colour(194, 32, 19)

        // blue
        // juce::Colour gradientFrom_ = juce::Colour(0, 163, 255),
        // juce::Colour gradientTo_ = juce::Colour(0, 55, 85)
    );

    void paint (juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void resized() override;
    void update() override;

private:
    float relX, relY, wobbler, stepSize, blobSize;
    juce::Colour gradientFrom, gradientTo;
    std::vector<float> coordinateX, coordinateY;
    void computeTarget(bool fastforward = false);
    void drawBlob(float centerX, float centerY, float radius, juce::Colour colour, juce::Graphics& g);

    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThermalComponent)
};
