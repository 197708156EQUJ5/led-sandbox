#include "sandbox/web/WebSceneServer.hpp"

#include <fstream>
#include <iterator>
#include <string>

#include <nlohmann/json.hpp>

namespace sandbox::web
{
WebSceneServer::WebSceneServer(uint16_t port, std::filesystem::path webRoot, SceneMailbox& sceneMailbox) :
    mPort(port),
    mWebRoot(std::move(webRoot)),
    mSceneMailbox(sceneMailbox)
{
    configureRoutes();
}

WebSceneServer::~WebSceneServer()
{
    stop();
}

void WebSceneServer::start()
{
    mServerThread = std::thread(
        [this]()
        {
            mApp.port(mPort).multithreaded().run();
        });
}

void WebSceneServer::stop()
{
    mApp.stop();

    if (mServerThread.joinable())
    {
        mServerThread.join();
    }
}

void WebSceneServer::configureRoutes()
{
    CROW_ROUTE(mApp, "/")
    ([this]()
    {
        return serveFile(mWebRoot / "index.html", "text/html; charset=utf-8");
    });

    CROW_ROUTE(mApp, "/app.js")
    ([this]()
    {
        return serveFile(mWebRoot / "app.js", "application/javascript; charset=utf-8");
    });

    CROW_ROUTE(mApp, "/styles.css")
    ([this]()
    {
        return serveFile(mWebRoot / "styles.css", "text/css; charset=utf-8");
    });

    CROW_ROUTE(mApp, "/api/scene")
    .methods(crow::HTTPMethod::POST)
    ([this](const crow::request& request)
    {
        try
        {
            const nlohmann::json scene_json = nlohmann::json::parse(request.body);

            if (!scene_json.contains("shapes") || !scene_json.at("shapes").is_array())
            {
                return crow::response(400, "Expected JSON object containing a shapes array.");
            }

            mSceneMailbox.submit(request.body);

            return crow::response(202, "Scene accepted.");
        }
        catch (const nlohmann::json::parse_error&)
        {
            return crow::response(400, "Request body is not valid JSON.");
        }
    });
}

crow::response WebSceneServer::serveFile(const std::filesystem::path& filePath, std::string_view contentType) const
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        return crow::response(404, "Web asset not found.");
    }

    const std::string contents{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    crow::response response(contents);
    response.set_header("Content-Type", std::string(contentType));

    return response;
}
}
