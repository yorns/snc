#include "snc/client.h"
#include "stdlib.h"
#include <string>

int main(int argc, char* argv[]) {

    if (argc != 3 && argc != 4) {
        std::cerr << "usage "<<argv[0]<<" <receiver nick> <send data>\n";
        return -1;
    }
    boost::asio::io_service io_service;

    snc::Client client("generic", io_service, "127.0.0.1", 12001);

    std::string receiver_nick(argv[1]);
    std::string data(argv[2]);

    io_service.post(
                    [&client, &receiver_nick, data]() { client.send(snc::Client::SendType::cl_send, receiver_nick, data); });

    io_service.post([&](){client.stop();});

    io_service.run();

    return 0;
}

