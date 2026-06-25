#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "sandbox/LedDisplay.hpp"
#include "sandbox/Scene.hpp"
#include "sandbox/SceneParser.hpp"
#include "sandbox/SceneFolderMonitor.hpp"

namespace sandbox
{
class Application
{
public:
    explicit Application(const std::atomic<bool>& running);
    ~Application() = default;

    void run();

private:

    std::unique_ptr<sandbox::LedDisplay> mLedDisplay = nullptr;
    std::unique_ptr<sandbox::SceneFolderMonitor> mSceneFolderMonitor = nullptr;
    std::unique_ptr<sandbox::SceneParser> mParser = nullptr;
    std::vector<sandbox::Scene> mScenes {};
    const std::atomic<bool>& mRunning;

    bool init();
};
}
