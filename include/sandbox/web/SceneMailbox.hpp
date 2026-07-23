#pragma once

#include <mutex>
#include <optional>
#include <string>

namespace sandbox::web
{
class SceneMailbox
{
public:
    void submit(std::string sceneJson)
    {
        std::scoped_lock lock(mMutex);
        mLatestSceneJson = std::move(sceneJson);
    }

    [[nodiscard]] std::optional<std::string> takeLatest()
    {
        std::scoped_lock lock(mMutex);

        std::optional<std::string> scene_json = std::move(mLatestSceneJson);
        mLatestSceneJson.reset();

        return scene_json;
    }

private:
    std::mutex mMutex;
    std::optional<std::string> mLatestSceneJson;
};
}