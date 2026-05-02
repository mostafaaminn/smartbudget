
#include "networkserver.h"
#include "jsontools.h"

#include <iostream>
#include <istream>
#include <memory>
#include <chrono>

#include <QDate>

using boost::asio::ip::tcp;

class NetworkServer::ClientSession
    : public std::enable_shared_from_this<NetworkServer::ClientSession>
{
public:
    ClientSession(tcp::socket socket, NetworkServer& server)
        : socket_(std::move(socket)),
        server_(server),
        timeoutTimer_(socket_.get_executor())
    {
    }

    void start()
    {
        std::cout << "Client connected.\n";
        startRead();
    }

private:
    void startRead()
    {
        startTimeout();

        auto self = shared_from_this();

        boost::asio::async_read_until(
            socket_,
            buffer_,
            '\n',
            [this, self](const boost::system::error_code& ec, std::size_t)
            {
                stopTimeout();

                if (ec) {
                    std::cerr << "Read error: " << ec.message() << "\n";
                    close();
                    return;
                }

                std::istream input(&buffer_);
                std::string message;
                std::getline(input, message);

                handleMessage(message);
                startRead();
            });
    }

    void handleMessage(const std::string& message)
    {
        if (!JsonTools::isValidJson(message)) {
            sendResponse(JsonTools::errorResponse("Invalid or corrupted message"));
            return;
        }

        std::string action = JsonTools::extractAction(message);

        if (action == "add_transaction") {
            Transaction tx(0.0, "", "", QDate::currentDate(), "EGP");
            if (!JsonTools::extractTransaction(message, tx)) {
                sendResponse(JsonTools::errorResponse("Invalid transaction JSON"));
                return;
            }

            server_.transactions_.push_back(tx);
            sendResponse(JsonTools::okResponse("Transaction stored"));
        }
        else if (action == "get_all") {
            sendResponse(JsonTools::transactionsResponse(server_.transactions_));
        }
        else {
            sendResponse(JsonTools::errorResponse("Unknown action"));
        }
    }

    void sendResponse(const std::string& response)
    {
        auto self = shared_from_this();
        auto buffer = std::make_shared<std::string>(response);

        boost::asio::async_write(
            socket_,
            boost::asio::buffer(*buffer),
            [this, self, buffer](const boost::system::error_code& ec, std::size_t)
            {
                if (ec) {
                    std::cerr << "Write error: " << ec.message() << "\n";
                    close();
                }
            });
    }

    void startTimeout()
    {
        timeoutTimer_.expires_after(std::chrono::seconds(30));

        auto self = shared_from_this();

        timeoutTimer_.async_wait(
            [this, self](const boost::system::error_code& ec)
            {
                if (!ec) {
                    std::cerr << "Client timeout.\n";
                    close();
                }
            });
    }

    void stopTimeout()
    {
        boost::system::error_code ec;
        timeoutTimer_.cancel();    }

    void close()
    {
        boost::system::error_code ec;

        timeoutTimer_.cancel();

        if (socket_.is_open()) {
            socket_.shutdown(tcp::socket::shutdown_both, ec);
            socket_.close(ec);
        }
    }

private:
    tcp::socket socket_;
    NetworkServer& server_;
    boost::asio::streambuf buffer_;
    boost::asio::steady_timer timeoutTimer_;
};

NetworkServer::NetworkServer(boost::asio::io_context& io, unsigned short port)
    : io_(io),
    acceptor_(io, tcp::endpoint(tcp::v4(), port))
{
    startAccept();
}

void NetworkServer::startAccept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code& ec, tcp::socket socket)
        {
            if (!ec) {
                std::make_shared<ClientSession>(
                    std::move(socket),
                    *this
                    )->start();
            }
            else {
                std::cerr << "Accept error: " << ec.message() << "\n";
            }

            startAccept();
        });
}
