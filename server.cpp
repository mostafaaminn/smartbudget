#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        readMessage();
    }

private:
    void readMessage()
    {
        auto self = shared_from_this();

        boost::asio::async_read_until(
            socket_,
            buffer_,
            '\n',
            [this, self](const boost::system::error_code& ec, std::size_t)
            {
                if (ec) {
                    std::cout << "Client disconnected or read error: " << ec.message() << std::endl;
                    return;
                }

                std::istream input(&buffer_);
                std::string line;
                std::getline(input, line);

                std::cout << "Received JSON message: " << line << std::endl;

                readMessage();
            }
            );
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
};

class Server
{
public:
    Server(boost::asio::io_context& ioContext, unsigned short port)
        : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port))
    {
        acceptConnection();
    }

private:
    void acceptConnection()
    {
        acceptor_.async_accept(
            [this](const boost::system::error_code& ec, tcp::socket socket)
            {
                if (!ec) {
                    std::cout << "Client connected." << std::endl;
                    std::make_shared<Session>(std::move(socket))->start();
                } else {
                    std::cout << "Accept error: " << ec.message() << std::endl;
                }

                acceptConnection();
            }
            );
    }

    tcp::acceptor acceptor_;
};

int main()
{
    try {
        boost::asio::io_context ioContext;
        Server server(ioContext, 12345);

        std::cout << "Server running on port 12345..." << std::endl;
        ioContext.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}
