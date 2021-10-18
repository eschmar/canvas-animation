#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "TrackpadComponent.h"
#include "Point.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class ThermalComponent : public TrackpadComponent {
public:
    ThermalComponent() : ThermalComponent(TRACKPAD_DEFAULT_SIZE) {}
    ThermalComponent(
        int size_,
        int inset_ = 96,
        int fps_ = 144,
        float stepSize_ = 36.0,
        float blobSize_ = 52.0,
        juce::Colour gradientFrom_ = juce::Colour(194, 32, 19),
        juce::Colour gradientTo_ = juce::Colour(255, 155, 0)

        // blue
        // juce::Colour gradientFrom_ = juce::Colour(0, 163, 255),
        // juce::Colour gradientTo_ = juce::Colour(0, 55, 85)
    );

    void paint (juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void resized() override;
    void update() override;

private:
    std::vector<float> blobRadius, blobRadiusTarget;

    size_t verticeCount = 6, blobCount;

    float relX, relY, wobbler, stepSize, blobSize, epsilon;
    juce::Colour gradientFrom, gradientTo;

    void computeTarget(bool fastforward = false);
    float calculateBezierDistance(const float radius);

    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThermalComponent)
};
