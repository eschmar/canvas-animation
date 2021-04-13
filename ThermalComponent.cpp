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
    setFramesPerSecond(60);
    // TODO: calculate rotation speed using fps as base

    stepSize = stepSize_;
    blobSize = blobSize_;
    gradientFrom = gradientFrom_;
    gradientTo = gradientTo_;
    minRadius = 48.0;

    size_t blobCount = (size_t) ((getWidth() - inset - blobSize) / stepSize);

    blobs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
    blobsTarget.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
    radi.resize((size_t) blobCount, std::vector<float>((size_t) verticeCount + 1));
    radiTarget.resize((size_t) blobCount, std::vector<float>((size_t) verticeCount + 1));
    vecs.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));


    // blobPos.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
    // blobTarget.resize((size_t) blobCount, std::vector<Point<float>>((size_t) verticeCount + 1));
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
    // g.fillAll(gradientFrom);
    g.fillAll(baseColor);

    // float roundness = 1.1f;
    float roundness = 0.6f;
    size_t pointCount = blobs.size();
    pointCount = 1;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / verticeCount);

    for (size_t i = pointCount - 1; i < pointCount; --i) {
        juce::Path blob;

        float radius = (blobSize + (stepSize * i)) * (1.0f - epsilon * (1.0f - ((float) i / pointCount)) * blobRadius[0]);
        float bezierDistance = calculateBezierDistance(radius) * roundness;

        float prevX = position.x() + radius * (float) std::cos(0);
        float prevY = position.y() + radius * (float) std::sin(0);

        // g.setColour(juce::Colour(0x77ed6809));
        // g.fillEllipse(prevX - 5, prevY - 5, 10, 10);

        blob.startNewSubPath(juce::Point<float>(prevX, prevY));

        for (size_t j = 1; j < blobRadius.size(); j++) {
            radius = (blobSize + (stepSize * i)) * (1.0f - epsilon * (1.0f - ((float) i / pointCount)) * blobRadius[j]);

            // polar to cartesian vector
            float vx = std::cos(theta * j);
            float vy = std::sin(theta * j);

            // next point position
            float px = position.x() + radius * vx;
            float py = position.y() + radius * vy;

            g.setColour(juce::Colour(0x7709e6ed));
            g.fillEllipse(px - 5, py - 5, 10, 10);

            // Bezier points using perpendicular vector [-vy, vx].
            // Calculate bezier points for the previous point.
            float bezierX1 = prevX - std::sin(theta * (j - 1)) * bezierDistance;
            float bezierY1 = prevY - (-1.0f * std::cos(theta * (j - 1))) * bezierDistance;

            // Calculate bezier points for the target point.
            float bezierX2 = px + vy * bezierDistance;
            float bezierY2 = py + (-1.0f * vx) * bezierDistance;

            blob.cubicTo(bezierX1, bezierY1, bezierX2, bezierY2, px, py);

            prevX = px;
            prevY = py;

            if (j == blobRadius.size() - 1) {
                g.setColour(juce::Colour(0x77aa1100));
                g.fillEllipse(bezierX1 - 5, bezierY1 - 5, 10, 10);
                g.fillEllipse(bezierX2 - 5, bezierY2 - 5, 10, 10);

                g.setColour(juce::Colour(0x770011aa));
                g.fillEllipse(px - 5, py - 5, 10, 10);
            }
        }

        // blob.lineTo(px, py);

        // Calculate next colour in gradient.
        float percent = (float) i / (pointCount - 1);

        g.setColour(juce::Colour(
            (u_int8_t) (gradientFrom.getRed() * percent + gradientTo.getRed() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getGreen() * percent + gradientTo.getGreen() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getBlue() * percent + gradientTo.getBlue() * (1.0f - percent))
        ));

        // g.setColour(i % 2 == 0 ? juce::Colour(0xaadeed09) : juce::Colour(0x22aa1100));
        g.fillPath(blob);
    }

    /* for (size_t i = pointCount - 1; i < pointCount; --i) {
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

            if (i == 0) {
                g.setColour(juce::Colour(0x77aa1100));
                g.fillEllipse(bezierX1 - 5, bezierY1 - 5, 10, 10);
                g.fillEllipse(bezierX2 - 5, bezierY2 - 5, 10, 10);
                g.setColour(juce::Colour(0x770011aa));
                g.fillEllipse(blobs[i][j].x() - 5, blobs[i][j].y() - 5, 10, 10);
            }
        }

        // The most inner (last) blob should be special colour
        if (i == 0) {
            g.setColour(baseColor);
            g.fillPath(blob);
            continue;
        }

        // Calculate next colour in gradient.
        float percent = (float) i / (pointCount - 1);

        g.setColour(juce::Colour(
            (u_int8_t) (gradientFrom.getRed() * percent + gradientTo.getRed() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getGreen() * percent + gradientTo.getGreen() * (1.0f - percent)),
            (u_int8_t) (gradientFrom.getBlue() * percent + gradientTo.getBlue() * (1.0f - percent))
        ));

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

    g.setColour(baseColor); */
    // g.fillPath(hole);

    // Simple one-coloured outer ring
    // g.setColour(gradientFrom);
    // g.drawEllipse(
    //     halfInset - ringOffset,
    //     halfInset - ringOffset,
    //     getWidth() - inset + ringOffset * 2,
    //     getHeight() - inset + ringOffset * 2,
    //     3.0
    // );

    // Cursor
    // g.setColour(gradientFrom);
    // g.drawLine(juce::Line<float>(target.x() - 10.0f, target.y(), target.x() + 10.0f, target.y()));
    // g.drawLine(juce::Line<float>(target.x(), target.y() - 10.0f, target.x(), target.y() + 10.0f));
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
}

void ThermalComponent::computeTarget(bool fastforward) {
    // Periodically randomise blob radius slightly.
    bool shouldRandomiseRadius = fmod(getFrameCounter(), 48) == 0;

    if (shouldRandomiseRadius) {
        for (size_t i = 0; i < blobRadiusTarget.size(); i++) {
            blobRadiusTarget[i] = juce::Random::getSystemRandom().nextFloat();
            if (i == blobRadiusTarget.size() - 1) blobRadiusTarget[i] = blobRadiusTarget[0];
            if (fastforward) blobRadius[i] = blobRadiusTarget[i];
        }
    }

    float halfSize = size * 0.5f;

    // Angle in radians between each point.
    float theta = (float) (M_PI * 2.0 / verticeCount);

    // Add slow rotation.
    wobbler += 0.002f;

    for (size_t i = 0; i < blobs.size(); i++) {
        // Add perspective to blobs by offsetting the position slightly
        float distance = position.distance(Point<float>(halfSize, halfSize));
        float beta = std::atan((position.y() - halfSize) / (position.x() - halfSize));
        if (position.x() < halfSize) beta += (float) M_PI; // Adjust theta depending on quadrant

        float offset = i * stepSize * 3 * (distance / (size - inset) * 0.5f); // The farther from origin the more offset
        float perspX = halfSize + (distance - offset) * std::cos(beta);
        float perspY = halfSize + (distance - offset) * std::sin(beta);

        // Avoid divide by zero, which will break tweening
        if (distance == 0) {
            perspX = position.x();
            perspY = position.y();
        }

        // Evenly lay out points on the circle.
        for (size_t j = 0; j < verticeCount; j++) {
            // Rewrite this section with polar coordinates. maybe also replace above, just use one theta, not beta.

            float radius = minRadius + (stepSize * i);

            if (shouldRandomiseRadius) {
                // TODO: Improve radius calc
                // Radnomise radius to generate different blobs.
                radiTarget[i][j] = radius + (stepSize * 0.4) * (juce::Random::getSystemRandom().nextFloat() - 0.5);
                if (fastforward) radi[i][j] = radiTarget[i][j];
            }

            vecs[i][j].x(std::cos(theta * j + wobbler));
            vecs[i][j].y(std::sin(theta * j + wobbler));

            blobsTarget[i][j].x(perspX + radi[i][j] * vecs[i][j].x());
            blobsTarget[i][j].y(perspY + radi[i][j] * vecs[i][j].y());

            if (!fastforward) continue;
            blobs[i][j].x(perspX + radi[i][j] * vecs[i][j].x());
            blobs[i][j].y(perspY + radi[i][j] * vecs[i][j].y());
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

    // epsiloning
    for (size_t i = 0; i < blobRadius.size(); i++) {
        blobRadius[i] = blobRadius[i] + (blobRadiusTarget[i] - blobRadius[i]) * 0.1f;
    }

    // blob radi tweening
    for (size_t i = 0; i < blobs.size(); i++) {
        for (size_t j = 0; j < verticeCount; j++) {
            blobs[i][j].x(blobs[i][j].x() + (blobsTarget[i][j].x() - blobs[i][j].x()) * 0.1f);
            blobs[i][j].y(blobs[i][j].y() + (blobsTarget[i][j].y() - blobs[i][j].y()) * 0.1f);
            radi[i][j] = radi[i][j] + (radiTarget[i][j] - radi[i][j]) * 0.005f;
        }

        blobs[i][verticeCount].x(blobs[i][verticeCount].x() + (blobsTarget[i][verticeCount].x() - blobs[i][verticeCount].x()) * 0.1f);
        blobs[i][verticeCount].y(blobs[i][verticeCount].y() + (blobsTarget[i][verticeCount].y() - blobs[i][verticeCount].y()) * 0.1f);
    }

    computeTarget();
}

void ThermalComponent::resized() {}

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
