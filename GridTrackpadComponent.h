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
class GridTrackpadComponent : public TrackpadComponent {
public:
    GridTrackpadComponent() : GridTrackpadComponent(TRACKPAD_DEFAULT_SIZE) {}
    GridTrackpadComponent(int size_, int inset_ = 96, int fps_ = 60, float radius_ = 96, float gridSize_ = 12, float maxPointSize_ = 6);
    void paint (juce::Graphics&) override;
    void resized() override;
    void update() override;

private:
    float radius, gridSize, maxPointSize;
    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridTrackpadComponent)
};
