#include "ThermalComponent.h"

ThermalComponent::ThermalComponent(
    int size_,
    int inset_,
    int fps_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.65f;
    y = getHeight() * 0.65f;
    wobbler = (float) M_PI;
}

void ThermalComponent::paint(juce::Graphics& g) {
    juce::Colour baseColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);

    // Blobs
    float blobStepSize = 32.0f;
    float minBlobSize = 38.0f;
    float radius = getWidth() - inset;
    int i = 0;

    u_int8_t r1 = 255, g1 = 155, b1 = 0;
    u_int8_t r2 = 194, g2 = 32, b2 = 19;

    int iterations = (int) ((getWidth() - inset - minBlobSize) / blobStepSize);

    while (radius > minBlobSize) {
        // Round((R1*Percent + R2*(100-Percent))/100.0)
        float percent = (float) i++ / iterations;
        drawBlob(
            (float) x, // TODO: add perspective
            (float) y, // TODO: add perspective
            radius,
            juce::Colour(
                (u_int8_t) (r1 * percent + r2 * (1.0f - percent)),
                (u_int8_t) (g1 * percent + g2 * (1.0f - percent)),
                (u_int8_t) (b1 * percent + b2 * (1.0f - percent))
            ),
            g
        );

        radius -= blobStepSize;
        // blobStepSize *= 0.95f;
    }

    // Draw smallest size blob in base color
    drawBlob((float) x, (float) y, minBlobSize, baseColor, g);

    // Create illusion of a hole
    juce::Path hole;
    float halfInset = inset * 0.5f;

    hole.addRectangle(juce::Rectangle<int>(0, 0, getWidth(), getHeight()));
    hole.setUsingNonZeroWinding(false);
    hole.addEllipse(juce::Rectangle<float>(halfInset, halfInset, getWidth() - inset, getHeight() - inset));
    g.setColour(baseColor);
    g.fillPath(hole);

    // Draw a lil ring around the whole
    g.setColour(juce::Colour(r1, g1, b1));
    float ringOffset = 8;
    g.drawEllipse(juce::Rectangle<float>(halfInset - ringOffset, halfInset - ringOffset, getWidth() - inset + ringOffset * 2, getHeight() - inset + ringOffset * 2), 4.0f);

    // Draggable circle
    float cursorRadius = 16;
    g.setColour(juce::Colours::yellow);
    g.drawEllipse((float) x - cursorRadius, (float) y - cursorRadius, cursorRadius * 2, cursorRadius * 2, 3);
}

void ThermalComponent::drawBlob(float centerX, float centerY, float radius, juce::Colour colour, juce::Graphics& g) {
    float offsetX = centerX + 20.0f * std::sin(wobbler);
    float offsetY = centerY + 15.0f * std::cos(wobbler * 2);

    float circ = 4.0f * (std::sqrt(2.0f) - 1.0f) / 3.0f;
    float c = 0.0f;

    juce::Path wave;

    // top right
    c = circ + (0.2f * std::sin(wobbler));
    wave.startNewSubPath(juce::Point<float>(offsetX + 0, offsetY - radius));
    wave.cubicTo(
        offsetX + c * radius, offsetY - radius,
        offsetX + radius, offsetY - c * radius,
        offsetX + radius, offsetY + 0
    );

    // g.setColour(juce::Colours::white);
    // g.strokePath(wave, juce::PathStrokeType(12.0f));

    // bottom right
    c = circ + (0.2f * std::cos(wobbler));
    wave.cubicTo(
        offsetX + radius, offsetY + c * radius,
        offsetX + c * radius, offsetY + radius,
        offsetX + 0, offsetY + radius
    );

    // g.setColour(juce::Colours::blue);
    // g.strokePath(wave, juce::PathStrokeType(8.0f));

    // bottom left
    c = circ + (0.2f * std::sin(wobbler * 2));
    wave.cubicTo(
        offsetX - c * radius, offsetY + radius,
        offsetX - radius, offsetY + c * radius,
        offsetX - radius, offsetY + 0
    );

    // g.setColour(juce::Colours::green);
    // g.strokePath(wave, juce::PathStrokeType(4.0f));

    // top left
    c = circ + (0.2f * std::cos(wobbler + 1));
    wave.cubicTo(
        offsetX - radius, offsetY - c * radius,
        offsetX - c * radius, offsetY - radius,
        offsetX + 0, offsetY - radius
    );

    // g.setColour(juce::Colours::red);
    // g.strokePath(wave, juce::PathStrokeType(1.0f));

    g.setColour(colour);
    g.fillPath(wave);

    wobbler = (float) std::fmod(wobbler + 0.005, M_PI * 2.0f);
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
    // http://squircular.blogspot.com/2015/09/mapping-circle-to-square.html
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
