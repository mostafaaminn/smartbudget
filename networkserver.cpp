#include "NetworkServer.h"
#include "JsonTools.h"
#include <iostream>

using boost::asio::ip::tcp;

NetworkServer::NetworkServer(boost::asio::io_context& io, unsigned short port)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)),
    socket_(io) {
    startAccept();
}

void NetworkServer::startAccept() {
    acceptor_.async_accept(socket_, [this](const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "Client connected.\n";
            startRead();
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }
    });
}

void NetworkServer::startRead() {
    boost::asio::async_read_until(socket_, buffer_, '\n',
                                  [this](const boost::system::error_code& ec, std::size_t) {
                                      if (ec) {
                                          std::cerr << "Read error: " << ec.message() << "\n";
                                          return;
                                      }

                                      std::istream input(&buffer_);
                                      std::string message;
                                      std::getline(input, message);

                                      handleMessage(message);
                                      startRead();
                                  });
}

void NetworkServer::handleMessage(const std::string& message) {
    if (message.find("\"action\":\"add_transaction\"") != std::string::npos) {
        size_t pos = message.find("\"transaction\":");
        if (pos == std::string::npos) {
            sendResponse(JsonTools::errorResponse("Missing transaction"));
            return;
        }

        std::string txJson = message.substr(pos + 14);
        if (!txJson.empty() && txJson.back() == '}') {
        }

        Transaction tx(0.0, "", "", QDate::currentDate());
        if (!JsonTools::fromJson(txJson, tx)) {
            sendResponse(JsonTools::errorResponse("Invalid transaction JSON"));
            return;
        }

        transactions_.push_back(tx);
        sendResponse(JsonTools::okResponse("Transaction stored"));
    }
    else if (message.find("\"action\":\"get_all\"") != std::string::npos) {
        sendResponse(JsonTools::transactionsResponse(transactions_));
    }
    else {
        sendResponse(JsonTools::errorResponse("Unknown action"));
    }
}

void NetworkServer::sendResponse(const std::string& response) {
    boost::asio::async_write(socket_, boost::asio::buffer(response),
                             [this](const boost::system::error_code& ec, std::size_t) {
                                 if (ec) {
                                     std::cerr << "Write error: " << ec.message() << "\n";
                                 }
                             });
}