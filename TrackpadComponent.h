#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>

/**
 * Base component
 */
class TrackpadComponent : public juce::AnimatedAppComponent {
public:
    TrackpadComponent() : TrackpadComponent(512, 96, 60) {}
    TrackpadComponent(int size_, int inset_ = 0, int fps_ = 60);
    void paint (juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void resized() override;
    void update() override;

protected:
    double x, y;
    int size, inset, fps;
    std::tuple<float, float> calculateRelativePosition(int pixelX, int pixelY);
    std::tuple<int, int> calculatePixelPosition(float relX, float relY);
    double euclideanDistance(float x1, float y1, float x2, float y2);

private:
    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackpadComponent)
};
