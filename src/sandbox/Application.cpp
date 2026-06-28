#include "sandbox/Application.hpp"

#include <iostream>
#include <string>
#include <thread>

#include "sandbox/config/ApplicationConfig.hpp"
#include "sandbox/utils/FontLibrary.hpp"

using sandbox::SceneParser;
using sandbox::comms::DisplayIpcServer;
using sandbox::config::ApplicationConfig;
using sandbox::utils::FontLibrary;

namespace sandbox
{
Application::Application(const std::atomic<bool>& running) : 
    mRunning(running),
    mParser(std::make_unique<SceneParser>())
{
    if (!init())
    {
        throw std::runtime_error("Failed to initialize application.");
    }
}

bool Application::init()
{
    mConfig = ApplicationConfig::load("config/led-display.toml");

    RGBMatrix::Options options;
    mConfig.rgbMatrix.applyTo(options);

    switch (mConfig.data.method)
    {
        case config::DataIngestionMethod::ZMQ_IPC:
            mDisplayIpcServer = std::make_unique<DisplayIpcServer>(
                mConfig.data.zmqIpc.endpoint);
            break;
    
        case config::DataIngestionMethod::FOLDER_WATCHER:
            mWatchedFolder = mConfig.data.jsonFolderWatcher.folder;
    
            mSceneFolderMonitor = std::make_unique<SceneFolderMonitor>(mWatchedFolder);
            mSceneFolderMonitor->start();
            break;
    }
    
    mLedDisplay = std::make_unique<LedDisplay>(options, mConfig.fonts);

    if (!mLedDisplay->init())
    {
        std::cerr << "Failed to initialize LED display." << std::endl;
        return false;
    }

    return true;
}

void Application::shutdown()
{
    std::cout << "Shutting down LED-Display..." << std::endl;
    mDisplayIpcServer.reset();

    if (mSceneFolderMonitor)
    {
        mSceneFolderMonitor->stop();
    }

    if (mLedDisplay)
    {
        mLedDisplay->shutdown();
    }
}

void Application::run()
{
    std::cout << "Starting LED-Display..." << std::endl;
    while (mRunning)
    {
        std::string jsonText;

        if (mDisplayIpcServer->tryReceive(jsonText))
        {
            try 
            {
                const Scene scene = mParser->parseJsonText(jsonText);
                std::vector<Scene> scenes {scene};
                mLedDisplay->draw(scenes);
            }
            catch (const std::exception& error)
            {
                std::cerr << "Rejected scene: " << error.what() << '\n';
            }
        }
    }
    std::cout << "Stopping LED-Display..." << std::endl;

    shutdown();
}

/*
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
*/
}
