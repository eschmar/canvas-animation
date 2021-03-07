#include "UnknownPleasuresComponent.h"

UnknownPleasuresComponent::UnknownPleasuresComponent(
    int size_,
    int inset_,
    int fps_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
}

void UnknownPleasuresComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

}

void UnknownPleasuresComponent::update() {
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

void UnknownPleasuresComponent::resized() {
    // This is called when the UnknownPleasuresComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
