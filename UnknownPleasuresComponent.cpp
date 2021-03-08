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

    int rowCount = (int) std::floor((float) (getHeight() - inset) / verticalOffset) + 1;
    int colCount = (getWidth() - inset) / horizontalStepOffset + 1;
    position.resize((size_t) rowCount, std::vector<float>((size_t) colCount));
    target.resize((size_t) rowCount, std::vector<float>((size_t) colCount));
    computeTarget(true);
}

void UnknownPleasuresComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

    float halfInset = 0.5f * inset;

    for (int i = 0; i < position.size(); i++) {
        juce::Path wave;
        g.setColour(juce::Colours::white);

        for (int j = 0; j < position[0].size(); j++) {
            if (j == 0) {
                wave.startNewSubPath(juce::Point<float>(halfInset, position[i][j]));
                continue;
            }

            // bezier
            float currentX = halfInset + j * horizontalStepOffset;
            float bezierX1 = currentX - 0.8f * horizontalStepOffset;
            float bezierX2 = currentX - 0.2f * horizontalStepOffset;

            wave.cubicTo(bezierX1, position[i][j - 1], bezierX2, position[i][j], currentX, position[i][j]);
        }

        g.strokePath(wave, juce::PathStrokeType(3.0f));

        // fill body below to create illusion of depth
        wave.lineTo(getWidth() - halfInset, getHeight());
        wave.lineTo(halfInset, getHeight());
        wave.closeSubPath();
        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        g.fillPath(wave);
    }
}

void UnknownPleasuresComponent::mouseDrag(const juce::MouseEvent& event) {
    // printf("X>>> <x,y>=<%d,%d>\n", event.x, event.y);
    TrackpadComponent::mouseDrag(event);
    // printf("Y>>> <x',y'>=<%.2f,%.2f>\n", x, y);

    computeTarget();
}

void UnknownPleasuresComponent::computeTarget(bool fastforward) {
    float variance = 6.0f * horizontalStepOffset;
    float waveyOffset = verticalOffset * 0.2f;
    float halfInset = 0.5f * inset;

    int height = getHeight();
    int width = getWidth();

    for (int i = 0; i < target.size(); i++) {
        float currentY = halfInset + i * verticalOffset;

        for (int j = 0; j < target[0].size(); j++) {
            float currentX = halfInset + j * horizontalStepOffset;

            double distance = euclideanDistance(currentX, currentY, (float) x, (float) y);
            // float distance = std::abs(x - currentX); // Joy Division example

            float ratio = (float) distance / radius;

            // what's the current amplitude?
            float shift = 0.0f;
            if (ratio < 1.0f) shift = juce::Random::getSystemRandom().nextFloat() * variance * (1.0f - ratio);

            // attemp sort of wavey line
            shift += (juce::Random::getSystemRandom().nextFloat() * waveyOffset) - waveyOffset * 0.5f;

            target[i][j] = currentY - shift;
            if (fastforward) position[i][j] = currentY - shift;
        }
    }
}

void UnknownPleasuresComponent::update() {
    // TODO: beat syncing

    // basic tweening
    for (int i = 0; i < position.size(); i++) {
        for (int j = 0; j < position[0].size(); j++) {
            position[i][j] += (target[i][j] - position[i][j]) * 0.1f;
        }
    }
}

void UnknownPleasuresComponent::resized() {
    // This is called when the UnknownPleasuresComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
