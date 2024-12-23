#include "Application.h"
#include "Events/Event.h"
#include "Renderer/Renderer.h"
#include "Util/Timestep.h"

void Application::keyPressCallback(KeyPressEvent& e) {
    switch (e.getKeyCode()) {
        case GL::KeyCode::Q:
            m_window.destroy();
            break;
    }
}

Application::Application(const ApplicationSpec& spec) :
    m_frameCount{ 0 },
    m_lastTime{ Timestep::getCurrentTime() },
    m_width{ spec.width },
    m_height{ spec.height },
    m_appName{ spec.name },
    m_window{ spec.width, spec.height, spec.name } {
    LOG_CORE_INFO("Application created successfully");
    m_window.setEventCallback([&](Event& e) -> void { onEvent(e); });
}

void Application::onEvent(Event& e) {
    EventDispatcher dispatcher(e);

    dispatcher.dispatch<KeyPressEvent>(
        BIND_EVENT_FUNC(Application::keyPressCallback)
    );

    if (const auto& scene = m_currentScene.lock()) {
        scene->onEvent(e);
    }
}

void Application::setScene(Ref<Scene> scene) {
    if (auto scene = m_currentScene.lock()) {
        scene->onDetach();
    }
    m_currentScene = scene;
    if (auto scene = m_currentScene.lock()) {
        scene->onInit();
    }
}

void Application::run() {
    m_window.whileOpen([&]() -> void {
        const auto currentTime = Timestep::getCurrentTime();
        const float deltaTime =
            Timestep::getDeltaTimeS(currentTime, m_lastTime);
        if (auto scene = m_currentScene.lock()) {
            scene->onRender(deltaTime, m_frameCount);
        }
        m_lastTime = currentTime;
        m_frameCount++;
    });
}
