#include "networkclient.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/post.hpp>

#include <istream>
#include <chrono>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent),
    workGuard(boost::asio::make_work_guard(ioContext)),
    socket(std::make_unique<boost::asio::ip::tcp::socket>(ioContext)),
    resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext)),
    timeoutTimer(std::make_unique<boost::asio::steady_timer>(ioContext)),
    connected(false)
{
    startIoThread();
}

NetworkClient::~NetworkClient()
{
    connected = false;

    boost::asio::post(ioContext, [this]() {
        boost::system::error_code ec;

        if (timeoutTimer) {
            timeoutTimer->cancel();        }

        if (resolver) {
            resolver->cancel();
        }

        if (socket && socket->is_open()) {
            socket->close(ec);
        }
    });

    workGuard.reset();

    if (ioThread.joinable()) {
        ioThread.join();
    }

    ioContext.stop();
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

        startTimeoutTimer();

        resolver->async_resolve(
            host,
            std::to_string(port),
            [this](const boost::system::error_code& ec,
                   boost::asio::ip::tcp::resolver::results_type results)
            {
                if (ec) {
                    stopTimeoutTimer();
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
                        stopTimeoutTimer();

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
                    });
            });
    });
}

void NetworkClient::startRead()
{
    boost::asio::async_read_until(
        *socket,
        readBuffer,
        '\n',
        [this](const boost::system::error_code& ec, std::size_t)
        {
            if (ec) {
                connected = false;

                emit errorOccurred(QString("Connection lost: %1")
                                       .arg(QString::fromStdString(ec.message())));
                emit statusChanged("Disconnected");
                return;
            }

            std::istream input(&readBuffer);
            std::string message;
            std::getline(input, message);

            emit messageReceived(QString::fromStdString(message));

            startRead();
        });
}

void NetworkClient::sendMessage(const std::string& message)
{
    boost::asio::post(ioContext, [this, message]() {
        if (!connected || !socket || !socket->is_open()) {
            emit errorOccurred("Not connected to server.");
            emit statusChanged("Disconnected");
            return;
        }

        std::string finalMessage = message;

        if (finalMessage.empty() || finalMessage.back() != '\n') {
            finalMessage += '\n';
        }

        auto buffer = std::make_shared<std::string>(finalMessage);

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

                emit messageSent();
            });
    });
}

void NetworkClient::startTimeoutTimer()
{
    if (!timeoutTimer) {
        return;
    }

    timeoutTimer->expires_after(std::chrono::seconds(5));

    timeoutTimer->async_wait([this](const boost::system::error_code& ec) {
        if (!ec && !connected) {
            boost::system::error_code closeEc;

            if (resolver) {
                resolver->cancel();
            }

            if (socket && socket->is_open()) {
                socket->close(closeEc);
            }

            emit errorOccurred("Network timeout.");
            emit statusChanged("Disconnected");
        }
    });
}

void NetworkClient::stopTimeoutTimer()
{
    if (!timeoutTimer) {
        return;
    }

    boost::system::error_code ec;
    timeoutTimer->cancel();}

bool NetworkClient::isConnected() const
{
    return connected;
}
