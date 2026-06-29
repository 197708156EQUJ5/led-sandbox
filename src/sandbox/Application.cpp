#include "sandbox/Application.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <exception>
#include <utility>

#include "sandbox/config/ApplicationConfig.hpp"
#include "sandbox/utils/FontLibrary.hpp"

using sandbox::SceneParser;
using sandbox::comms::DisplayIpcServer;
using sandbox::config::ApplicationConfig;
using sandbox::utils::FontLibrary;

namespace sandbox
{
Application::Application(const std::atomic<bool>& running) : 
    mIsRunning(running),
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
            
        case config::DataIngestionMethod::REST_API:
            mPort = mConfig.data.restApi.port;
            mRestApiServer = std::make_unique<comms::RestApiServer>(mPort);
            mRestApiServer->start();
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
        mSceneFolderMonitor.reset();
    }

    if (mLedDisplay)
    {
        mLedDisplay->shutdown();
    }

    if (mRestApiServer)
    {
        mRestApiServer->stop();
    }
}

void Application::run()
{
    std::cout << "Starting LED-Display..." << std::endl;

    while (mIsRunning)
    {
        switch (mConfig.data.method)
        {
            case config::DataIngestionMethod::ZMQ_IPC:
                runIpcIngestion();
                break;

            case config::DataIngestionMethod::FOLDER_WATCHER:
                runFolderWatcherIngestion();
                break;

            case config::DataIngestionMethod::REST_API:
                runRestApiIngestion();
                break;
        }
    }

    std::cout << "Stopping LED-Display..." << std::endl;
    shutdown();
}

void Application::runIpcIngestion()
{
    std::string json_text;

    if (!mDisplayIpcServer->tryReceive(json_text))
    {
        return;
    }

    try
    {
        const Scene scene = mParser->parseJsonText(json_text);
        mLedDisplay->draw({scene});
    }
    catch (const std::exception& error)
    {
        std::cerr << "Rejected IPC scene: " << error.what() << '\n';
    }
}

void Application::runFolderWatcherIngestion()
{
    const auto changed_files = mSceneFolderMonitor->consumeChanges();

    if (changed_files.has_value())
    {
        std::vector<Scene> updated_scenes;
        updated_scenes.reserve(changed_files->size());

        for (const auto& file : *changed_files)
        {
            if (file.extension() != ".json")
            {
                continue;
            }

            try
            {
                updated_scenes.push_back(mParser->parseFile(file));
            }
            catch (const std::exception& error)
            {
                std::cerr << "Could not load scene file '" << file.string() << "': " << error.what() << '\n';
            }
        }

        mScenes = std::move(updated_scenes);
        mLedDisplay->draw(mScenes);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Application::runRestApiIngestion()
{
    std::string json_text;

    if (!mRestApiServer->tryReceive(json_text))
    {
        return;
    }

    try
    {
        const Scene scene = mParser->parseJsonText(json_text);
        mLedDisplay->draw({scene});
    }
    catch (const std::exception& error)
    {
        std::cerr << "Rejected REST Api scene: " << error.what() << '\n';
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));    
}
}
