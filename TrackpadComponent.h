#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "Point.h"

#define TRACKPAD_DEFAULT_SIZE 512
// #define TRACKPAD_DEFAULT_SIZE 390 // iPhone12

/**
 * Base component
 */
class TrackpadComponent : public juce::AnimatedAppComponent {
public:
    TrackpadComponent() : TrackpadComponent(TRACKPAD_DEFAULT_SIZE, 96, 60) {}
    TrackpadComponent(int size_, int inset_ = 0, int fps_ = 60);
    void paint (juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void resized() override;
    void update() override;

protected:
    Point<float> position, target;
    int size, inset, fps;

    std::tuple<float, float> calculateRelativePosition(int pixelX, int pixelY);
    std::tuple<int, int> calculatePixelPosition(float relX, float relY);

private:
    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackpadComponent)
};
