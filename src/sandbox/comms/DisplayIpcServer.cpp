#include "sandbox/comms/DisplayIpcServer.hpp"

#include <string>
#include <iostream>
#include <filesystem>

namespace sandbox::comms
{

DisplayIpcServer::DisplayIpcServer(std::string ipcEndpoint) : 
    mContext(1), 
    mSocket(mContext, zmq::socket_type::pull)
{
    std::cout << "endpoint: " << ipcEndpoint.data() << std::endl;
    std::filesystem::remove("/tmp/led-display.sock");
    mSocket.bind(ipcEndpoint.data());
    std::cout << "after endpoint: " << ipcEndpoint.data() << std::endl;
}

bool DisplayIpcServer::tryReceive(std::string& json_text)
{
    zmq::message_t message;

    const auto received = mSocket.recv(message, zmq::recv_flags::dontwait);

    if (!received.has_value())
    {
        return false;
    }

    json_text.assign(static_cast<const char*>(message.data()), message.size());

    std::cout << "Received: " << json_text << '\n';
    return true;
}
}