#include "networkclient.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/post.hpp>

static const int MAX_RETRY_ATTEMPTS = 3;
static const int RETRY_DELAY_SECONDS = 3;

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , workGuard(boost::asio::make_work_guard(ioContext))
    , socket(std::make_unique<boost::asio::ip::tcp::socket>(ioContext))
    , resolver(std::make_unique<boost::asio::ip::tcp::resolver>(ioContext))
    , connected(false)
    , retryCount(0)
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

    if (ioThread.joinable())
        ioThread.join();
}

void NetworkClient::startIoThread()
{
    ioThread = std::thread([this]() {
        ioContext.run();
    });
}

void NetworkClient::connectToServer(const std::string& host, unsigned short port)
{
    lastHost = host;
    lastPort = port;
    retryCount = 0;
    doConnect();
}

void NetworkClient::doConnect()
{
    boost::asio::post(ioContext, [this]() {
        emit statusChanged("Connecting...");

        resolver->async_resolve(
            lastHost,
            std::to_string(lastPort),
            [this](const boost::system::error_code& ec,
                   boost::asio::ip::tcp::resolver::results_type results)
            {
                if (ec) {
                    connected = false;
                    emit errorOccurred(QString("Resolve failed: %1")
                                           .arg(QString::fromStdString(ec.message())));
                    scheduleRetry();
                    return;
                }

                boost::asio::async_connect(
                    *socket, results,
                    [this](const boost::system::error_code& ec, const auto&)
                    {
                        if (ec) {
                            connected = false;
                            emit errorOccurred(QString("Connect failed: %1")
                                                   .arg(QString::fromStdString(ec.message())));
                            scheduleRetry();
                            return;
                        }

                        connected  = true;
                        retryCount = 0;
                        emit statusChanged("Connected");
                        startRead();
                    });
            });
    });
}

void NetworkClient::scheduleRetry()
{
    if (retryCount >= MAX_RETRY_ATTEMPTS) {
        emit statusChanged("Disconnected (max retries reached)");
        return;
    }

    ++retryCount;
    emit statusChanged(QString("Reconnecting... (attempt %1/%2)")
                           .arg(retryCount).arg(MAX_RETRY_ATTEMPTS));

    auto timer = std::make_shared<boost::asio::steady_timer>(
        ioContext, boost::asio::chrono::seconds(RETRY_DELAY_SECONDS));

    timer->async_wait([this, timer](const boost::system::error_code& ec) {
        if (!ec) {
            // Re-create socket before retrying
            socket = std::make_unique<boost::asio::ip::tcp::socket>(ioContext);
            doConnect();
        }
    });
}

void NetworkClient::startRead()
{
    socket->async_read_some(
        boost::asio::buffer(readBuffer),
        [this](const boost::system::error_code& ec, std::size_t bytesRead)
        {
            if (ec) {
                connected = false;
                emit errorOccurred(QString("Connection lost: %1")
                                       .arg(QString::fromStdString(ec.message())));
                emit statusChanged("Disconnected");
                scheduleRetry();
                return;
            }

            // Basic validation: response must be non-empty and start with '{'
            if (bytesRead > 0 && readBuffer[0] == '{') {
                // Valid JSON-looking response — continue normally
            } else if (bytesRead > 0) {
                emit errorOccurred("Received corrupted or unexpected message from server.");
            }

            startRead();
        });
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
                    scheduleRetry();
                    return;
                }

                emit statusChanged("Connected");
                emit messageSent();
            });
    });
}

bool NetworkClient::isConnected() const
{
    return connected;
}
