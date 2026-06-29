#pragma once

#include <cstdint>
#include <filesystem>
#include <thread>

#include <crow.h>

#include "sandbox/web/SceneMailbox.hpp"

namespace sandbox::web
{
class WebSceneServer
{
public:
    WebSceneServer(uint16_t port, std::filesystem::path webRoot, SceneMailbox& sceneMailbox);
    ~WebSceneServer();

    WebSceneServer(const WebSceneServer&) = delete;
    WebSceneServer& operator=(const WebSceneServer&) = delete;

    void start();
    void stop();

private:
    void configureRoutes();

    [[nodiscard]] crow::response serveFile(
        const std::filesystem::path& filePath,
        std::string_view contentType) const;

    uint16_t mPort;
    std::filesystem::path mWebRoot;
    SceneMailbox& mSceneMailbox;

    crow::SimpleApp mApp;
    std::thread mServerThread;
};
}