#include "sandboxApp.hpp"
#include <csignal>
#include <exception>
#include <iostream>

volatile sig_atomic_t g_quitRequested = 0;

void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::clog << "\nCtrl+C detected. Shutting down..." << std::endl;

        g_quitRequested = 1;
    }
}

int main() {
    std::signal(SIGINT, handleSignal);

    try {
        SandboxApp app;
        app.run();

    } catch (const std::exception &e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
