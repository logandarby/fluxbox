

#include "Application.h"
#include "Scene/FluidScene.h"
#include <algorithm>
#include <ranges>

static const ApplicationSpec APP_SPEC = { .width = 960,
                                          .height = 540,
                                          .name = "FluxLab" };

void runApplication() {
    Application app(APP_SPEC);
    auto fluidScene = std::make_shared<FluidScene>(APP_SPEC);
    app.setScene(fluidScene);

    app.run();
}

int main(void) {
    // Core initializations
    Logger::init();
    // App initialization
    try {
        runApplication();
        LOG_CORE_INFO("Application Termination Success");
    } catch (std::runtime_error& e) {
        LOG_CORE_ERR("Runtime Error {}", e.what());
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        LOG_CORE_ERR("Unknown error {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}