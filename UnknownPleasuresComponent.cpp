#include "UnknownPleasuresComponent.h"

UnknownPleasuresComponent::UnknownPleasuresComponent(
    int size_,
    int inset_,
    int fps_,
    int verticalOffset_,
    int horizontalStepOffset_,
    int radius_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
    verticalOffset = verticalOffset_;
    horizontalStepOffset = horizontalStepOffset_;
    radius = radius_;
}

void UnknownPleasuresComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

    float variance = 6.0f * horizontalStepOffset;
    float waveyOffset = verticalOffset * 0.2f;
    float halfInset = 0.5f * inset;

    int height = getHeight();
    int width = getWidth();

    for (int i = 0; i < (float) height / verticalOffset; i++) {
        float currentY = halfInset + i * verticalOffset;
        if (currentY > height - halfInset) break;

        juce::Path wave;
        g.setColour(juce::Colours::white);

        float prevLocalY = currentY;

        for (int j = 0; j <= (width - inset) / horizontalStepOffset; j++) {
            float currentX = halfInset + j * horizontalStepOffset;

            double distance = euclideanDistance(currentX, currentY, (float) x, (float) y);
            // float distance = std::abs(x - currentX); // Joy Division example

            float ratio = (float) distance / radius;

            // what's the current amplitude?
            float shift = 0.0f;
            if (ratio < 1.0f) shift = juce::Random::getSystemRandom().nextFloat() * variance * (1.0f - ratio);

            // attemp sort of wavey line
            shift += (juce::Random::getSystemRandom().nextFloat() * waveyOffset) - waveyOffset * 0.5f;

            // if this is the first point, start the path
            if (j == 0) {
                wave.startNewSubPath(juce::Point<float>(halfInset, currentY - shift));
                continue;
            }

            // bezier
            float bezierX1 = currentX - 0.8f * horizontalStepOffset;
            float bezierX2 = currentX - 0.2f * horizontalStepOffset;
            // float bezierX = currentX - 0.5f * horizontalStepOffset;

            wave.cubicTo(bezierX1, prevLocalY, bezierX2, currentY - shift, currentX, currentY - shift);
            prevLocalY = currentY - shift;
        }

        g.strokePath(wave, juce::PathStrokeType(3.0f));
        wave.lineTo(width - halfInset, height - halfInset);
        wave.lineTo(halfInset, height - halfInset);
        wave.closeSubPath();

        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        g.fillPath(wave);
    }
}

void UnknownPleasuresComponent::mouseDrag(const juce::MouseEvent& event) {
    printf("X>>> <x,y>=<%d,%d>\n", event.x, event.y);
    TrackpadComponent::mouseDrag(event);
    printf("Y>>> <x',y'>=<%.2f,%.2f>\n", x, y);
    // Update `target`
}

void UnknownPleasuresComponent::update() {
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.

    // move `current` towards `target`
}

void UnknownPleasuresComponent::resized() {
    // This is called when the UnknownPleasuresComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
