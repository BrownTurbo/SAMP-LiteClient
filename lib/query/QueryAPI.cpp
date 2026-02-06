#include "QueryAPI.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <chrono>

void QueryAPI::GetSAMPServerInfo(
        const char* ip,
        uint16_t port,
        QueryAPI::SAMPServerInfo& outInfo,
        bool& success
) {
    success = false;

    // Initialize output
    outInfo.serverName.clear();
    outInfo.mapName.clear();
    outInfo.gameMode.clear();
    outInfo.language.clear();
    outInfo.currentPlayers = 0;
    outInfo.maxPlayers = 0;
    outInfo.ping = 0;

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        printf("QueryAPI: Socket creation failed: %s", strerror(errno));
        return;
    }

    // Set receive timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        printf("QueryAPI: setsockopt failed: %s", strerror(errno));
        close(sock);
        return;
    }

    // Setup server address
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        printf("QueryAPI: Invalid IP address: %s", ip);
        close(sock);
        return;
    }

    uint8_t basePacket[11];
    memcpy(basePacket, "SAMP", 4);

    in_addr addr;
    if (inet_pton(AF_INET, ip, &addr) <= 0) {
        printf("QueryAPI: Failed to convert IP");
        close(sock);
        return;
    }
    memcpy(basePacket + 4, &addr.s_addr, 4);
    basePacket[8] = (uint8_t)(port & 0xFF);
    basePacket[9] = (uint8_t)((port >> 8) & 0xFF);

    uint8_t buffer[2048];
    socklen_t fromLen = sizeof(serverAddr);

    // 1) Try 'p' packet (ping probe) first. It's the intended lightweight ping packet.
    {
        uint8_t pingPacket[11];
        memcpy(pingPacket, basePacket, sizeof(pingPacket));
        pingPacket[10] = 'p';

        auto t1 = std::chrono::steady_clock::now();
        ssize_t sent = sendto(sock, pingPacket, sizeof(pingPacket), 0,
                              (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (sent == sizeof(pingPacket)) {
            ssize_t recvLen = recvfrom(sock, (char*)buffer, sizeof(buffer), 0,
                                       (struct sockaddr*)&serverAddr, &fromLen);
            auto t2 = std::chrono::steady_clock::now();

            if (recvLen >= 0 && memcmp(buffer, "SAMP", 4) == 0 && buffer[10] == 'p') {
                outInfo.ping = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                // valid ping reply received
            } else {
                // ping probe failed or invalid response; leave ping=0 and fall back to 'i'
                outInfo.ping = 0;
            }
        }
    }

    // 2) Send 'i' to fetch server info (and fallback for ping if 'p' didn't work)
    uint8_t infoPacket[11];
    memcpy(infoPacket, basePacket, sizeof(infoPacket));
    infoPacket[10] = 'i';

    auto t1_info = std::chrono::steady_clock::now();
    ssize_t sentInfo = sendto(sock, infoPacket, sizeof(infoPacket), 0,
                              (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sentInfo != sizeof(infoPacket)) {
        printf("QueryAPI: Failed to send query packet: %s", strerror(errno));
        close(sock);
        return;
    }

    socklen_t fromLen2 = sizeof(serverAddr);
    ssize_t recvLen = recvfrom(sock, (char*)buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&serverAddr, &fromLen2);
    auto t2_info = std::chrono::steady_clock::now();

    if (recvLen < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("QueryAPI: Timeout waiting for response");
        } else {
            printf("QueryAPI: recvfrom failed: %s", strerror(errno));
        }
        close(sock);
        return;
    }

    // If 'p' didn't provide a ping, use the 'i' round-trip as fallback
    if (outInfo.ping == 0) {
        outInfo.ping = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(t2_info - t1_info).count();
    }

    if (recvLen < 11) {
        printf("QueryAPI: Response too short: %zd bytes", recvLen);
        close(sock);
        return;
    }

    // Check header
    if (memcmp(buffer, "SAMP", 4) != 0) {
        printf("QueryAPI: Invalid response header");
        close(sock);
        return;
    }

    if (buffer[10] != 'i') {
        printf("QueryAPI: Invalid opcode: 0x%02X (expected 'i')", buffer[10]);
        close(sock);
        return;
    }

    size_t offset = 11;

    if (offset >= (size_t)recvLen) {
        printf("QueryAPI: Response incomplete (no password byte)");
        close(sock);
        return;
    }
    uint8_t password = buffer[offset++];

    if (offset + 4 <= (size_t)recvLen)
        //printf("QueryAPI: Players bytes at offset %zu: %02X %02X %02X %02X", offset, buffer[offset], buffer[offset+1], buffer[offset+2], buffer[offset+3]);

    if (offset + 2 > (size_t)recvLen) {
        printf("QueryAPI: Response incomplete (no players)");
        close(sock);
        return;
    }
    uint16_t players = (uint16_t)buffer[offset] | ((uint16_t)buffer[offset + 1] << 8);
    offset += 2;

    if (offset + 2 > (size_t)recvLen) {
        printf("QueryAPI: Response incomplete (no max players)");
        close(sock);
        return;
    }
    uint16_t maxPlayers = (uint16_t)buffer[offset] | ((uint16_t)buffer[offset + 1] << 8);
    offset += 2;

    //printf("QueryAPI: Parsed players=%d, maxPlayers=%d", players, maxPlayers);

    //printf("QueryAPI: After players - offset=%zu, recvLen=%zd", offset, recvLen);
    if (offset < (size_t)recvLen && offset + 20 <= (size_t)recvLen) {
        //printf("QueryAPI: Next 20 bytes: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
            //  buffer[offset], buffer[offset+1], buffer[offset+2], buffer[offset+3], buffer[offset+4],
            //  buffer[offset+5], buffer[offset+6], buffer[offset+7], buffer[offset+8], buffer[offset+9],
            //  buffer[offset+10], buffer[offset+11], buffer[offset+12], buffer[offset+13], buffer[offset+14],
            //  buffer[offset+15], buffer[offset+16], buffer[offset+17], buffer[offset+18], buffer[offset+19]);
    }

    auto readString = [&](std::string& out, const char* fieldName) -> bool {
        if ((size_t)recvLen < offset + 4) {
            printf("QueryAPI: %s: Not enough bytes (need 4 for length, offset=%zu, recvLen=%zd)", fieldName, offset, recvLen);
            return false;
        }

        uint8_t len = buffer[offset];
        offset += 4;

        //printf("QueryAPI: %s: length=%u (from 1 byte), new offset=%zu", fieldName, len, offset);

        if (len == 0) {
            out.clear();
            return true;
        }

        if ((size_t)recvLen < offset + len) {
            printf("QueryAPI: %s: Not enough bytes for string (offset=%zu, len=%u, recvLen=%zd)", fieldName, offset, len, recvLen);
            return false;
        }

        // Read string data
        out.assign((const char*)&buffer[offset], (size_t)len);
        offset += len;
        //printf("QueryAPI: %s: read '%s'", fieldName, out.c_str());
        return true;
    };

    if (!readString(outInfo.serverName, "ServerName")) {
        printf("QueryAPI: Failed to read server name");
        close(sock);
        return;
    }

    if (!readString(outInfo.gameMode, "GameMode")) {
        printf("QueryAPI: Failed to read gamemode");
        outInfo.gameMode.clear();
    }

    std::string language;
    if (offset < (size_t)recvLen) {
        readString(language, "Language"); // Try to read, but don't fail if missing
    }
    outInfo.language = language;

    outInfo.currentPlayers = (uint16_t)players;
    outInfo.maxPlayers = (uint16_t)maxPlayers;
    outInfo.mapName.clear();

    //printf("QueryAPI: Query successful");
    //printf("QueryAPI: Server Name: %s", outInfo.serverName.c_str());
    //printf("QueryAPI: Game Mode: %s", outInfo.gameMode.c_str());
    //printf("QueryAPI: Players: %d/%d", outInfo.currentPlayers, outInfo.maxPlayers);

    success = true;
    close(sock);
}
