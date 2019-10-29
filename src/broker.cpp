#include <iostream>
#include <array>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>

#include "../system/SystemdIface.h"
#include "commandNames.h"

#define dbg std::cerr

using boost::asio::ip::udp;

class ClientSet {
public:
  udp::endpoint m_clientEndpoint;
  std::string m_nickname;
  bool m_online;
  ClientSet(const std::string& name, udp::endpoint& clientEndpoint)
  : m_clientEndpoint(clientEndpoint), m_nickname(name), m_online(true) {}
};

class Broker {

private:
    boost::asio::io_service &m_io_service;
    udp::socket m_socket;
    std::array<char, 255> m_buffer;
    udp::endpoint m_sender_endpoint;
    std::vector<ClientSet> clientList;
    SystemdIface systemdIface;
    boost::asio::deadline_timer watchdogTimer;
    std::chrono::microseconds watchdogDuration;


    std::string getNickname(const udp::endpoint &sender) {
        std::string nickname("unknown");
        for (auto client : clientList)
            if (client.m_clientEndpoint == sender)
                nickname = client.m_nickname;
        return nickname;
    }

    udp::endpoint getEndpoint(const std::string &nickname) {
        udp::endpoint ep;
        for (auto client : clientList)
            if (client.m_nickname == nickname)
                ep = client.m_clientEndpoint;
        return ep;
    }

    bool startMessageWith(const std::string &inString, const std::string &command) {
        return inString.length() > command.size() + 2 /* space + at least one char for next info */ &&
               inString.substr(0, command.length()) == command;
    }

    void set_async_receive() {
        m_socket.async_receive_from(
                boost::asio::buffer(m_buffer), m_sender_endpoint,
                [this](const boost::system::error_code &error, size_t bytes_recvd) {
                    receiveHandler(error, bytes_recvd);
                });
    }

    void receiveHandler(const boost::system::error_code &error,
                        size_t bytes_recvd) {

        if (error) {
            dbg << "stopping broker\n";
            return;
        }

        if (bytes_recvd > 0) {
            std::string data(m_buffer.data(), bytes_recvd);
            dbg << "Message received: " << data << std::endl;

            if (startMessageWith(data, CommandName::cmdRegister))
                do_register(data.substr(CommandName::cmdRegister.length() + 1));

            if (startMessageWith(data, CommandName::cmdBroadcast))
                do_broadcast(data.substr(CommandName::cmdBroadcast.length() + 1));

            if (startMessageWith(data, CommandName::cmdSend))
                do_send(data.substr(CommandName::cmdSend.length() + 1));
        } else
            std::cout << "error - no usable data received\n";
        set_async_receive();
    }

    void do_register(const std::string &nickname) {
        std::cout << "new client registered <" << nickname << ">\n";

        std::string ownName(CommandName::cmdNewone + " " + nickname);

        auto knownClient = std::find_if(clientList.begin(), clientList.end(),
                         [&nickname](const ClientSet& clientSet){ return clientSet.m_nickname == nickname;});
        if (knownClient != clientList.end()) {

            for (auto client : clientList) {
                std::string msg(CommandName::cmdNewone + " " + client.m_nickname);
                m_socket.send_to(boost::asio::buffer(msg), m_sender_endpoint);
            }

            knownClient->m_clientEndpoint = m_sender_endpoint;
            knownClient->m_online = true;
        }
        else {
            // informing all other clients about the new one and send available clients to new one
            for (auto client : clientList) {
                std::string msg(CommandName::cmdNewone + " " + client.m_nickname);
                m_socket.send_to(boost::asio::buffer(msg), m_sender_endpoint);
                m_socket.send_to(boost::asio::buffer(ownName), client.m_clientEndpoint);
            }

            // keep client in our database
            clientList.emplace_back(ClientSet(nickname, m_sender_endpoint));
        }
    }

    void do_broadcast(const std::string &message) {
        std::cout << "new broadcast message <" << message << ">\n";

        std::string msg(CommandName::cmdBroadcast + " " + getNickname(m_sender_endpoint) + " " + message);

        std::for_each(std::begin(clientList), std::end(clientList),
                      [&](ClientSet &cs) {
                          if (m_sender_endpoint != cs.m_clientEndpoint)
                              m_socket.send_to(boost::asio::buffer(msg), cs.m_clientEndpoint);
                      });
    }

    void do_send(const std::string &remainder) {
        std::string::size_type nickEnd = remainder.find_first_of(' ');
        std::string toNickName = remainder.substr(0, nickEnd);
        std::string message = CommandName::cmdMsg + " " + getNickname(m_sender_endpoint) + " " + remainder.substr(nickEnd + 1);

        udp::endpoint ep = getEndpoint(toNickName);

        if (ep.port() == 0)
            std::cout << "no client found for nick <" << toNickName << ">\n";
        else
            m_socket.send_to(boost::asio::buffer(message), ep);
    }

    void sendWatchdogTrigger() {
        systemdIface.notifyWatchdog();

        watchdogTimer.expires_from_now(watchdogDuration);
        watchdogTimer.async_wait([this](const boost::system::error_code& error ) {
            if (!error)
                sendWatchdogTrigger();
        });
    }

public:
    Broker(boost::asio::io_service &service, uint16_t port = 12001)
            : m_io_service(service), m_socket(m_io_service, udp::endpoint(udp::v4() /* any UDP */, port)),
    watchdogTimer(service){

        watchdogDuration = std::chrono::microseconds(systemdIface.getInterval()/2);
        sendWatchdogTrigger();

        // now, we need to listen to any IP, and react on incoming data
        set_async_receive();
        systemdIface.notifyReady();
    }

    void stop() {
        m_socket.close();
        watchdogTimer.cancel();
    }

};

int main() {

    boost::asio::io_service io_service;
    boost::asio::signal_set signalSet(io_service, SIGINT, SIGTERM); // on kill signal, remove interfaces,
                                                                    // to let io_service finish

    Broker server(io_service);

    signalSet.async_wait([&server](const boost::system::error_code &ec, int signal) {
        if (!ec) {
            std::cerr << "signal id <"<<signal<<"> called\n";
          server.stop();
      }
    });

    io_service.run();

    return 0;
}

