#include "TrackpadComponent.h"

TrackpadComponent::TrackpadComponent(int size_, int inset_, int fps_) : size(size_), inset(inset_), fps(fps_) {
    setSize(size_, size_);
    setFramesPerSecond (fps_);
    x = getWidth() * 0.5f;
    y = getHeight() * 0.5f;
}

void TrackpadComponent::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Draggable circle
    float radius = 16;
    g.setColour(juce::Colours::yellow);
    g.drawEllipse((float) x - radius, (float) y - radius, radius * 2, radius * 2, 3);
}

void TrackpadComponent::mouseDrag(const juce::MouseEvent& event) {
    // printf(">>> <x,y>=<%d,%d>\n", event.x, event.y);
    std::tuple<float, float> relPos = calculateRelativePosition(event.x, event.y);
    std::tuple<float, float> absPos = calculatePixelPosition(std::get<0>(relPos), std::get<1>(relPos));
    x = std::get<0>(absPos);
    y = std::get<1>(absPos);
    // printf(">>> <x',y'>=<%.2f,%.2f>\n", x, y);
}

/**
 * Translates the coordinate position to relative values.
 *
 * @return Relative coordinate position.
 */
std::tuple<float, float> TrackpadComponent::calculateRelativePosition(int pixelX, int pixelY) {
    float relX, relY;

    relX = (pixelX - inset * 0.5f) / ((float) getWidth() - inset);
    relY = (pixelY - inset * 0.5f) / ((float) getHeight() - inset);

    return {
        std::min(1.0f, std::max(0.0f, relX)),
        std::min(1.0f, std::max(0.0f, relY))
    };
}

/**
 * Translates the coordinate position to relative values.
 *
 * @return Returns relative coordinate position.
 */
std::tuple<int, int> TrackpadComponent::calculatePixelPosition(float relX, float relY) {
    int posX, posY;

    float halfInset = inset * 0.5f;
    posX = (int) (relX * ((float) getWidth() - inset) + halfInset);
    posY = (int) (relY * ((float) getHeight() - inset) + halfInset);

    return {
        std::min(getWidth() - (int) halfInset, (int) std::max((int) halfInset, posX)),
        std::min(getHeight() - (int) halfInset,(int) std::max((int) halfInset, posY)),
    };
}

void TrackpadComponent::update() {
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

void TrackpadComponent::resized() {
    // This is called when the TrackpadComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
