#ifndef MOCK_NETWORKCLIENT_H
#define MOCK_NETWORKCLIENT_H


#include <gmock/gmock.h>
#include "INetworkClient.h"

class MockNetworkClient : public INetworkClient
{
public:
    MOCK_METHOD(void, connectToServer,
                (const std::string& host, unsigned short port), (override));
    MOCK_METHOD(void, sendMessage,
                (const std::string& message), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
};

#endif // MOCK_NETWORKCLIENT_H
