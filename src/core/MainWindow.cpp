#include "MainWindow.h"
#include "imgui.h"
#include "Core/GLCore.h"
#include "Renderer/RendererTypes.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Utils {
    static const std::function<void(Event&)> EMPTY_EVENT_CALLBACK = [](Event& e
                                                                    ) {};
    static void glfwErrorCallback(int error, const char* desc) {
        LOG_GL_ERR("GLEW Error: [{}]: {}", error, desc);
    }
    static WindowUserPtr& getUserPtr(GLFWwindow* window) {
        return *static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window));
    }
}  // namespace Utils

MainWindow::~MainWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    LOG_CORE_INFO("GLFW Window Destroyed");
}

MainWindow::MainWindow(
    const int width, const int height, const std::string& name
) :
    m_window(nullptr),
    m_data({Utils::EMPTY_EVENT_CALLBACK}),
    m_isRunning{false} {
    glfwSetErrorCallback(Utils::glfwErrorCallback);

    if (!glfwInit()) throw std::runtime_error("GLFW Could not initialize");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("GLFW Could not create a window");
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Error intializing GLEW");
    }
    LOG_GL_INFO("Using GL version {}", (char*)glGetString(GL_VERSION));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(
        m_window,
        true
    );  // Second param install_callback=true will install
        // GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glDepthFunc(GL_LEQUAL));

    glfwSetWindowUserPointer(m_window, &m_data);
    toggleCursorMode(GL::CursorMode::Normal);
    initEventHandling();

    m_isRunning = true;
    glfwGetCursorPos(m_window, &m_data.lastCursorX, &m_data.lastCursorY);
}

void MainWindow::initEventHandling() {
    glfwSetKeyCallback(
        m_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
            WindowUserPtr& data = Utils::getUserPtr(window);
            switch (action) {
                case GLFW_PRESS: {
                    KeyPressEvent ep(key, mods);
                    data.eventCallback(ep);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleaseEvent er(key, mods);
                    data.eventCallback(er);
                    break;
                }
            }
        }
    );
    glfwSetScrollCallback(
        m_window,
        [](GLFWwindow* window, double xoffset, double yoffset) {
            ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
            WindowUserPtr& data = Utils::getUserPtr(window);
            MouseScrollEvent e(xoffset, yoffset);
            data.eventCallback(e);
        }
    );
    glfwSetCursorPosCallback(
        m_window,
        [](GLFWwindow* window, double xpos, double ypos) {
            ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
            WindowUserPtr& data = Utils::getUserPtr(window);
            MouseMoveEvent e(xpos, ypos);
            data.eventCallback(e);

            const double xOffset = xpos - data.lastCursorX;
            const double yOffset = ypos - data.lastCursorY;
            MouseOffsetEvent offsetEvent(xOffset, yOffset);
            data.eventCallback(offsetEvent);
            data.lastCursorX = xpos;
            data.lastCursorY = ypos;
        }
    );
}

bool MainWindow::isOpen() {
    return !glfwWindowShouldClose(m_window) && m_isRunning;
}

void MainWindow::whileOpen(std::function<void(void)> renderLoop) {
    while (isOpen()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderLoop();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(m_window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

void MainWindow::destroy() {
    m_isRunning = false;
}

void MainWindow::toggleCursorMode(const GL::CursorMode mode) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GL::getCursorMode(mode));
}
