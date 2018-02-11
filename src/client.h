#ifndef SNC_CLIENT_H
#define SNC_CLIENT_H

#include <iostream>
//#include <chrono>
#include <thread>
#include <array>
#include <functional>
#include <boost/asio.hpp>

typedef std::function<void(const std::string&)> StringCall;
typedef std::function<void(const std::string&, const std::string& )> DoubleStringCall;

using boost::asio::ip::udp;

class Client {

    boost::asio::io_service& m_service;
    udp::socket m_socket;
    udp::endpoint m_server_endpoint;
    std::array<char, 255> m_inBuffer;
    DoubleStringCall m_receiveHandler;
    StringCall m_newNameHandler;
    bool m_stopped;
    udp::endpoint ep;

    void set_async_receive();

    void receiveHandler(const boost::system::error_code& error,
                        size_t bytes_recvd);

public:
    Client() = delete;
    Client(const std::string& name, boost::asio::io_service& service, const std::string& serverIp, uint16_t port);

    ~Client();

    enum class SendType {
        cl_send,
        cl_broadcast,
        cl_register
    };

    void send(SendType type, const std::string& nick, const std::string& data = std::string());

    void recvHandler(const DoubleStringCall& receiveHandler);
    void newPartnerHandler(const StringCall& newNameHandler);

    void stop();

};



#endif //SNC_CLIENT_H