#include "ThermalComponent.h"

ThermalComponent::ThermalComponent(
    int size_,
    int inset_,
    int fps_,
    float stepSize_,
    float blobSize_,
    juce::Colour gradientFrom_,
    juce::Colour gradientTo_
) : TrackpadComponent(size_, inset_, fps_), current(size_ * 0.5f, size_ * 0.5f), target(size_ * 0.5f, size_ * 0.5f) {
    setSize(size_, size_);
    setFramesPerSecond(24);

    stepSize = stepSize_;
    blobSize = blobSize_;
    gradientFrom = gradientFrom_;
    gradientTo = gradientTo_;

    size_t blobCount = (size_t) ((getWidth() - inset - blobSize) / stepSize);
    blobs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) 6 + 1));
    vecs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) 6 + 1));

    wobbler = (float) 0.0f;
    rotator = (float) 0.0f;
}

void ThermalComponent::paint(juce::Graphics& g) {
    // u_int8_t r1 = gradientFrom.getRed(), g1 = gradientFrom.getGreen(), b1 = gradientFrom.getBlue();
    // u_int8_t r2 = gradientTo.getRed(), g2 = gradientTo.getGreen(), b2 = gradientTo.getBlue();

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // juce::Path triangle = generateBlob(g, coordinateX[1], coordinateY[1], 128.0f, 8, 0.6f);
    // g.setColour(juce::Colour(0x33aa1100));
    // g.fillPath(triangle);

    juce::Path hexagon = generateBlob(g, current, 128.0f, 6, 1.1f);
    g.setColour(juce::Colour(0x330011aa));
    g.fillPath(hexagon);

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

juce::Path ThermalComponent::generateBlob(juce::Graphics& g, Point<float>& center, float radius, size_t pointCount, float roundness, bool wobbling) {
    float pointRadius = 3.0f;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / pointCount);

    // Holds the points of which the blobs consists.
    std::vector<Point<float>> points(pointCount + 1);
    std::vector<Point<float>> vectors(pointCount + 1);

    // Evenly lay out points on the circle.
    for (size_t i = 0; i < pointCount; i++) {
        // Add slow rotation
        if (wobbling) wobbler += 0.005f;

        vectors[i].x(std::cos(theta * i + wobbler));
        vectors[i].y(std::sin(theta * i + wobbler));

        points[i].x(center.x() + radius * vectors[i].x());
        points[i].y(center.y() + radius * vectors[i].y());

        // show point coordinates
        g.setColour(juce::Colours::white);
        g.fillEllipse(juce::Rectangle<float>(points[i].x() - pointRadius, points[i].y() - pointRadius, pointRadius * 2.0f, pointRadius * 2.0f));
    }

    // Close the path by adding the first point at the end again.
    vectors[pointCount].x(vectors[0].x());
    vectors[pointCount].y(vectors[0].y());
    points[pointCount].x(points[0].x());
    points[pointCount].y(points[0].y());

    // Calculate distance to the point of intersection for two tangents, making
    // use of the fact that the first intersection will be on (center.x() + radius).
    float vx = std::cos(theta);
    float vy = std::sin(theta);
    float px = center.x() + radius * vx;
    float py = center.x() + radius * vy;

    float t = (center.x() + radius - px) / vy;
    float s = py - t * vx;
    float bezierDistance = (float) euclideanDistance(px, py, center.x() + radius, s) * roundness;

    // Start the bezier path.
    juce::Path blob;
    blob.startNewSubPath(juce::Point<float>(points[0].x(), points[0].y()));

    // Draw bezier curves through points, skip first point.
    for (size_t i = 1; i < pointCount + 1; i++) {
        // Tangent vector for [x,y] is [y, -x]
        // Calculate bezier points for the previous point.
        float bezierX1 = points[i - 1].x() - vectors[i - 1].y() * bezierDistance;
        float bezierY1 = points[i - 1].y() - (-1.0f * vectors[i - 1].x()) * bezierDistance;

        // Calculate bezier points for the target point.
        float bezierX2 = points[i].x() + vectors[i].y() * bezierDistance;
        float bezierY2 = points[i].y() + (-1.0f * vectors[i].x()) * bezierDistance;

        // blob.lineTo(pointX, pointY);
        blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, points[i].x(), points[i].y());

        // draw bezier curve points for the second point
        if (i == 1) {
            g.setColour(juce::Colours::cyan);
            g.fillEllipse(juce::Rectangle<float>(bezierX1 - pointRadius, bezierY1 - pointRadius, pointRadius * 2.0f, pointRadius * 2.0f));
            g.drawLine(juce::Line<float>(points[i - 1].x(), points[i - 1].y(), bezierX1, bezierY1));

            g.setColour(juce::Colours::teal);
            g.fillEllipse(juce::Rectangle<float>(bezierX2 - pointRadius, bezierY2 - pointRadius, pointRadius * 2.0f, pointRadius * 2.0f));
            g.drawLine(juce::Line<float>(points[i].x(), points[i].y(), bezierX2, bezierY2));
        }
    }

    g.setColour(gradientTo);
    g.strokePath(blob, juce::PathStrokeType(2.0f));

    return blob;
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
        target.x(center + (deltaX / length) * radius);
        target.y(center + (deltaY / length) * radius);

    } else {
        target.x(event.x);
        target.y(event.y);
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
    size_t pointCount = blobs[0].size() - 1;
    float radius = 128.0;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / pointCount);
    size_t i = 0;

    // Evenly lay out points on the circle.
    for (size_t j = 0; j < pointCount; j++) {
        // Add slow rotation
        // if (wobbling) wobbler += 0.005f;

        vecs[i][j].x(std::cos(theta * j + wobbler));
        vecs[i][j].y(std::sin(theta * j + wobbler));

        // Bring back "wobbling" and distinguish to rotator.

        // float r = 48.0 * (juce::Random::getSystemRandom().nextFloat() - 0.5f);
        float r = 0.0f;

        blobs[i][j].x(current.x() + (radius + r) * vecs[i][j].x());
        blobs[i][j].y(current.y() + (radius + r) * vecs[i][j].y());
    }

    // Close the path by adding the first point at the end again.
    vecs[i][pointCount].x(vecs[i][0].x());
    vecs[i][pointCount].y(vecs[i][0].y());
    blobs[i][pointCount].x(blobs[i][0].x());
    blobs[i][pointCount].y(blobs[i][0].y());
}

void ThermalComponent::update() {
    // basic tweening
    current.x(current.x() + (target.x() - current.x()) * 0.1f);
    current.y(current.y() + (target.y() - current.y()) * 0.1f);
}

void ThermalComponent::resized() {
    // This is called when the ThermalComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
