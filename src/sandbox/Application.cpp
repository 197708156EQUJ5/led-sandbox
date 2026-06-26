#include "sandbox/Application.hpp"

#include <iostream>
#include <string>
#include <thread>

using sandbox::SceneParser;
using sandbox::ApplicationConfig;
using rgb_matrix::RGBMatrix;

namespace sandbox
{
Application::Application(const std::atomic<bool>& running) : 
    mRunning(running),
    mParser(std::make_unique<SceneParser>()),
    mLedDisplay(std::make_unique<LedDisplay>())
{
    init();
}

bool Application::init()
{        
    mConfig = ApplicationConfig::load("config/led-display.toml");

    RGBMatrix::Options options;
    mConfig.rgbMatrix.applyTo(options);

    mMatrix = RGBMatrix::CreateFromOptions(options, mRuntimeOptions);

    if (mMatrix == nullptr)
    {
        return false;
    }

    mWatchedFolder = mConfig.data.jsonFolderWatcher.folder;

    mSceneFolderMonitor = std::make_unique<SceneFolderMonitor>(mWatchedFolder);
    mSceneFolderMonitor->start();

    return true;
}

void Application::run()
{
    const auto scene = mParser->parse(mWatchedFolder / "sandbox.json");

    while (mRunning)
    {
        const auto changed_files = mSceneFolderMonitor->consumeChanges();
        
        if (changed_files.has_value())
        {
            mScenes.clear();
            for (const auto& file : *changed_files)
            {
                mScenes.push_back(mParser->parse(file));
                // Store/render/register the parsed scene here.
            }
        }
        mLedDisplay->draw(mScenes);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    mLedDisplay->close();
}
}