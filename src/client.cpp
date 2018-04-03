#include "client.h"

using namespace snc;

void Client::set_async_receive() {
    m_socket.async_receive_from(
            boost::asio::buffer(m_inBuffer), ep, [this](const boost::system::error_code& error,
                                                        std::size_t bytes_transferred )
            {
                receiveHandler(error, bytes_transferred);
            });
}

void Client::receiveHandler(const boost::system::error_code &error, size_t bytes_recvd) {

    if (m_stopped)
        return;

    const size_t size_newOne { std::string("newOne").size() };
    const size_t size_msg { std::string("msg").size() };
    const size_t size_broadcast { std::string("broadcast").size() };

    if (!error && bytes_recvd>0) {
        std::string receivedData(&m_inBuffer[0], bytes_recvd);
//        std::cerr << "-- received: "<<receivedData<<"\n";
        if (receivedData.substr(0, size_newOne) == "newOne" && m_newNameHandler)
            m_newNameHandler( receivedData.substr(size_newOne+1) );

        if (receivedData.substr(0, size_msg) == "msg" && m_receiveHandler) {
            std::size_t end;
            std::string nick = receivedData.substr(4, (end = receivedData.find_first_of(' ',size_msg+1))-(size_msg+1));
            m_receiveHandler( nick, receivedData.substr(end+1));
        }

        if (receivedData.substr(0, size_broadcast) == "broadcast" && m_broadcastHandler) {
            std::size_t end;
            std::string nick = receivedData.substr(size_broadcast+1, (end = receivedData.find_first_of(' ',size_broadcast+1))-(size_broadcast+1));
            m_broadcastHandler( nick, receivedData.substr(end+1));
        }
    }
    set_async_receive();
}


void Client::send(Client::SendType type, const std::string &nick, const std::string &data) {
    if (!m_stopped) {
        switch (type) {
            case Client::SendType::cl_send:
            m_socket.send_to(boost::asio::buffer("send " + nick + " " + data), m_server_endpoint);
                break;
            case Client::SendType::cl_register:
                m_socket.send_to(boost::asio::buffer("register " + nick ), m_server_endpoint);
                break;
            case Client::SendType::cl_broadcast:
                m_socket.send_to(boost::asio::buffer("broadcast " + data), m_server_endpoint);
                break;
        }
    }
}

void Client::recvHandler(const DoubleStringCall &receiveHandler) { m_receiveHandler = receiveHandler; }

void Client::broadcastHandler(const DoubleStringCall &broadcastHandler) { m_broadcastHandler = broadcastHandler; }

void Client::newPartnerHandler(const StringCall &newNameHandler) { m_newNameHandler = newNameHandler; }

void Client::stop() { m_stopped = true; m_socket.close(); }

Client::~Client() { std::cout << "Client destructor\n"; }
