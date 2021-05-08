#include "snc/client.h"
#include "stdlib.h"
#include <string>

#define READBUFFERLEN 10
#define LINEBUFFERLEN 255

int runCommand(const std::string& command, const std::function<void(const std::string&)>& resultFunction)
{
    std::string result;

    FILE* pipe = popen(command.c_str(), "re");
    if (!pipe)
    {
        std::cerr << "Couldn't start command." << std::endl;
        return 0;
    }
    static std::array<char,LINEBUFFERLEN> buffer;
    char myChar[READBUFFERLEN];
    int cnt(0);
    std::cerr << "start reading\n";
    while (!std::feof(pipe)) {
        if ((std::fgets(myChar, READBUFFERLEN, pipe)) != NULL) {

            for (int i(0); i < READBUFFERLEN; ++i) {
                if (myChar[i] == 0)
                    break;
                if (myChar[i] == '\n' && resultFunction) {
                    resultFunction(std::string(&buffer[0], &buffer[cnt]));
                    cnt = 0;
                } else
                    if (cnt < LINEBUFFERLEN)
                        buffer[cnt++] = myChar[i];
            }
        }
    }
    std::cerr << "end\n";
    pclose(pipe);
    return 0;
}

int main(int argc, char* argv[]) {

    if (argc != 3 && argc != 4) {
        std::cerr << "usage "<<argv[0]<<" <command> <receiver nick> [grep value]\n";
        return -1;
    }
    boost::asio::io_service io_service;

    snc::Client client("generic", io_service, "127.0.0.1", 12001);

    std::string command(argv[1]);
    std::string receiver_nick(argv[2]);
    std::string grepValue(argc==4?argv[3]:"");

    std::thread t([&]() {
        runCommand(command, [&](const std::string &result) {
            if (!grepValue.empty() && result.find(grepValue) == std::string::npos)
                return;
            std::cerr << result << std::endl;
            io_service.post(
                    [&client, &receiver_nick, result]() { client.send(snc::Client::SendType::cl_send, receiver_nick, result); });
        });
        io_service.post([&](){client.stop();});
    });

    io_service.run();

    t.join();

    return 0;
}

