/*
 *   Crashday Master Server
 *   Copyright (C) 2006-2026 Moonbyte Games - a gaming brand of 83 Ventures GmbH
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __MASTER_COMMON_H
#define __MASTER_COMMON_H

#include "RakNet/ArrayList.h"
#include "RakNet/NetworkTypes.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetworkFactory.h"

using namespace RakNet;

class RakPeerInterface;
struct Packet;

// IP, Port, Ping - case sensitive!
#define NUMBER_OF_DEFAULT_MASTER_SERVER_KEYS 3
// If we ping NUMBER_OF_MISSED_PINGS_TO_DROP without ever a response, that server is dropped from the list.
// This includes the last ping, so actually NUMBER_OF_MISSED_PINGS_TO_DROP-1 would be truly missed
#define NUMBER_OF_MISSED_PINGS_TO_DROP 3
// KEEP_ALIVE_PING_FREQUENCY is how often to ping servers to make sure they are active
#define KEEP_ALIVE_PING_FREQUENCY 10000
// How many ms must pass per connection before we check average bytes for a flood attack
#define FLOOD_ATTACK_CHECK_DELAY 5000
// How many bytes per ms someone has to send on average before they are banned.
#define FLOOD_ATTACK_BYTES_PER_MS 2.0f

#define PULSETIME_KEEPCONNECT 3000
#define PULSES_TO_FORCE_UPLOAD 2

#define MASTERSERVERPORT 60008
#define MASTERCLIENTPORT 60007

struct GameServerRule;
struct GameServer;

// This struct is for internal use.
// It represents a list of game servers.
struct GameServerList
{
public:
        GameServerList();
        ~GameServerList();
        void Clear(void);
        void SortOnKey(char *key, bool ascending);
        void QuickSort(int low, int high, bool ascending);
        int Partition(int low, int high, bool ascending);
        int GetIndexByPlayerID(PlayerID playerID);
        int GetIndexByPlayerIP(PlayerID playerID);


        BasicDataStructures::List<GameServer*> serverList;
};

class MasterCommon
{
public:
        MasterCommon();

        // ---------------------------------------------------
        // BROWSER FUNCTIONS
        // ---------------------------------------------------
        // Sorting function
        // ruleIdentifier is a string used by you previously when adding rules via PostRule
        // It can also be "IP" "Port" or "Ping"
        // Set ascending to true to sort from low to high.  Otherwise sorts from high to low.
        void SortServerListOnKey(char *ruleIdentifier, bool ascending);

        // serverIndex should be from 0 to GetServerListSize()-1
        // ruleIdentifier is a string used by you previously when adding rules via PostRule
        // It can also be "IP" "Port" or "Ping".
        // identifier found will return true if the specified rule is found AND you are reading the
        // correct type.
        // GetServerListRuleAsInt should be used for int values.
        // GetServerListRuleAsString should be used for string values
        unsigned long GetServerListSize(void);
        int GetServerListRuleAsInt(int serverIndex, const char *ruleIdentifier, bool *identifierFound);
        const char* GetServerListRuleAsString(int serverIndex, const char *ruleIdentifier, bool *identifierFound);

protected:
        // Delete all elements from the server list
        void ClearServerList(void);
        // Returns true if a rule is reserved
        bool IsReservedRuleIdentifier(char *ruleIdentifier);
        void HandlePong(Packet *packet);
        // Adds or updates the specified rule to the specified server.
        // Returns true if the server has been changed.  False if we are adding a rule that is already the same
        bool UpdateServerRule(GameServer *gameServer, char *ruleIdentifier, char *stringData, int intData);
        // Remove the specified rule from the server.
        // Returns true if the rule was removed.
        bool RemoveServerRule(GameServer *gameServer, char *ruleIdentifier);
        // Encode a playerID to a bitstream
        void SerializePlayerID(PlayerID *playerID, BitStream *outputBitStream);
        // Encode a rule to a bitstream
        void SerializeRule(GameServerRule *gameServerRule, BitStream *outputBitStream);
        // Decode a playerID from a bitstream
        void DeserializePlayerID(PlayerID *playerID, BitStream *inputBitStream);
        // Decode a rule from a bitstream
        GameServerRule *DeserializeRule(BitStream *inputBitStream);
        // Encode a server to a bitstream
        void SerializeServer(GameServer *gameServer, BitStream *outputBitStream);
        // Create a server from a bitstream
        GameServer *DeserializeServer(BitStream *inputBitStream);

        bool ReadStringFromBitStream(char *output, int maxCharsToWrite, BitStream *input );
        void WriteStringToBitStream(char *input, int maxCharsToWrite, BitStream *output );

        // Add the default rules to a server (ip, port, ping)
        void AddDefaultRulesToServer(GameServer *gameServer, PlayerID playerID);
        // Update one server based on the information in another
        void UpdateServer(GameServer *destination, GameServer *source, bool deleteSingleRules);
        // Add the specified server to the list of servers - or if the server already exists
        // Update the existing server and delete the server passed
        // deleteSingleRules means if a match is found and a rule exists in the old
        // server but not the new, then delete that rule.
        // Returns the new or updated server
        GameServer * UpdateServerList(GameServer *gameServer, bool deleteSingleRules, bool *newServerAdded);

        void SendKeepAlivePulse() const;

        unsigned long lastKeepAlivePulseTimeSent;
        int			  keepListedCounter;


        RakNetworkFactory       theNetworkFactory;
        RakPeerInterface *rakPeer;
        GameServerList gameServerList;
};

struct GameServerRule
{
        GameServerRule();
        ~GameServerRule();

        char *key;
        // stringValue and intValue are mutually exclusive
        char *stringValue;
        int intValue;
};

struct GameServer
{
        GameServer();
        ~GameServer();
        void Clear(void);
        bool FindKey(const char *key);
        int keyIndex;
        int numberOfKeysFound;
        unsigned lastUpdateTime;
        PlayerID connectionIdentifier; // The game server
        PlayerID originationId; // Only used by the server - the master client PlayerID
        int failedPingResponses;
        unsigned long nextPingTime;

        // When inserting rules, don't forget that IP and ping should always be added.
        // These are required for any game server
        BasicDataStructures::List<GameServerRule*> serverRules;
};

struct SimpleMessageStruct
{
	unsigned char typeId; // several id's
	// We we then append data manually.  as a char * at the end of the packet after structure size
};


struct InformHostIDMessageStruct
{
	unsigned char typeId; // several id's
	PlayerID	mappedId;
	// We we then append data manually.  as a char * at the end of the packet after structure size
};


#endif
