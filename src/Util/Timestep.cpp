#include "Timestep.h"
#include <chrono>

Timestep::CurrentTime Timestep::getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

float Timestep::getDeltaTimeS(
    Timestep::CurrentTime currentTime, Timestep::CurrentTime lastTime
) {
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastTime
    );
    return static_cast<float>(duration.count() / 1000.0f);
}