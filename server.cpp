#include <iostream>
#include <boost/asio.hpp>
#include "networkserver.h"
int main()
{
    try {
        boost::asio::io_context ioContext;
        NetworkServer server(ioContext, 12345);

        std::cout << "Server running on port 12345..." << std::endl;
        ioContext.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}
