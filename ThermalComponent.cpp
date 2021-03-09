#include "ThermalComponent.h"

ThermalComponent::ThermalComponent(
    int size_,
    int inset_,
    int fps_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
}

void ThermalComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Background circle
    float halfInset = inset * 0.5f;
    g.setColour(juce::Colour(37, 48, 54));
    g.fillEllipse(juce::Rectangle<float>(halfInset, halfInset, getWidth() - inset, getHeight() - inset));
    g.drawEllipse(juce::Rectangle<float>(halfInset - 6, halfInset - 6, getWidth() - inset + 12, getHeight() - inset + 12), 4.0f);

    // Draggable circle
    float radius = 16;
    g.setColour(juce::Colours::yellow);
    g.drawEllipse((float) x - radius, (float) y - radius, radius * 2, radius * 2, 3);
}

void ThermalComponent::mouseDrag(const juce::MouseEvent& event) {
    // printf("X>>> <x,y>=<%d,%d>\n", event.x, event.y);
    TrackpadComponent::mouseDrag(event);
    // printf("Y>>> <x',y'>=<%.2f,%.2f>\n", x, y);

    computeTarget();
}

void ThermalComponent::computeTarget(bool fastforward) {
    // Todo.
}

void ThermalComponent::update() {
    // Todo.
}

void ThermalComponent::resized() {
    // This is called when the ThermalComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
