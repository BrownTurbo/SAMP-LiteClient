#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <string>
#include <cstdint>

class QueryAPI {
public:
    struct SAMPServerInfo {
        std::string serverName;
        std::string mapName;
        std::string gameMode;
        std::string language;
        uint16_t currentPlayers;
        uint16_t maxPlayers;
        uint32_t ping;
    };
    static void GetSAMPServerInfo(
            const char* ip,
            uint16_t port,
            SAMPServerInfo& outInfo,
            bool& success
    );
};
