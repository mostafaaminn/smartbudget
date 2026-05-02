#pragma once

#include <string>

class INetworkClient
{
public:
    virtual ~INetworkClient() = default;

    virtual void connectToServer(const std::string& host, unsigned short port) = 0;
    virtual void sendMessage(const std::string& message) = 0;
    virtual bool isConnected() const = 0;
};