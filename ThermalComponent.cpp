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
    setFramesPerSecond(24);

    stepSize = stepSize_;
    blobSize = blobSize_;
    gradientFrom = gradientFrom_;
    gradientTo = gradientTo_;
    minRadius = 48.0;

    size_t blobCount = (size_t) ((getWidth() - inset - blobSize) / stepSize);

    blobs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
    blobsTarget.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
    radi.resize((size_t) blobCount, std::vector<float>((size_t) verticeCount + 1));

    vecs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));

    wobbler = (float) 0.0f;
    rotator = (float) 0.0f;
    computeTarget(true);
}

/**
 * Calculate distance to the point of intersection for two tangents, making
 * use of the fact that the first intersection will be on (center.x() + radius).
 */
float ThermalComponent::calculateBezierDistance(float radius) {
    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / blobs.size());

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

    u_int8_t r1 = gradientFrom.getRed(), g1 = gradientFrom.getGreen(), b1 = gradientFrom.getBlue();
    u_int8_t r2 = gradientTo.getRed(), g2 = gradientTo.getGreen(), b2 = gradientTo.getBlue();

    float roundness = 1.1f;
    size_t pointCount = blobs.size();

    for (size_t i = pointCount - 1; i < pointCount; --i) {
        // Determines roundness of blob path
        float bezierDistance = calculateBezierDistance(minRadius + (stepSize * i)) * roundness;

        // Start the bezier path.
        juce::Path blob;
        blob.startNewSubPath(juce::Point<float>(blobs[i][0].x(), blobs[i][0].y()));

        // Draw bezier curves through points, skip first point.
        for (size_t j = 1; j < blobs[i].size(); j++) {
            // Tangent vector for [x,y] is [y, -x]
            // Calculate bezier points for the previous point.
            float bezierX1 = blobs[i][j - 1].x() - vecs[i][j - 1].y() * bezierDistance;
            float bezierY1 = blobs[i][j - 1].y() - (-1.0f * vecs[i][j - 1].x()) * bezierDistance;

            // Calculate bezier points for the target point.
            float bezierX2 = blobs[i][j].x() + vecs[i][j].y() * bezierDistance;
            float bezierY2 = blobs[i][j].y() + (-1.0f * vecs[i][j].x()) * bezierDistance;

            // blob.lineTo(pointX, pointY);
            blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, blobs[i][j].x(), blobs[i][j].y());
        }

        if (i == 0) {
            g.setColour(baseColor);
            g.fillPath(blob);
            continue;
        }

        // Calculate next colour in gradient.
        float percent = (float) i / (pointCount - 1);

        g.setColour(juce::Colour(
            (u_int8_t) (r1 * percent + r2 * (1.0f - percent)),
            (u_int8_t) (g1 * percent + g2 * (1.0f - percent)),
            (u_int8_t) (b1 * percent + b2 * (1.0f - percent))
        ));

        g.fillPath(blob);
    }

    // // Create illusion of a hole
    /* juce::Path hole;
    float halfInset = inset * 0.5f;

    hole.addRectangle(juce::Rectangle<int>(0, 0, getWidth(), getHeight()));
    hole.setUsingNonZeroWinding(false);

    float ringOffset = 8;
    float ringStrokeWidth = 2;

    hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset, halfInset - ringOffset, getWidth() - inset + ringOffset * 2, getHeight() - inset + ringOffset * 2));
    hole.addEllipse(juce::Rectangle<float>(halfInset - ringOffset + ringStrokeWidth, halfInset - ringOffset + ringStrokeWidth, getWidth() - inset + ringOffset * 2 - ringStrokeWidth * 2, getHeight() - inset + ringOffset * 2 - ringStrokeWidth * 2));
    hole.addEllipse(juce::Rectangle<float>(halfInset, halfInset, getWidth() - inset, getHeight() - inset));

    g.setColour(baseColor);
    g.fillPath(hole);

    // Cursor
    g.setColour(gradientFrom);
    g.drawLine(juce::Line<float>(target.x() - 10.0f, target.y(), target.x() + 10.0f, target.y()));
    g.drawLine(juce::Line<float>(target.x(), target.y() - 10.0f, target.x(), target.y() + 10.0f)); */
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
    float radius = minRadius;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / verticeCount);
    // size_t i = 0;

    for (size_t i = 0; i < blobs.size(); i++) {
        // Evenly lay out points on the circle.
        for (size_t j = 0; j < verticeCount; j++) {
            // Add slow rotation
            // if (wobbling) wobbler += 0.005f;

            // TODO: needs to be updated only at the right time.
            radi[i][j] = radius + (stepSize * i) + (stepSize * 0.5) * (juce::Random::getSystemRandom().nextFloat() - 0.5);

            vecs[i][j].x(std::cos(theta * j + wobbler));
            vecs[i][j].y(std::sin(theta * j + wobbler));


            // TODO: calculate offset from current position instead, so the changes can follow the cursor

            blobsTarget[i][j].x(position.x() + radi[i][j] * vecs[i][j].x());
            blobsTarget[i][j].y(position.y() + radi[i][j] * vecs[i][j].y());
            // printf("<P>: <%.2f, %.2f>\n", blobsTarget[i][j].x(), blobsTarget[i][j].y());

            if (!fastforward) continue;
            blobs[i][j].x(position.x() + radi[i][j] * vecs[i][j].x());
            blobs[i][j].y(position.y() + radi[i][j] * vecs[i][j].y());
        }

        // Close the path by adding the first point at the end again.
        vecs[i][verticeCount].x(vecs[i][0].x());
        vecs[i][verticeCount].y(vecs[i][0].y());
        blobsTarget[i][verticeCount].x(blobsTarget[i][0].x());
        blobsTarget[i][verticeCount].y(blobsTarget[i][0].y());

        if (fastforward) {
            blobs[i][verticeCount].x(blobs[i][0].x());
            blobs[i][verticeCount].y(blobs[i][0].y());
        }
    }
}

void ThermalComponent::update() {
    // basic cursor tweening
    position.x(position.x() + (target.x() - position.x()) * 0.1f);
    position.y(position.y() + (target.y() - position.y()) * 0.1f);

    // blob radi tweening
    for (size_t i = 0; i < blobs.size(); i++) {
        for (size_t j = 0; j < verticeCount; j++) {
            blobs[i][j].x(blobs[i][j].x() + (blobsTarget[i][j].x() - blobs[i][j].x()) * 0.1f);
            blobs[i][j].y(blobs[i][j].y() + (blobsTarget[i][j].y() - blobs[i][j].y()) * 0.1f);
        }

        blobs[i][verticeCount].x(blobs[i][verticeCount].x() + (blobsTarget[i][verticeCount].x() - blobs[i][verticeCount].x()) * 0.1f);
        blobs[i][verticeCount].y(blobs[i][verticeCount].y() + (blobsTarget[i][verticeCount].y() - blobs[i][verticeCount].y()) * 0.1f);
    }

    if (rotator++ < 20.0) return;
    computeTarget();
    rotator = 0.0;
}

void ThermalComponent::resized() {
    // This is called when the ThermalComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

juce::Path ThermalComponent::drawBlob(juce::Graphics& g, Point<float>& center, float radius, size_t pointCount, float roundness, bool wobbling) {
    float pointRadius = 3.0f;
    size_t i = 0;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / pointCount);

    // Calculate distance to the point of intersection for two tangents, making
    // use of the fact that the first intersection will be on (center.x() + radius).
    float vx = std::cos(theta);
    float vy = std::sin(theta);
    float px = center.x() + radius * vx;
    float py = center.x() + radius * vy;

    float t = (center.x() + radius - px) / vy;
    float s = py - t * vx;
    float bezierDistance = Point<float>::distance(px, py, center.x() + radius, s) * roundness;

    // Start the bezier path.
    juce::Path blob;
    blob.startNewSubPath(juce::Point<float>(blobs[i][0].x(), blobs[i][0].y()));

    // Draw bezier curves through points, skip first point.
    for (size_t j = 1; j < pointCount + 1; j++) {
        // Tangent vector for [x,y] is [y, -x]
        // Calculate bezier points for the previous point.
        float bezierX1 = blobs[i][j - 1].x() - vecs[i][j - 1].y() * bezierDistance;
        float bezierY1 = blobs[i][j - 1].y() - (-1.0f * vecs[i][j - 1].x()) * bezierDistance;

        // Calculate bezier points for the target point.
        float bezierX2 = blobs[i][j].x() + vecs[i][j].y() * bezierDistance;
        float bezierY2 = blobs[i][j].y() + (-1.0f * vecs[i][j].x()) * bezierDistance;

        // blob.lineTo(pointX, pointY);
        blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, blobs[i][j].x(), blobs[i][j].y());

        // draw bezier curve points for the second point
        if (j == 1) {
            g.setColour(juce::Colours::cyan);
            g.fillEllipse(juce::Rectangle<float>(bezierX1 - pointRadius, bezierY1 - pointRadius, pointRadius * 2.0f, pointRadius * 2.0f));
            g.drawLine(juce::Line<float>(blobs[i][j - 1].x(), blobs[i][j - 1].y(), bezierX1, bezierY1));

            g.setColour(juce::Colours::teal);
            g.fillEllipse(juce::Rectangle<float>(bezierX2 - pointRadius, bezierY2 - pointRadius, pointRadius * 2.0f, pointRadius * 2.0f));
            g.drawLine(juce::Line<float>(blobs[i][j].x(), blobs[i][j].y(), bezierX2, bezierY2));
        }
    }

    g.setColour(gradientTo);
    g.strokePath(blob, juce::PathStrokeType(2.0f));

    return blob;
}
