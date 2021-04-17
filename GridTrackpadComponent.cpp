#include "GridTrackpadComponent.h"

GridTrackpadComponent::GridTrackpadComponent(
    int size_,
    int inset_,
    int fps_,
    float radius_,
    float gridSize_,
    float maxPointSize_
) : TrackpadComponent(size_, inset_, fps_),
    radius(radius_),
    gridSize(gridSize_),
    maxPointSize(maxPointSize_)
{
    setSize(size_, size_);
    setFramesPerSecond (fps_);

    position = Point<float>(size_ * 0.5f, size_ * 0.5f);
    target = Point<float>(size_ * 0.5f, size_ * 0.5f);
}

void GridTrackpadComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

    for (int i = 0; i <= (float) getHeight() / gridSize; i++) {
        int centerY = (int) (i * gridSize);

        for (int j = 0; j <= (float) getWidth() / gridSize; j++) {
            int centerX = (int) (j * gridSize);

            if (inset > 0) {
                if (centerX < (inset * 0.5) || centerX > getWidth() - (inset * 0.5)) continue;
                if (centerY < (inset * 0.5) || centerY > getHeight() - (inset * 0.5)) continue;
            }

            double distance = Point<float>::distance(centerX, centerY, position.x(), position.y());
            double ratio = distance / radius;

            int posX = centerX;
            int posY = centerY;

            if (ratio <= 1) {
                posX = posX - ((int) ((position.x() - posX) * (1.0 - ratio) * 0.5));
                posY = posY - ((int) ((position.y() - posY) * (1.0 - ratio) * 0.5));
            }

            // Scale individual point size between 2 and maxPointSize to introduce scaling effect.
            float pointSize = std::max(2.0f, maxPointSize * (1.0f - std::min(1.0f, (float) ratio)));
            g.fillEllipse(posX, posY, pointSize, pointSize);
        }
    }
}

void GridTrackpadComponent::update() {
    // Basic cursor tweening
    position.x(position.x() + (target.x() - position.x()) * 0.2f);
    position.y(position.y() + (target.y() - position.y()) * 0.2f);
}

void GridTrackpadComponent::resized() {}
