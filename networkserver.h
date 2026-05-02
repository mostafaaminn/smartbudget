#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>

#include "transaction.h"

class NetworkServer {
public:
    NetworkServer(boost::asio::io_context& io, unsigned short port);

private:
    class ClientSession;
    friend class ClientSession;

    void startAccept();

    boost::asio::io_context& io_;
    boost::asio::ip::tcp::acceptor acceptor_;

    std::vector<Transaction> transactions_;
};
