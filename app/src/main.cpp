#include <Ghost/ghostRender.hpp>
#include <csignal>
#include <iostream>

void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::clog << "\nCtrl+C detected. Shutting down..."
                  << std::endl;
        Ghost::GhostRender::s_quitFlag.store(true);
    }
}

int main() {
    std::signal(SIGINT, handleSignal);
    Ghost::GhostRender render;
    render.run();
    return 0;
}
