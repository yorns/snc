#include "client.h"
#include "CommandLine.h"
#include "KeyHit.h"

int main(int argc, char* argv[]) {

    boost::asio::io_service io_service;

    snc::Client client(argv[1], io_service, argv[2], 12001);
    CommandLine cmdLine(io_service);
    KeyHit keyHit;

    keyHit.setKeyReceiver([&io_service,&cmdLine](const char& key){ io_service.post([&cmdLine, &key](){cmdLine.keyInputExternal(key); });});
    cmdLine.setStop([&keyHit, &client](){ keyHit.stop(); client.stop(); });
    cmdLine.setSend([&client](const std::string& msg){
        std::string nick = msg.substr(0, msg.find_first_of(' '));
        if (nick == "broadcast") {
            client.send(snc::Client::SendType::cl_broadcast, "", msg.substr(nick.length()+1));
        }
        else {
            client.send(snc::Client::SendType::cl_send, nick, msg.substr(nick.length()+1));
        } } );
    client.recvHandler([&cmdLine](const std::string& nick, const std::string& msg){ cmdLine.output("message from "+nick+": "+msg);});
    client.broadcastHandler([&cmdLine](const std::string& nick, const std::string& msg){ cmdLine.output("broadcast message from "+nick+": "+msg);});

    cmdLine.output("starting");

    io_service.run();

    return 0;
}
