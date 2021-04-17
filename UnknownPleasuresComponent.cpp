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
    position = Point<float>(size_ * 0.5f, size_ * 0.5f);
    target = Point<float>(size_ * 0.5f, size_ * 0.5f);
    verticalOffset = verticalOffset_;
    horizontalStepOffset = horizontalStepOffset_;
    radius = radius_;

    int rowCount = (int) std::floor((float) (getHeight() - inset) / verticalOffset) + 1;
    int colCount = (getWidth() - inset) / horizontalStepOffset + 1;
    linesPosition.resize((size_t) rowCount, std::vector<float>((size_t) colCount));
    linesTarget.resize((size_t) rowCount, std::vector<float>((size_t) colCount));
    computeTarget(true);

    start = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void UnknownPleasuresComponent::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

    float halfInset = 0.5f * inset;
    for (size_t i = 0; i < linesPosition.size(); i++) {
        juce::Path wave;
        g.setColour(juce::Colours::white);

        for (size_t j = 0; j < linesPosition[0].size(); j++) {
            if (j == 0) {
                wave.startNewSubPath(juce::Point<float>(halfInset, linesPosition[i][j]));
                continue;
            }

            // bezier
            float currentX = halfInset + (float) j * horizontalStepOffset;
            float bezierX1 = currentX - 0.8f * horizontalStepOffset;
            float bezierX2 = currentX - 0.2f * horizontalStepOffset;

            wave.cubicTo(bezierX1, linesPosition[i][j - 1], bezierX2, linesPosition[i][j], currentX, linesPosition[i][j]);
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
    TrackpadComponent::mouseDrag(event);
    computeTarget();
}

void UnknownPleasuresComponent::computeTarget(bool fastforward) {
    float variance = 6.0f * horizontalStepOffset;
    float waveyOffset = verticalOffset * 0.2f;
    float halfInset = 0.5f * inset;

    for (size_t i = 0; i < linesTarget.size(); i++) {
        float currentY = halfInset + (float) i * verticalOffset;

        for (size_t j = 0; j < linesTarget[0].size(); j++) {
            float currentX = halfInset + (float) j * horizontalStepOffset;

            double distance = Point<float>::distance(currentX, currentY, target.x(), target.y());
            // float distance = std::abs(x - currentX); // Joy Division example

            float ratio = (float) distance / radius;

            // what's the current amplitude?
            float shift = 0.0f;
            if (ratio < 1.0f) shift = juce::Random::getSystemRandom().nextFloat() * variance * (1.0f - ratio);

            // attemp sort of wavey line
            shift += (juce::Random::getSystemRandom().nextFloat() * waveyOffset) - waveyOffset * 0.5f;

            linesTarget[i][j] = currentY - shift;
            if (fastforward) linesPosition[i][j] = currentY - shift;
        }
    }
}

void UnknownPleasuresComponent::update() {
    // TODO: replace fake beat syncing with the proper thing
    uint64_t beatLength = 500;
    uint64_t now = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() - (uint64_t) 50;

    uint64_t timeSinceBeat = now % beatLength;

    if (timeSinceBeat > beatLength - 25) {
        computeTarget();
    }

    // basic tweening
    for (size_t i = 0; i < linesPosition.size(); i++) {
        for (size_t j = 0; j < linesPosition[0].size(); j++) {
            linesPosition[i][j] += (linesTarget[i][j] - linesPosition[i][j]) * 0.1f;
        }
    }
}

void UnknownPleasuresComponent::resized() {
    // This is called when the UnknownPleasuresComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
