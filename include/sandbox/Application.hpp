#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <thread>

#include "sandbox/LedDisplay.hpp"
#include "sandbox/Scene.hpp"
#include "sandbox/SceneParser.hpp"
#include "sandbox/SceneFolderMonitor.hpp"
#include "sandbox/comms/DisplayIpcServer.hpp"
#include "sandbox/comms/RestApiServer.hpp"

namespace sandbox
{
class Application
{
public:

    explicit Application(const std::atomic<bool>& running);
    ~Application() = default;

    void run();

private:

    sandbox::config::ApplicationConfig mConfig;
    std::filesystem::path mWatchedFolder = "";
    uint16_t mPort;

    std::unique_ptr<sandbox::comms::DisplayIpcServer> mDisplayIpcServer = nullptr;
    std::unique_ptr<sandbox::LedDisplay> mLedDisplay = nullptr;
    std::unique_ptr<sandbox::SceneFolderMonitor> mSceneFolderMonitor = nullptr;
    std::unique_ptr<comms::RestApiServer> mRestApiServer = nullptr;
    std::unique_ptr<sandbox::SceneParser> mParser = nullptr;
    std::vector<sandbox::Scene> mScenes {};
    const std::atomic<bool>& mIsRunning;

    bool init();
    void shutdown();

    void runIpcIngestion();
    void runFolderWatcherIngestion();
    void runRestApiIngestion();
};
}
