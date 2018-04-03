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

namespace snc {

    class Client {

        boost::asio::io_service &m_service;
        udp::socket m_socket;
        udp::endpoint m_server_endpoint;
        std::array<char, 255> m_inBuffer;
        DoubleStringCall m_receiveHandler;
        DoubleStringCall m_broadcastHandler;
        StringCall m_newNameHandler;
        bool m_stopped;
        udp::endpoint ep;

        void set_async_receive();

        void receiveHandler(const boost::system::error_code &error,
                            size_t bytes_recvd);

    public:
        Client() = delete;

        Client(const std::string &name, boost::asio::io_service &service, const std::string &serverIp,
                       uint16_t port) :
                m_service(service), m_socket(m_service, udp::endpoint(udp::v4(), 0 /* take random port */)),
                m_server_endpoint(boost::asio::ip::address::from_string(serverIp), port),
                m_stopped(false)
        {
            send(Client::SendType::cl_register, name);
            set_async_receive();
        }

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
