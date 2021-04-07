#include "PolarComponent.h"

PolarComponent::PolarComponent(
    int size_,
    int inset_,
    int fps_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond(144);

    position.x(size * 0.5f);
    position.y(size * 0.5f);
    target.x(size * 0.5f);
    target.y(size * 0.5f);
}

void PolarComponent::paint(juce::Graphics& g) {
    float halfSize = size * 0.5f;
    float halfInset = inset * 0.5f;
    juce::Colour baseColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.fillAll(baseColor);

    // Boreder ellipse
    g.setColour(juce::Colour(0xaae8eb34));
    g.drawEllipse(
        halfInset,
        halfInset,
        getWidth() - inset,
        getHeight() - inset,
        3.0
    );

    // Cursor
    g.setColour(juce::Colour(0xffe8eb34));
    g.drawLine(juce::Line<float>(position.x() - 10.0f, position.y(), position.x() + 10.0f, position.y()));
    g.drawLine(juce::Line<float>(position.x(), position.y() - 10.0f, position.x(), position.y() + 10.0f));

    // Project Point onto ellipse
    float radius = (size - inset) / 2.0;
    float theta = std::atan((position.y() - halfSize) / (position.x() - halfSize));

    // Adjust theta depending on quadrant
    if (position.x() < halfSize) theta += M_PI;

    Point<float> projection = Point<float>(
        halfSize + radius * std::cos(theta),
        halfSize + radius * std::sin(theta)
    );

    g.setColour(juce::Colour(0xfffc0f5e));
    g.drawLine(juce::Line<float>(projection.x() - 10.0f, projection.y(), projection.x() + 10.0f, projection.y()));
    g.drawLine(juce::Line<float>(projection.x(), projection.y() - 10.0f, projection.x(), projection.y() + 10.0f));

    // Ray from origin
    g.setColour(juce::Colour(0x77fc0f5e));
    g.drawLine(juce::Line<float>(halfSize, halfSize, projection.x(), projection.y()), 3.0);
}

void PolarComponent::mouseDrag(const juce::MouseEvent& event) {
    float radius = (size - inset) / 2.0f;
    float center = size * 0.5f;
    float distance = (float) std::sqrt(std::pow(event.x - center, 2) + std::pow(event.y - center, 2));

    // Limit movement to circle
    /* if (float ratio = distance / radius; ratio > 1.0) {
        float deltaX = event.x - center;
        float deltaY = event.y - center;

        float length = (float) std::sqrt(std::pow(deltaX, 2.0) + std::pow(deltaY, 2.0));
        target.x(center + (deltaX / length) * radius);
        target.y(center + (deltaY / length) * radius);
        return;
    } */

    target.x(event.x);
    target.y(event.y);
}

void PolarComponent::update() {
    // basic cursor tweening
    position.x(position.x() + (target.x() - position.x()) * 0.1f);
    position.y(position.y() + (target.y() - position.y()) * 0.1f);
}

void PolarComponent::resized() {}
