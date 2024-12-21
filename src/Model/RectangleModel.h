#pragma once

#include "Renderer/BufferLayout.h"
#include "glm/glm.hpp"
#include <array>

struct RectangleModel2D {
    std::array<PositionWithTexture2D, 4> m_points = { {
        { { 1.0, 1.0 }, { 1.0, 1.0 } },
        { { -1.0, 1.0 }, { 0.0, 1.0 } },
        { { 1.0, -1.0 }, { 1.0, 0.0 } },
        { { -1.0, -1.0 }, { 0.0, 0.0 } },
    } };
    std::array<unsigned int, 6> m_indices = { { 0, 1, 2, 1, 2, 3 } };
};
