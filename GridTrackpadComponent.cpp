#include "GridTrackpadComponent.h"

GridTrackpadComponent::GridTrackpadComponent(
    int size_,
    int inset_,
    int fps_,
    float radius_,
    float gridSize_,
    float maxPointSize_
) : TrackpadComponent(size_, inset_, fps_), radius(radius_), gridSize(gridSize_), maxPointSize(maxPointSize_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
}

void GridTrackpadComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::yellow);

    for (int i = 0; i <= (float) getHeight() / gridSize; i++) {
        int centerY = (int) (i * gridSize);

        for (int j = 0; j <= (float) getWidth() / gridSize; j++) {
            int centerX = (int) (j * gridSize);

            if (inset > 0) {
                if (centerX < (inset * 0.5) || centerX > getWidth() - (inset * 0.5)) continue;
                if (centerY < (inset * 0.5) || centerY > getHeight() - (inset * 0.5)) continue;
            }

            double distance = euclideanDistance(centerX, x, centerY, y);
            double ratio = distance / radius;

            int posX = centerX;
            int posY = centerY;

            if (ratio <= 1) {
                posX = posX - ((int) ((x - posX) * (1 - ratio) * 0.5));
                posY = posY - ((int) ((y - posY) * (1 - ratio) * 0.5));
            }

            // Scale individual point size between 2 and maxPointSize to introduce scaling effect.
            float pointSize = std::max(2.0f, maxPointSize * (1.0f - std::min(1.0f, (float) ratio)));
            g.fillEllipse(posX, posY, pointSize, pointSize);
        }
    }
}

void GridTrackpadComponent::update() {
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

void GridTrackpadComponent::resized() {
    // This is called when the GridTrackpadComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
