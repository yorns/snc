#ifndef SNC_CLIENT_H
#define SNC_CLIENT_H

#include <iostream>
#include <thread>
#include <array>
#include <functional>
#include <boost/asio.hpp>
#include "config.h"

using boost::asio::ip::udp;

namespace snc {

    typedef std::function<void(const std::string&)> StringCall;
    typedef std::function<void(const std::string&, const std::string& )> DoubleStringCall;

    class Client {

        boost::asio::io_service &m_service;
        udp::socket m_socket;
        udp::endpoint m_server_endpoint;
        std::array<char, snc::config::maxMsgLength> m_inBuffer;
        DoubleStringCall m_receiveHandler;
        DoubleStringCall m_broadcastHandler;
        StringCall m_newNameHandler;
        bool m_stopped;
        udp::endpoint ep;

        void set_async_receive();

        void receiveHandler(const boost::system::error_code &error,
                            size_t bytes_recvd);

        std::string getNick(const std::string line, const std::string& prefix);

    public:
        Client() = delete;

        Client(const std::string &name, boost::asio::io_service &service, const std::string &serverIp,
                       uint16_t port);

        ~Client();

        enum class SendType {
            cl_send,
            cl_broadcast,
            cl_register
        };

        void send(SendType type, const std::string &nick, const std::string &data = std::string());

        void recvHandler(const DoubleStringCall &receiveHandler);

        void newPartnerHandler(const StringCall &newNameHandler);

        void broadcastHandler(const DoubleStringCall &broadcastHandler);

        void stop();

    };

}

#endif //SNC_CLIENT_H
