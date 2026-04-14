#pragma once

#include <boost/asio.hpp>
#include <vector>
#include "transaction.h"

class NetworkServer {
public:
    NetworkServer(boost::asio::io_context& io, unsigned short port);

private:
    void startAccept();
    void startRead();
    void handleMessage(const std::string& message);
    void sendResponse(const std::string& response);

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::vector<Transaction> transactions_;
};