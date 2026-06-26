#include "sandbox/Application.hpp"

#include <iostream>
#include <string>
#include <thread>

#include "sandbox/utils/FontLibrary.hpp"

using sandbox::SceneParser;
using sandbox::ApplicationConfig;
using sandbox::utils::FontLibrary;
using rgb_matrix::RGBMatrix;

namespace sandbox
{
Application::Application(const std::atomic<bool>& running) : 
    mRunning(running),
    mParser(std::make_unique<SceneParser>())
{
    init();
}

bool Application::init()
{
    mConfig = ApplicationConfig::load("config/led-display.toml");

    RGBMatrix::Options options;
    mConfig.rgbMatrix.applyTo(options);

    mWatchedFolder = mConfig.data.jsonFolderWatcher.folder;

    mSceneFolderMonitor = std::make_unique<SceneFolderMonitor>(mWatchedFolder);
    mSceneFolderMonitor->start();
    
    mLedDisplay = std::make_unique<LedDisplay>(options, mConfig.fonts);

    if (mLedDisplay->init())
    {
        std::cerr << "Failed to initialize LED display." << std::endl;
        return false;
    }

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