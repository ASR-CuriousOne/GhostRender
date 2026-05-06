#include <Ghost/ghostApp.hpp>
#include <csignal>
#include <iostream>

void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::clog << "\nCtrl+C detected. Shutting down..." << std::endl;
        Ghost::GhostApp::s_quitFlag.store(true);
    }
}

int main() {
    std::signal(SIGINT, handleSignal);
    Ghost::GhostApp app;
    app.run();
    return 0;
}
