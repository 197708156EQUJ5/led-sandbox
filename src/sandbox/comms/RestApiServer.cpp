#include "sandbox/comms/RestApiServer.hpp"

#include <nlohmann/json.hpp>

#include <utility>

using json = nlohmann::json;

namespace sandbox::comms
{

RestApiServer::RestApiServer(uint16_t port) :
    mPort(port)
{
    CROW_ROUTE(mApp, "/api/scene")
        .methods(crow::HTTPMethod::Post)
    (
        [this](const crow::request& request)
        {
            return publishScene(request);
        }
    );

    CROW_ROUTE(mApp, "/api/clear")
        .methods(crow::HTTPMethod::Post)
    (
        [this](const crow::request&)
        {
            return clearDisplay();
        }
    );
}

RestApiServer::~RestApiServer()
{
    stop();
}

void RestApiServer::start()
{
    mServerThread = std::thread(
        [this]()
        {
            mApp.bindaddr("0.0.0.0")
                .port(mPort)
                .multithreaded()
                .run();
        }
    );
}

void RestApiServer::stop()
{
    mApp.stop();

    if (mServerThread.joinable())
    {
        mServerThread.join();
    }
}

bool RestApiServer::tryReceive(std::string& jsonText)
{
    std::lock_guard<std::mutex> lock(mQueueMutex);

    if (mPendingJson.empty())
    {
        return false;
    }

    jsonText = std::move(mPendingJson.front());
    mPendingJson.pop_front();

    return true;
}

crow::response RestApiServer::publishScene(const crow::request& request)
{
    json requestJson;

    try
    {
        requestJson = json::parse(request.body);
    }
    catch (const json::parse_error&)
    {
        return crow::response(400, R"({"error":"Request body must contain valid JSON."})");
    }

    if (!requestJson.is_object() || !requestJson.contains("objects") || !requestJson["objects"].is_array())
    {
        return crow::response(400, R"({"error":"JSON must contain an objects array."})");
    }

    const std::size_t object_count = requestJson["objects"].size();

    enqueue(request.body);

    json responseJson = {
        {"status", "published"}, 
        {"objectCount", object_count}
    };

    return crow::response(200, responseJson.dump());
}

crow::response RestApiServer::clearDisplay()
{
    enqueue(R"({"objects":[]})");

    return crow::response(200, R"({"status":"cleared"})");
}

void RestApiServer::enqueue(std::string jsonText)
{
    std::lock_guard<std::mutex> lock(mQueueMutex);

    mPendingJson.push_back(std::move(jsonText));
}

} // namespace sandbox::comms