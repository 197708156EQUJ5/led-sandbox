#pragma once

#include <string>

#include <zmq.hpp>

namespace sandbox::comms
{
class DisplayIpcServer
{
public:
    DisplayIpcServer(std::string ipcEndpoint);

    bool tryReceive(std::string& jsonText);

private:
    zmq::context_t mContext;
    zmq::socket_t mSocket;
};
}