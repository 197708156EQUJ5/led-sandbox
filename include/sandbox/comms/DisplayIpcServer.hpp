#pragma once

#include <string>

#include <zmq.hpp>

namespace sandbox::comms
{
class DisplayIpcServer
{
public:
    DisplayIpcServer(std::string ipcEndpoint);
    ~DisplayIpcServer();

    void stop();
    bool tryReceive(std::string& jsonText);

private:
    std::string mIpcEndpoint;
    zmq::context_t mContext;
    zmq::socket_t mSocket;
};
}