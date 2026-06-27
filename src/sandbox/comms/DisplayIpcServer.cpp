#include "sandbox/comms/DisplayIpcServer.hpp"

#include <sys/stat.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <stdexcept>

namespace sandbox::comms
{

DisplayIpcServer::DisplayIpcServer(std::string ipcEndpoint) : 
    mContext(1), 
    mSocket(mContext, zmq::socket_type::pull)
{
    std::cout << "endpoint: " << ipcEndpoint.data() << std::endl;
     mSocket.bind(ipcEndpoint);

    const std::string socket_path = ipcEndpoint.substr(std::string("ipc://").size());

    if (chmod(socket_path.c_str(), 0666) != 0)
    {
        throw std::runtime_error("Could not set IPC socket permissions.");
    }
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