#include "MainComponent.h"

MainComponent::MainComponent() {
    setSize(512, 512);
    setFramesPerSecond (60);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
}

void MainComponent::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Draggable circle
    float radius = 24;
    g.setColour(juce::Colours::yellow);
    g.drawEllipse(x - radius, y - radius, radius * 2, radius * 2, 3);

    // Hello wolrd
    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::mouseDrag(const juce::MouseEvent& event) {
    x = event.x;
    y = event.y;
}

void MainComponent::update() {
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

void MainComponent::resized() {
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
