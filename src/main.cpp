#include <atomic>
#include <csignal>
#include <unistd.h>

#include "sandbox/Application.hpp"

std::atomic<bool> gIsRunning{true};

void handleSignal(int)
{
    write(STDERR_FILENO, "signal received\n", 16);
    gIsRunning = false;
}

int main()
{
    std::signal(SIGINT, handleSignal);   // Ctrl-C
    std::signal(SIGTERM, handleSignal);  // kill <pid>

    sandbox::Application app(gIsRunning);
    app.run();

    return 0;
}