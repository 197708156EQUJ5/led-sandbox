#include "sandbox/Application.hpp"

#include <iostream>
#include <string>
#include <thread>

using sandbox::SceneParser;

namespace sandbox
{
Application::Application() :
    mParser(std::make_unique<SceneParser>()),
    mLedDisplay(std::make_unique<LedDisplay>())
{
    init();
}

bool Application::init()
{
    mSceneFolderMonitor = std::make_unique<SceneFolderMonitor>("assets/scenes");
    mSceneFolderMonitor->start();

    return true;
}

void Application::run()
{
    const auto scene = mParser->parse("assets/scenes/sandbox.json");

    while (true)
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
}
}