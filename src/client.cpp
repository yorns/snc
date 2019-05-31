#include "client.h"
#include "commandNames.h"

using namespace snc;

void Client::set_async_receive() {
    m_socket.async_receive_from(
            boost::asio::buffer(m_inBuffer), ep, [this](const boost::system::error_code& error,
                                                        std::size_t bytes_transferred )
            {
                receiveHandler(error, bytes_transferred);
            });
}

std::string Client::getNick(const std::string line, const std::string &prefix) {
    return line.substr(prefix.size()+1, (line.find_first_of(' ', prefix.size()+1)) - (prefix.size()+1));
}

void Client::receiveHandler(const boost::system::error_code &error, size_t bytes_recvd) {

    if (m_stopped)
        return;

    if (!error && bytes_recvd>0) {
        std::string receivedData(&m_inBuffer[0], bytes_recvd);

        if (receivedData.substr(0, CommandName::cmdNewone.size()) == CommandName::cmdNewone && m_newNameHandler)
            m_newNameHandler( receivedData.substr(CommandName::cmdNewone.size()+1) );

        if (receivedData.substr(0, CommandName::cmdMsg.size()) == CommandName::cmdMsg && m_receiveHandler) {
            std::string nick = getNick(receivedData, CommandName::cmdMsg);
            m_receiveHandler( nick, receivedData.substr(CommandName::cmdMsg.size() + nick.size() + 2 /* separators */));
        }

        if (receivedData.substr(0, CommandName::cmdBroadcast.size()) == CommandName::cmdBroadcast && m_broadcastHandler) {
            std::string nick = getNick(receivedData, CommandName::cmdBroadcast);
            m_broadcastHandler( nick, receivedData.substr(CommandName::cmdBroadcast.size() + nick.size() + 2 /* separators */));
        }
    }
    set_async_receive();
}


void Client::send(Client::SendType type, const std::string &nick, const std::string &data) {
    if (!m_stopped) {
        switch (type) {
            case Client::SendType::cl_send:
            m_socket.send_to(boost::asio::buffer(CommandName::cmdSend + " " + nick + " " + data), m_server_endpoint);
                break;
            case Client::SendType::cl_register:
                m_socket.send_to(boost::asio::buffer(CommandName::cmdRegister + " " + nick), m_server_endpoint);
                break;
            case Client::SendType::cl_broadcast:
                m_socket.send_to(boost::asio::buffer(CommandName::cmdBroadcast + " " + data), m_server_endpoint);
                break;
        }
    }
}

void Client::recvHandler(const DoubleStringCall &receiveHandler) { m_receiveHandler = receiveHandler; }

void Client::broadcastHandler(const DoubleStringCall &broadcastHandler) { m_broadcastHandler = broadcastHandler; }

void Client::newPartnerHandler(const StringCall &newNameHandler) { m_newNameHandler = newNameHandler; }

void Client::stop() { m_stopped = true; m_socket.close(); }

Client::~Client() { std::cout << "Client destructor\n"; }

