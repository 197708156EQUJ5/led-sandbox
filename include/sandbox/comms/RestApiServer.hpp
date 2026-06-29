#pragma once

#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include <crow.h>

namespace sandbox::comms
{

class RestApiServer
{
public:
    explicit RestApiServer(uint16_t port);

    ~RestApiServer();

    RestApiServer(const RestApiServer&) = delete;
    RestApiServer& operator=(const RestApiServer&) = delete;

    void start();

    void stop();

    bool tryReceive(std::string& jsonText);

private:
    crow::response publishScene(const crow::request& request);

    crow::response clearDisplay();

    void enqueue(std::string jsonText);

    uint16_t mPort;
    crow::SimpleApp mApp;
    std::thread mServerThread;

    std::mutex mQueueMutex;
    std::deque<std::string> mPendingJson;
};

} // namespace sandbox::comms