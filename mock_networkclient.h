#pragma once
#include "INetworkClient.h"
#include <gmock/gmock.h>
#include <string>

class MockNetworkClient : public INetworkClient
{
public:
    MOCK_METHOD(void, connectToServer,
                (const std::string& host, unsigned short port), (override));
    MOCK_METHOD(void, sendMessage,
                (const std::string& message), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
};
