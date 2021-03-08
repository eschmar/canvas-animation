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
class UnknownPleasuresComponent : public TrackpadComponent {
public:
    UnknownPleasuresComponent() : UnknownPleasuresComponent(512) {}
    UnknownPleasuresComponent(
        int size_,
        int inset_ = 96,
        int fps_ = 60,
        int verticalOffset_ = 16,
        int horizontalStepOffset_ = 16,
        int radius_ = 96
    );

    void paint (juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void resized() override;
    void update() override;

private:
    int verticalOffset, horizontalStepOffset, radius;
    std::vector<std::vector<float>> position, target;
    void computeTarget(bool fastforward = false);

    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnknownPleasuresComponent)
};
