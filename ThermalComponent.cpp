#include "ThermalComponent.h"

ThermalComponent::ThermalComponent(
    int size_,
    int inset_,
    int fps_,
    float stepSize_,
    float blobSize_,
    juce::Colour gradientFrom_,
    juce::Colour gradientTo_
) : TrackpadComponent(size_, inset_, fps_) {
    setSize(size_, size_);
    setFramesPerSecond(12);

    stepSize = stepSize_;
    blobSize = blobSize_;
    gradientFrom = gradientFrom_;
    gradientTo = gradientTo_;

    coordinateX.resize(2);
    coordinateX[0] = getWidth() * 0.5f;
    coordinateX[1] = getWidth() * 0.5f;

    coordinateY.resize(2);
    coordinateY[0] = getHeight() * 0.5f;
    coordinateY[1] = getHeight() * 0.5f;

    wobbler = (float) 0.0f;
}

void ThermalComponent::paint(juce::Graphics& g) {
    // u_int8_t r1 = gradientFrom.getRed(), g1 = gradientFrom.getGreen(), b1 = gradientFrom.getBlue();
    // u_int8_t r2 = gradientTo.getRed(), g2 = gradientTo.getGreen(), b2 = gradientTo.getBlue();

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    drawBlob2(coordinateX[1], coordinateY[1], 196.0f, gradientTo, g);

    juce::Path blobbed = generateBlob(coordinateX[1], coordinateY[1], 188.0f);

    g.setColour(juce::Colours::blueviolet);
    g.fillPath(blobbed);

    // (float centerX, float centerY, float radius, size_t pointCount, float roundness)

    // float halfSize = size * 0.5f;
    // float distance = (float) euclideanDistance(halfSize, halfSize, coordinateX[0], coordinateY[0]);
    // float ratio = distance / ((size - inset) * 0.5f);

    // // Fill canvas in case blobs don't cover everything
    // g.fillAll(gradientTo);

    // // Draw stacked blobs, start with a radius that will fill the active zone
    // size_t blobCount = (size_t) ((getWidth() - inset - blobSize) / stepSize);

    // for (size_t i = 0; i < blobCount; i++) {
    //     float percent = (float) i / blobCount;
    //     float radius = blobSize + (blobCount - i) * stepSize;

    //     // Add perspective to blobs by offsetting the position slightly
    //     float persp = stepSize * (1.0f - percent) * ratio * 0.8f;
    //     float perspX = ((coordinateX[0] - halfSize) / distance) * (distance - persp * i);
    //     float perspY = ((coordinateY[0] - halfSize) / distance) * (distance - persp * i);

    //     drawBlob(
    //         (float) halfSize + perspX,
    //         (float) halfSize + perspY,
    //         radius,
    //         juce::Colour(
    //             (u_int8_t) (r1 * percent + r2 * (1.0f - percent)),
    //             (u_int8_t) (g1 * percent + g2 * (1.0f - percent)),
    //             (u_int8_t) (b1 * percent + b2 * (1.0f - percent))
    //         ),
    //         g
    //     );
    // }

    // // Draw smallest size blob in base color
    // juce::Colour baseColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    // drawBlob(coordinateX[0], coordinateY[0], blobSize, baseColor, g);

    // // Create illusion of a hole
    // juce::Path hole;
    // float halfInset = inset * 0.5f;

    // hole.addRectangle(juce::Rectangle<int>(0, 0, getWidth(), getHeight()));
    // hole.setUsingNonZeroWinding(false);

    // float ringOffset = 8;
    // float ringStrokeWidth = 2;

    // hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset, halfInset - ringOffset, getWidth() - inset + ringOffset * 2, getHeight() - inset + ringOffset * 2));
    // hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset + ringStrokeWidth, halfInset - ringOffset + ringStrokeWidth, getWidth() - inset + ringOffset * 2 - ringStrokeWidth * 2, getHeight() - inset + ringOffset * 2 - ringStrokeWidth * 2));
    // hole.addEllipse(juce::Rectangle<float>(halfInset, halfInset, getWidth() - inset, getHeight() - inset));

    // g.setColour(baseColor);
    // g.fillPath(hole);

    // Cursor
    // g.setColour(gradientFrom);
    // g.drawLine(juce::Line<float>(coordinateX[1] - 10.0f, coordinateY[1], coordinateX[1] + 10.0f, coordinateY[1]));
    // g.drawLine(juce::Line<float>(coordinateX[1], coordinateY[1] - 10.0f, coordinateX[1], coordinateY[1] + 10.0f));
}

juce::Path ThermalComponent::generateBlob(float centerX, float centerY, float radius, size_t pointCount, float roundness, bool wobbling) {
    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / pointCount);

    // TODO
    float perpDist = 128.0;

    // Holds the points of which the blobs consists.
    // std::vector<float[2]> points(pointCount + 1);
    // std::vector<float[2]> vectors(pointCount + 1);
    float points[pointCount + 1][2];
    float vectors[pointCount + 1][2];

    // Evenly lay out points on the circle.
    for (size_t i = 0; i < pointCount; i++) {
        // Add slow rotation
        if (wobbling) wobbler += 0.01f;

        vectors[i][0] = std::cos(theta * i + wobbler);
        vectors[i][1] = std::sin(theta * i + wobbler);

        points[i][0] = centerX + radius * vectors[i][0];
        points[i][1] = centerY + radius * vectors[i][1];
    }

    // Close the path by adding the first point at the end again.
    vectors[pointCount][0] = vectors[0][0];
    vectors[pointCount][1] = vectors[0][1];
    points[pointCount][0] = points[0][0];
    points[pointCount][1] = points[0][1];

    // Start the bezier path.
    juce::Path blob;
    blob.startNewSubPath(juce::Point<float>(points[0][0], points[0][1]));

    // Draw bezier curves through points, skip first point.
    for (size_t i = 1; i < pointCount + 1; i++) {
        // Tangent vector for [x,y] is [y, -x]
        // Calculate bezier points for the previous point.
        float bezierX1 = points[i - 1][0] - vectors[i - 1][1] * perpDist;
        float bezierY1 = points[i - 1][1] - (-1.0f * vectors[i - 1][0]) * perpDist;

        // Calculate bezier points for the target point.
        float bezierX2 = points[i][0] + vectors[i][1] * perpDist;
        float bezierY2 = points[i][1] + (-1.0f * vectors[i][0]) * perpDist;

        // blob.lineTo(pointX, pointY);
        blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, points[i][0], points[i][1]);
    }

    return blob;
}

void ThermalComponent::drawBlob2(float centerX, float centerY, float radius, juce::Colour colour, juce::Graphics& g) {
    g.setColour(gradientFrom);
    g.drawEllipse(juce::Rectangle<float>(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f), 1.0f);

    // evenly distribute points around circle
    size_t totalPoints = 3;
    float theta = (float) (M_PI * 2.0 / totalPoints);
    g.setColour(juce::Colours::white);

    juce::Path blob;
    float pr = 3;
    float perpDist = 128.0;
    float bezierX1 = 0.0f, bezierY1 = 0.0f;
    float prevX = 0.0f, prevY = 0.0f;

    // float prevX, prevY, prevVectorX, prevVectorY;
    for (size_t i = 0; i < totalPoints; i++) {
        float vectorX = std::cos(theta * i + wobbler);
        float vectorY = std::sin(theta * i + wobbler);
        float pointX = centerX + radius * vectorX;
        float pointY = centerY + radius * vectorY;

        // Add slow rotation
        wobbler += 0.01f;

        // show point coordinates
        g.setColour(juce::Colours::white);
        g.fillEllipse(juce::Rectangle<float>(pointX - pr, pointY - pr, pr * 2.0f, pr * 2.0f));

        float perpX = vectorY;
        float perpY = -1.0f * vectorX;

        if (i == 0) {
            blob.startNewSubPath(juce::Point<float>(pointX, pointY));
        } else {
            float bezierX2, bezierY2;

            // calculate bezier points for the target point.
            bezierX2 = pointX + perpX * perpDist;
            bezierY2 = pointY + perpY * perpDist;

            // draw bezier curve points for the second point
            if (i == 1) {
                g.setColour(juce::Colours::cyan);
                g.fillEllipse(juce::Rectangle<float>(bezierX1 - pr, bezierY1 - pr, pr * 2.0f, pr * 2.0f));
                g.drawLine(juce::Line<float>(prevX, prevY, bezierX1, bezierY1));

                g.setColour(juce::Colours::teal);
                g.fillEllipse(juce::Rectangle<float>(bezierX2 - pr, bezierY2 - pr, pr * 2.0f, pr * 2.0f));
                g.drawLine(juce::Line<float>(pointX, pointY, bezierX2, bezierY2));
            }

            // blob.lineTo(pointX, pointY);
            blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, pointX, pointY);
        }

        bezierX1 = pointX - perpX * perpDist;
        bezierY1 = pointY - perpY * perpDist;

        prevX = pointX;
        prevY = pointY;
    }

    // by the unit circle it's trivial to determine the bezier points for the starting condition.
    blob.cubicTo(bezierX1, bezierY1, centerX + radius, centerY - perpDist, centerX + radius, centerY);

    g.setColour(gradientTo);
    g.strokePath(blob, juce::PathStrokeType(2.0f));
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

    wobbler = (float) std::fmod(wobbler + 0.001, M_PI * 2.0f);
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
        coordinateX[1] = center + (deltaX / length) * radius;
        coordinateY[1] = center + (deltaY / length) * radius;

    } else {
        coordinateX[1] = event.x;
        coordinateY[1] = event.y;
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
    // basic tweening
    coordinateX[0] += (coordinateX[1] - coordinateX[0]) * 0.1f;
    coordinateY[0] += (coordinateY[1] - coordinateY[0]) * 0.1f;
}

void ThermalComponent::resized() {
    // This is called when the ThermalComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
