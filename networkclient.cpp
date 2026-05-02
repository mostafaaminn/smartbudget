#include "networkclient.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/post.hpp>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent),
    workGuard(boost::asio::make_work_guard(ioContext)),
    socket(std::make_unique<boost::asio::ip::tcp::socket>(ioContext)),
    resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext)),
    connected(false)
{
    startIoThread();
}

NetworkClient::~NetworkClient()
{
    connected = false;

    boost::asio::post(ioContext, [this]() {
        if (socket && socket->is_open()) {
            boost::system::error_code ec;
            socket->close(ec);
        }
    });

    workGuard.reset();
    ioContext.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }
}

void NetworkClient::startIoThread()
{
    ioThread = std::thread([this]() {
        ioContext.run();
    });
}

void NetworkClient::connectToServer(const std::string& host, unsigned short port)
{
    boost::asio::post(ioContext, [this, host, port]() {
        emit statusChanged("Connecting...");

        resolver->async_resolve(
            host,
            std::to_string(port),
            [this](const boost::system::error_code& ec,
                   boost::asio::ip::tcp::resolver::results_type results)
            {
                if (ec) {
                    connected = false;
                    emit errorOccurred(QString("Resolve failed: %1")
                                           .arg(QString::fromStdString(ec.message())));
                    emit statusChanged("Disconnected");
                    return;
                }

                boost::asio::async_connect(
                    *socket,
                    results,
                    [this](const boost::system::error_code& ec, const auto&)
                    {
                        if (ec) {
                            connected = false;
                            emit errorOccurred(QString("Connect failed: %1")
                                                   .arg(QString::fromStdString(ec.message())));
                            emit statusChanged("Disconnected");
                            return;
                        }

                        connected = true;
                        emit statusChanged("Connected");
                        startRead();
                    }
                    );
            }
            );
    });
}

void NetworkClient::startRead()
{
    socket->async_read_some(
        boost::asio::buffer(readBuffer),
        [this](const boost::system::error_code& ec, std::size_t)
        {
            if (ec) {
                connected = false;
                emit errorOccurred(QString("Connection lost: %1")
                                       .arg(QString::fromStdString(ec.message())));
                emit statusChanged("Disconnected");
                return;
            }

            startRead();
        }
        );
}

void NetworkClient::sendMessage(const std::string& message)
{
    boost::asio::post(ioContext, [this, message]() {
        if (!connected || !socket || !socket->is_open()) {
            emit statusChanged("Disconnected");
            return;
        }

        auto buffer = std::make_shared<std::string>(message);

        boost::asio::async_write(
            *socket,
            boost::asio::buffer(*buffer),
            [this, buffer](const boost::system::error_code& ec, std::size_t)
            {
                if (ec) {
                    connected = false;
                    emit errorOccurred(QString("Send failed: %1")
                                           .arg(QString::fromStdString(ec.message())));
                    emit statusChanged("Disconnected");
                    return;
                }

                emit statusChanged("Connected");
                emit messageSent();
            }
            );
    });
}

bool NetworkClient::isConnected() const
{
    return connected;
}
