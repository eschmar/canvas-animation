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
    relX = 0.5f;
    relY = 0.5f;
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
    float radius = (size - inset) / 2.0f;
    float center = size * 0.5f;
    float distance = (float) std::sqrt(std::pow(event.x - center, 2) + std::pow(event.y - center, 2));

    // Limit movement to circle
    if (float ratio = distance / radius; ratio > 1.0) {
        float deltaX = event.x - center;
        float deltaY = event.y - center;

        float length = (float) std::sqrt(std::pow(deltaX, 2.0) + std::pow(deltaY, 2.0));
        x = center + (deltaX / length) * radius;
        y = center + (deltaY / length) * radius;

    } else {
        x = event.x;
        y = event.y;
    }

    // map coordinates to unit circle
    float u = (float) (x - center) / radius;
    float v = (float) (y - center) / radius;

    // convert circle to square
    double u2 = u * u;
    double v2 = v * v;
    double twosqrt2 = 2.0 * std::sqrt(2.0);
    double subtermx = 2.0 + u2 - v2;
    double subtermy = 2.0 - u2 + v2;
    double termx1 = subtermx + u * twosqrt2;
    double termx2 = subtermx - u * twosqrt2;
    double termy1 = subtermy + v * twosqrt2;
    double termy2 = subtermy - v * twosqrt2;

    // x = ½ √( 2 + 2u√2 + u² - v² ) - ½ √( 2 - 2u√2 + u² - v² )
    float squareX = (float) (0.5 * std::sqrt(termx1) - 0.5 * std::sqrt(termx2));
    relX = (squareX + 1.0f) * 0.5f;

    // y = ½ √( 2 + 2v√2 - u² + v² ) - ½ √( 2 - 2v√2 - u² + v² )
    float squareY = (float) (0.5 * std::sqrt(termy1) - 0.5 * std::sqrt(termy2));
    relY = (squareY + 1.0f) * 0.5f;

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
