#ifndef INETWORKCLIENT_H
#define INETWORKCLIENT_H


#include <string>

class INetworkClient
{
public:
    virtual ~INetworkClient() = default;

    virtual void connectToServer(const std::string& host, unsigned short port) = 0;
    virtual void sendMessage(const std::string& message) = 0;
    virtual bool isConnected() const = 0;
};

#endif // INETWORKCLIENT_H
