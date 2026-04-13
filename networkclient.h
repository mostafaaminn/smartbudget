#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QString>
#include <thread>
#include <memory>
#include <array>

#include <boost/asio.hpp>

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    void connectToServer(const std::string& host, unsigned short port);
    void sendMessage(const std::string& message);
    bool isConnected() const;

signals:
    void statusChanged(const QString& status);
    void errorOccurred(const QString& error);
    void messageSent();

private:
    void startIoThread();
    void startRead();

    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
    std::thread ioThread;
    bool connected;
    std::array<char, 1024> readBuffer;
};

#endif
