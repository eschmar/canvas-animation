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
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);

    // float variance = 240.0f;
    int verticalOffset = 24;
    int horizontalStepOffset = 30;
    float variance = 4.0f * horizontalStepOffset;
    int radius = 96;

    // float posY = y < variance ? variance : y;
    int height = getHeight();
    int width = getWidth();


    float halfInset = 0.5f * inset;



    for (int i = 0; i < (float) height / verticalOffset; i++) {
        float currentY = halfInset + i * verticalOffset;
        if (currentY > height - halfInset) break;

        // printf(">> currentY: %.2f\n", currentY);
        // if (currentY < variance * 1.5f) continue;

        juce::Path wave;
        wave.startNewSubPath(juce::Point<float>(halfInset, currentY));

        if (std::abs(y - currentY) > radius) {
            g.setColour(juce::Colours::white);
            wave.lineTo(width - halfInset, currentY);
            g.strokePath(wave, juce::PathStrokeType(2.0f));

            wave.lineTo(width - halfInset, height - halfInset);
            wave.lineTo(halfInset, height - halfInset);
            wave.closeSubPath();

            g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
            g.fillPath(wave);
            continue;
        }
        

        g.setColour(juce::Colours::yellow);


        for (int j = 1; j < (width - halfInset) / horizontalStepOffset; j++) {
            float currentX = halfInset + j * horizontalStepOffset + horizontalStepOffset * 0.5f;


            float distance = euclideanDistance(currentX, currentY, x, y);
            float ratio = distance / radius;

            float bezierX1 = (j - 1) * horizontalStepOffset + 0.25f * horizontalStepOffset;
            float bezierX2 = j * horizontalStepOffset - 0.25f * horizontalStepOffset;

            if (ratio > 1) {
                wave.cubicTo(bezierX1, currentY - 2.0f, bezierX2, currentY + 8.0f, j * horizontalStepOffset, currentY);
                continue;
            }

            double shift = juce::Random::getSystemRandom().nextFloat() * variance;
            if (juce::Random::getSystemRandom().nextBool()) shift *= -1.0f;
            wave.cubicTo(bezierX1, currentY, bezierX2, currentY + shift, j * horizontalStepOffset, currentY + shift);




        }

        g.strokePath(wave, juce::PathStrokeType(2.0f));
        wave.lineTo(width - halfInset, height - halfInset);
        wave.lineTo(halfInset, height - halfInset);
        wave.closeSubPath();

        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        g.fillPath(wave);


        /* if (true || std::abs(currentY - posY) > radius || currentY < posY + verticalOffset) {
            wave.lineTo(width - halfInset, currentY);
            // wave.lineTo(width - inset * 0.5f, 0);
            // wave.lineTo(inset * 0.5f, 0);
            // wave.closeSubPath();
            g.strokePath(wave, juce::PathStrokeType(2.0f));
            // canvas.drawPath(wave, strokePaint);
            // canvas.drawPath(wave, fillPaint);
            continue;
        } */

        // juce::Path path;
        // path.startNewSubPath (juce::Point<float> (10, 10));
        // path.lineTo (juce::Point<float> (50, 10));
        // path.lineTo (juce::Point<float> (50, 50));
        // path.lineTo (juce::Point<float> (10, 50));
        // path.closeSubPath();


    }
}

// TODO: create a callback from mouseDrag to update `target`

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
