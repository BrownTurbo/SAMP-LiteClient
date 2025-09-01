#include "RakClient.h"

class CustomRakClient : public RakClient {
public:
    void RegisterAsRemoteProcedureCall(int* uniqueID, void (*functionPointer)(RPCParameters*)) override {
        RakPeer::RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
    }

    void RegisterClassMemberRPC(int* uniqueID, void* functionPointer) override {
        RakPeer::RegisterClassMemberRPC(uniqueID, functionPointer);
    }

    void UnregisterAsRemoteProcedureCall(int* uniqueID) override {
        RakPeer::UnregisterAsRemoteProcedureCall(uniqueID);
    }

    bool RPC(int* uniqueID, const char* data, unsigned int bitLength, PacketPriority priority,
             PacketReliability reliability, char orderingChannel, bool shiftTimestamp, 
             NetworkID networkID, RakNet::BitStream* replyFromTarget) override {
        if (remoteSystemList == nullptr) {
            return false;
        }
        return RakPeer::RPC(uniqueID, data, bitLength, priority, reliability, orderingChannel,
                            remoteSystemList[0].playerId, false, shiftTimestamp, networkID, replyFromTarget);
    }

    bool RPC(int* uniqueID, RakNet::BitStream* parameters, PacketPriority priority,
             PacketReliability reliability, char orderingChannel, bool shiftTimestamp,
             NetworkID networkID, RakNet::BitStream* replyFromTarget) override {
        if (remoteSystemList == nullptr) {
            return false;
        }
        return RakPeer::RPC(uniqueID, parameters, priority, reliability, orderingChannel,
                            remoteSystemList[0].playerId, false, shiftTimestamp, networkID, replyFromTarget);
    }
};

