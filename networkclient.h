#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QString>
#include <thread>
#include <memory>
#include <string>
#include <atomic>

#include <boost/asio.hpp>

#include "Inetworkclient.h"

class NetworkClient : public QObject, public INetworkClient
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    void connectToServer(const std::string& host, unsigned short port) override;
    void sendMessage(const std::string& message) override;
    bool isConnected() const override;

signals:
    void statusChanged(const QString& status);
    void errorOccurred(const QString& error);
    void messageSent();
    void messageReceived(const QString& message);

private:
    void startIoThread();
    void startRead();
    void startTimeoutTimer();
    void stopTimeoutTimer();

    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;

    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
    std::unique_ptr<boost::asio::steady_timer> timeoutTimer;

    std::thread ioThread;
    std::atomic<bool> connected;
    boost::asio::streambuf readBuffer;
};

#endif
