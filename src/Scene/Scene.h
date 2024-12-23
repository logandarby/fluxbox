#pragma once

#include "Events/Event.h"
#include <string>

class Scene {
public:
    Scene(std::string name) : m_name(name) {};
    virtual ~Scene() = default;

    virtual void onInit() = 0;
    virtual void onDetach() {};
    virtual void onRender(const float timestep, const size_t frameCount) = 0;
    virtual void onEvent(Event& event) {};

protected:
    const std::string m_name;
};