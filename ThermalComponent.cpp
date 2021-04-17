#include "ThermalComponent.h"

ThermalComponent::ThermalComponent(
    int size_,
    int inset_,
    int fps_,
    float stepSize_,
    float blobSize_,
    juce::Colour gradientFrom_,
    juce::Colour gradientTo_
) : TrackpadComponent(size_, inset_, fps_), position(size_ * 0.5f, size_ * 0.5f), target(size_ * 0.5f, size_ * 0.5f) {
    setSize(size_, size_);
    setFramesPerSecond(fps_);
    fps = fps_;

    stepSize = stepSize_;
    blobSize = blobSize_;
    gradientFrom = gradientFrom_;
    gradientTo = gradientTo_;

    blobCount = (size_t) ((getWidth() - inset - blobSize) / stepSize);

    blobRadius.resize((size_t) verticeCount + 1);
    blobRadiusTarget.resize((size_t) verticeCount + 1);
    epsilon = 0.8f;

    wobbler = (float) 0.0f;
    computeTarget(true);
}

/**
 * Calculate distance to the point of intersection for two tangents, making
 * use of the fact that the first intersection will be on (center.x() + radius).
 */
float ThermalComponent::calculateBezierDistance(float radius) {
    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / blobCount);

    float vx = std::cos(theta);
    float vy = std::sin(theta);
    float px = position.x() + radius * vx;
    float py = position.x() + radius * vy;

    float t = (position.x() + radius - px) / vy;
    float s = py - t * vx;

    return Point<float>::distance(px, py, position.x() + radius, s);
}

void ThermalComponent::paint(juce::Graphics& g) {
    juce::Colour baseColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.fillAll(gradientFrom);

    float roundness = 1.1f;
    size_t pointCount = blobCount;
    float halfSize = size * 0.5f;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / verticeCount);

    for (size_t i = pointCount - 1; i < pointCount; --i) {
        juce::Path blob;

        float percent = (float) i / (pointCount - 1);
        float baseRadius = blobSize + (stepSize * i);
        float radius = baseRadius + (baseRadius * epsilon * (1.0f - percent) * (blobRadius[0] - 0.5f) * 2);
        float bezierDistance = calculateBezierDistance(radius) * roundness;

        // Add perspective to blobs by offsetting the position slightly
        float distance = position.distance(Point<float>(halfSize, halfSize));
        float beta = std::atan((position.y() - halfSize) / (position.x() - halfSize));
        if (position.x() < halfSize) beta += (float) M_PI; // Adjust theta depending on quadrant

        float offset = i * stepSize * 3 * (distance / (size - inset) * 0.5f); // The farther from origin the more offset

        Point<float> localCenter = Point<float>(
            halfSize + (distance - offset) * std::cos(beta),
            halfSize + (distance - offset) * std::sin(beta)
        );

        // Avoid divide by zero, which will break tweening
        if (distance == 0) localCenter = Point<float>(position);

        // Initiate previous point to calculate bezier points
        float prevX = localCenter.x() + radius * (float) std::cos(0 + wobbler);
        float prevY = localCenter.y() + radius * (float) std::sin(0 + wobbler);

        // Start drawin blob at first point
        blob.startNewSubPath(juce::Point<float>(prevX, prevY));

        for (size_t j = 1; j < blobRadius.size(); j++) {
            radius = baseRadius + (baseRadius * epsilon * (1.0f - percent) * (blobRadius[j] - 0.5f) * 2);

            // polar to cartesian vector
            float vx = std::cos(theta * j + wobbler);
            float vy = std::sin(theta * j + wobbler);

            // next point position
            float px = localCenter.x() + radius * vx;
            float py = localCenter.y() + radius * vy;

            // Bezier points using perpendicular vector [-vy, vx].
            // Calculate bezier points for the previous point.
            float bezierX1 = prevX - std::sin(theta * (j - 1) + wobbler) * bezierDistance;
            float bezierY1 = prevY - (-1.0f * std::cos(theta * (j - 1) + wobbler)) * bezierDistance;

            // Calculate bezier points for the target point.
            float bezierX2 = px + vy * bezierDistance;
            float bezierY2 = py + (-1.0f * vx) * bezierDistance;

            blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, px, py);

            prevX = px;
            prevY = py;
        }

        g.setColour(juce::Colour(
            (u_int8_t) (gradientFrom.getRed() * percent + gradientTo.getRed() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getGreen() * percent + gradientTo.getGreen() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getBlue() * percent + gradientTo.getBlue() * (1.0f - percent)),
            (u_int8_t) 128
        ));

        // The most inner (last) blob should be special colour
        if (i == 0) g.setColour(baseColor);
        g.fillPath(blob);
    }

    // Create illusion of a hole
    float halfInset = inset * 0.5f;
    // float ringStrokeWidth = 2;
    float ringOffset = 8;

    juce::Path hole;
    hole.addRectangle(juce::Rectangle<int>(0, 0, getWidth(), getHeight()));
    hole.setUsingNonZeroWinding(false);

    // hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset, halfInset - ringOffset, getWidth() - inset + ringOffset * 2, getHeight() - inset + ringOffset * 2));
    // hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset + ringStrokeWidth, halfInset - ringOffset + ringStrokeWidth, getWidth() - inset + ringOffset * 2 - ringStrokeWidth * 2, getHeight() - inset + ringOffset * 2 - ringStrokeWidth * 2));
    hole.addEllipse(juce::Rectangle<float>(halfInset, halfInset, getWidth() - inset, getHeight() - inset));

    g.setColour(baseColor);
    g.fillPath(hole);

    // Simple one-coloured outer ring
    g.setColour(juce::Colour(
        (u_int8_t) (gradientFrom.getRed() * 0.5f + gradientTo.getRed() * 0.5f),
        (u_int8_t) (gradientFrom.getGreen() * 0.5f + gradientTo.getGreen() * 0.5f),
        (u_int8_t) (gradientFrom.getBlue() * 0.5f + gradientTo.getBlue() * 0.5f)
    ));

    g.drawEllipse(
        halfInset - ringOffset,
        halfInset - ringOffset,
        getWidth() - inset + ringOffset * 2,
        getHeight() - inset + ringOffset * 2,
        3.0
    );

    // Cursor
    // g.setColour(gradientFrom);
    // g.drawLine(juce::Line<float>(target.x() - 10.0f, target.y(), target.x() + 10.0f, target.y()));
    // g.drawLine(juce::Line<float>(target.x(), target.y() - 10.0f, target.x(), target.y() + 10.0f));
}

void ThermalComponent::mouseDrag(const juce::MouseEvent& event) {
    float halfSize = size * 0.5f;
    float radius = (size - inset) / 2.0f;

    // Limit movement to circle
    if (Point<float>::distance(event.x, event.y, halfSize, halfSize) > radius) {
        // Project Point onto ellipse, adjust theta depending on quadrant
        float theta = std::atan((event.y - halfSize) / (event.x - halfSize));
        if (event.x < halfSize) theta += (float) M_PI;

        target.x(halfSize + radius * std::cos(theta));
        target.y(halfSize + radius * std::sin(theta));
    } else {
        target.x(event.x);
        target.y(event.y);
    }

    // map coordinates to unit circle
    // http://squircular.blogspot.com/2015/09/mapping-circle-to-square.html
    float u = (float) (x - halfSize) / radius;
    float v = (float) (y - halfSize) / radius;

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
}

void ThermalComponent::computeTarget(bool fastforward) {
    // Periodically randomise blob radius slightly every ~0.8s.
    int frameTrigger = (int) (0.8 / (1.0 / (double) fps));
    bool shouldRandomise = fmod(getFrameCounter(), frameTrigger) == 0;

    if (shouldRandomise) {
        for (size_t i = 0; i < blobRadiusTarget.size(); i++) {
            blobRadiusTarget[i] = juce::Random::getSystemRandom().nextFloat();
            // if (i % 2 == 0) blobRadiusTarget[i] = 1.0f - 0.2f * blobRadiusTarget[i];
            if (i == blobRadiusTarget.size() - 1) blobRadiusTarget[i] = blobRadiusTarget[0];
            if (fastforward) blobRadius[i] = blobRadiusTarget[i];
        }
    }

    // Add slow rotation.
    wobbler += 0.002f;
}

void ThermalComponent::update() {
    // Basic cursor tweening
    position.x(position.x() + (target.x() - position.x()) * 0.1f);
    position.y(position.y() + (target.y() - position.y()) * 0.1f);

    // Tween epsilon for local radius of blob points
    for (size_t i = 0; i < blobRadius.size(); i++) {
        blobRadius[i] = blobRadius[i] + (blobRadiusTarget[i] - blobRadius[i]) * 0.01f;
    }

    computeTarget();
}

void ThermalComponent::resized() {}
