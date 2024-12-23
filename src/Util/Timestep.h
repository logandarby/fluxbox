#pragma once

#include <chrono>

class Timestep {
public:
    using CurrentTime = std::chrono::_V2::system_clock::time_point;

    static CurrentTime getCurrentTime();
    static float getDeltaTimeS(
        Timestep::CurrentTime currentTime, CurrentTime lastTime
    );
};
