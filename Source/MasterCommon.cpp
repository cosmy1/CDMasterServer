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

#include "MasterCommon.h"

#include "RakNet/RakNetworkFactory.h"
#include "RakNet/RakPeerInterface.h"
#include "RakNet/GetTime.h"
#include "RakNet/StringCompressor.h"
#include "RakNet/BitStream.h"

#include <string.h>
#include <stdio.h>

#include "RakCover.h"

using namespace RakNet;

GameServerRule::GameServerRule()
{
        key=0;
        stringValue=0;
        intValue=-1;
}
GameServerRule::~GameServerRule()
{
        if (key)
                delete [] key;
        if (stringValue)
                delete [] stringValue;
}

GameServer::GameServer()
{
        connectionIdentifier=UNASSIGNED_PLAYER_ID;
        nextPingTime=0;
        failedPingResponses=0;
        lastUpdateTime=CoverGetTime();//RakNet::GetTime();
}
GameServer::~GameServer()
{
        Clear();
}
void GameServer::Clear()
{
        unsigned i;
        for (i=0; i < serverRules.size(); i++)
                delete serverRules[i];
        serverRules.clear();
}
bool GameServer::FindKey(const char *key)
{
        unsigned i;
        for (i=0; i < serverRules.size(); i++)
                if (strcmp(serverRules[i]->key, key)==0)
                {
                        keyIndex=i;
                        return true;
                }

        keyIndex=-1;
        return false;
}

GameServerList::GameServerList()
{
}
GameServerList::~GameServerList()
{
        Clear();
}
void GameServerList::Clear(void)
{
        unsigned i;
        for (i=0; i < serverList.size(); i++)
                delete serverList[i];
        serverList.clear();
}
void GameServerList::SortOnKey(char *key, bool ascending)
{
        unsigned i;
        // Set keyindex
        for (i=0; i < serverList.size(); i++)
                serverList[i]->FindKey(key);

        QuickSort(0, serverList.size()-1,ascending);
}


void GameServerList::QuickSort(int low, int high, bool ascending)
{
        int pivot;
        if ( high > low )
        {
                pivot = Partition( low, high, ascending);
                QuickSort( low, pivot-1,ascending);
                QuickSort( pivot+1, high,ascending);
        }
}
int EQ(GameServer *left, GameServer *right)
{
        if (left->keyIndex==-1 || right->keyIndex==-1)
                return true;

        if (left->serverRules[left->keyIndex]->stringValue)
                return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue)==0;
        else
                return left->serverRules[left->keyIndex]->intValue == right->serverRules[right->keyIndex]->intValue;

}
int LT(GameServer *left, GameServer *right)
{
        if (left->keyIndex==-1)
                return true;
        if (right->keyIndex==-1)
                return false;

        if (left->serverRules[left->keyIndex]->stringValue)
                return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue) < 0;
        else
                return left->serverRules[left->keyIndex]->intValue < right->serverRules[right->keyIndex]->intValue;
}
int LTEQ(GameServer *left, GameServer *right)
{
        return LT(left, right) || EQ(left, right);
}
int GT(GameServer *left, GameServer *right)
{
        if (left->keyIndex==-1)
                return false;
        if (right->keyIndex==-1)
                return true;

        if (left->serverRules[left->keyIndex]->stringValue)
                return strcmp(left->serverRules[left->keyIndex]->stringValue,right->serverRules[right->keyIndex]->stringValue) > 0;
        else
                return left->serverRules[left->keyIndex]->intValue > right->serverRules[right->keyIndex]->intValue;
}
int GTEQ(GameServer *left, GameServer *right)
{
        return GT(left, right) || EQ(left, right);
}
int GameServerList::Partition(int low, int high, bool ascending)
{
        int left, right, pivot;
        GameServer *pivot_item, *temp;
        pivot_item = serverList[low];
        pivot = left = low;
        right = high;
        while ( left < right )
        {
                if (ascending)
                {
                        /* Move left while item < pivot */
                        while( LTEQ(serverList[left], pivot_item) && left < high) left++;
                        /* Move right while item > pivot */
                        while( GT(serverList[right], pivot_item) && right > 0) right--;
                        if ( left < right )
                        {
                                temp=serverList[left];
                                serverList[left]=serverList[right];
                                serverList[right]=temp;
                        }
                }
                else
                {
                        while( GTEQ(serverList[left], pivot_item) && left < high) left++;
                        while( LT(serverList[right], pivot_item) && right > 0) right--;
                        if ( left < right )
                        {
                                temp=serverList[left];
                                serverList[left]=serverList[right];
                                serverList[right]=temp;
                        }
                }
        }

        /* right is final position for the pivot */
        serverList[low] = serverList[right];
        serverList[right] = pivot_item;

        return right;
}

int GameServerList::GetIndexByPlayerID(PlayerID playerID)
{
        int i;
        for (i=0; i < (int)serverList.size(); i++)
        {
                //if (serverList[i]->connectionIdentifier==playerID)
                if (serverList[i]->connectionIdentifier.binaryAddress == playerID.binaryAddress &&
                        serverList[i]->connectionIdentifier.port == playerID.port)
                {
                        return i;
                }
        }
        return -1;
}

int GameServerList::GetIndexByPlayerIP(PlayerID playerID)
{
        int i;
        for (i=0; i < (int)serverList.size(); i++)
        {
                //if (serverList[i]->connectionIdentifier==playerID)
                if (serverList[i]->connectionIdentifier.binaryAddress == playerID.binaryAddress)
                {
                        return i;
                }
        }
        return -1;
}


MasterCommon::MasterCommon()
{
        //rakPeer = RakNetworkFactory::GetRakPeerInterface();
        rakPeer = CoverGetRakPeerInterface(&theNetworkFactory);
		lastKeepAlivePulseTimeSent = CoverGetTime();
		// force an upload on first keep alive pulse
		keepListedCounter = PULSES_TO_FORCE_UPLOAD;
}
void MasterCommon::ClearServerList(void)
{
        gameServerList.Clear();
}
void MasterCommon::SortServerListOnKey(char *ruleIdentifier, bool ascending)
{
        gameServerList.SortOnKey(ruleIdentifier, ascending);
}
unsigned long MasterCommon::GetServerListSize(void)
{
        return gameServerList.serverList.size();
}
int MasterCommon::GetServerListRuleAsInt(int serverIndex, const char *ruleIdentifier, bool *identifierFound)
{
        int keyIndex;
        if (serverIndex > (int)gameServerList.serverList.size())
        {
                *identifierFound=false;
                return -1;
        }

        gameServerList.serverList[serverIndex]->FindKey(ruleIdentifier);
        keyIndex=gameServerList.serverList[serverIndex]->keyIndex;
        if (keyIndex==-1)
        {
                *identifierFound=false;
                return -1;
        }

        if (gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue)
        {
                *identifierFound=false;
                return -1;
        }

                *identifierFound = true;
        return gameServerList.serverList[serverIndex]->serverRules[keyIndex]->intValue;
}
const char* MasterCommon::GetServerListRuleAsString(int serverIndex, const char *ruleIdentifier, bool *identifierFound)
{
        int keyIndex;
        if (serverIndex > (int)gameServerList.serverList.size())
        {
                *identifierFound=false;
                return "serverIndex out of bounds";
        }

        gameServerList.serverList[serverIndex]->FindKey(ruleIdentifier);
        keyIndex=gameServerList.serverList[serverIndex]->keyIndex;
        if (keyIndex==-1)
        {
                *identifierFound=false;
                return "Server does not contain specified rule";
        }

        if (gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue==0)
        {
                *identifierFound=false;
                return "Server rule is not a string. Use GetServerListRuleAsInt";
        }

        *identifierFound=true;
        return gameServerList.serverList[serverIndex]->serverRules[keyIndex]->stringValue;
}

bool MasterCommon::IsReservedRuleIdentifier(char *ruleIdentifier)
{
        if (strcmp(ruleIdentifier, "Ping")==0 ||
                strcmp(ruleIdentifier, "IP")==0 ||
                strcmp(ruleIdentifier, "Port")==0)
                return true;

        return false;
}

void MasterCommon::AddDefaultRulesToServer(GameServer *gameServer, PlayerID playerID)
{
        GameServerRule *gameServerRule;

        // Every server has NUMBER_OF_DEFAULT_MASTER_SERVER_KEYS keys by default: IP, port, and ping
        gameServerRule = new GameServerRule;
        gameServerRule->key=new char[strlen("IP")+1];
        strcpy(gameServerRule->key, "IP");
        gameServerRule->stringValue=new char[22]; // Should be enough to hold an IP address
        //strncpy(gameServerRule->stringValue, rakPeer->PlayerIDToDottedIP(playerID), 21);
        strncpy(gameServerRule->stringValue, CoverPeerPlayerIDToDottedIP(rakPeer, playerID), 21);
        gameServerRule->stringValue[21]=0;
        gameServer->serverRules.insert(gameServerRule);

        gameServerRule = new GameServerRule;
        gameServerRule->key=new char[strlen("Port")+1];
        strcpy(gameServerRule->key, "Port");
        gameServerRule->intValue=playerID.port;
        gameServer->serverRules.insert(gameServerRule);

        gameServerRule = new GameServerRule;
        gameServerRule->key=new char[strlen("Ping")+1];
        strcpy(gameServerRule->key, "Ping");
        gameServerRule->intValue=9999;
        gameServer->serverRules.insert(gameServerRule);
}
void MasterCommon::HandlePong(Packet *packet)
{
        // Find the server specified by packet
        int serverIndex;
        unsigned long pingTime;

        serverIndex=gameServerList.GetIndexByPlayerID(packet->playerId);
        if (serverIndex>=0)
        {
                gameServerList.serverList[serverIndex]->failedPingResponses=0;
                if (gameServerList.serverList[serverIndex]->FindKey("Ping"))
                {
                        memcpy((char*)&pingTime, packet->data+1, sizeof(unsigned long));
                        gameServerList.serverList[serverIndex]->serverRules[gameServerList.serverList[serverIndex]->keyIndex]->intValue=pingTime;
                        #ifdef _SHOW_MASTER_SERVER_PRINTF
                        printf("Got pong. Ping=%i\n", pingTime);
                        #endif
                }
#ifdef _DEBUG
                else
                        // No ping key!
                        assert(0);
#endif
        }
}
bool MasterCommon::UpdateServerRule(GameServer *gameServer, char *ruleIdentifier, char *stringData, int intData)
{
        GameServerRule *gameServerRule;
        gameServer->lastUpdateTime=CoverGetTime();//RakNet::GetTime();

        // Add the rule to our local server.  If it changes the local server, set a flag so we upload the
        // local server on the next update.
        if (gameServer->FindKey(ruleIdentifier))
        {
        		int gameserverkey = gameServer->keyIndex;
      	        int gameserverint = gameServer->serverRules[gameServer->keyIndex]->intValue;
      	        char *gameserverstring = gameServer->serverRules[gameServer->keyIndex]->stringValue;

                // Is the data the same?
                if (gameServer->serverRules[gameServer->keyIndex]->stringValue)
                {
                        if (stringData==0)
                        {
                                // No string.  Delete the string and use int data instead
                                delete [] gameServer->serverRules[gameServer->keyIndex]->stringValue;
                                gameServer->serverRules[gameServer->keyIndex]->stringValue=0;
                                gameServer->serverRules[gameServer->keyIndex]->intValue=intData;
                                return true;
                        }
                        else if (strcmp(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData)!=0)
                        {
                                // Different string
                                delete [] gameServer->serverRules[gameServer->keyIndex]->stringValue;
                                gameServer->serverRules[gameServer->keyIndex]->stringValue = new char [strlen(stringData)+1];
                                strcpy(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData);
                                return true;
                        }
                }
                else
                {
                        if (stringData)
                        {
                                // Has a string where there is currently none
                                gameServer->serverRules[gameServer->keyIndex]->stringValue = new char [strlen(stringData)+1];
                                strcpy(gameServer->serverRules[gameServer->keyIndex]->stringValue, stringData);
                                gameServer->serverRules[gameServer->keyIndex]->intValue=-1;
                                return true;
                        }
                        else if (gameServer->serverRules[gameServer->keyIndex]->intValue!=intData)
                        {
                                // Different int value
                                gameServer->serverRules[gameServer->keyIndex]->intValue=intData;
                                return true;
                        }
                }
        }
        else
        {
                // No such key.  Add a new one.
                gameServerRule = new GameServerRule;
                gameServerRule->key=new char[strlen(ruleIdentifier)+1];
                strcpy(gameServerRule->key, ruleIdentifier);
                if (stringData)
                {
                        gameServerRule->stringValue=new char[strlen(stringData)+1];
                        strcpy(gameServerRule->stringValue, stringData);
                }
                else
                {
                        gameServerRule->intValue=intData;
                }
                gameServer->serverRules.insert(gameServerRule);
                return true;
        }

        return false;
}
bool MasterCommon::RemoveServerRule(GameServer *gameServer, char *ruleIdentifier)
{
        if (gameServer->FindKey(ruleIdentifier))
        {
                delete gameServer->serverRules[gameServer->keyIndex];
                gameServer->serverRules.del(gameServer->keyIndex);
                return true;
        }

        return false;
}

void MasterCommon::SerializePlayerID(PlayerID *playerID, BitStream *outputBitStream)
{
        //outputBitStream->Write(playerID->binaryAddress);
        //outputBitStream->Write(playerID->port);
        CoverBitStreamWriteUnsignedLong(outputBitStream,playerID->binaryAddress );
        CoverBitStreamWriteUnsignedShort(outputBitStream,playerID->port);

}
void MasterCommon::SerializeRule(GameServerRule *gameServerRule, BitStream *outputBitStream)
{
        //stringCompressor->EncodeString(gameServerRule->key, 256, outputBitStream);
        //StringCompressor * theStringCompressor = CoverStringCompressorGetInstance();
        //CoverStringCompressorEncodeString(theStringCompressor,gameServerRule->key, 256, outputBitStream);
        WriteStringToBitStream(gameServerRule->key, 256, outputBitStream);

        if (gameServerRule->stringValue)
        {
                //outputBitStream->Write(true);
                CoverBitStreamWriteBool(outputBitStream, true);
                //stringCompressor->EncodeString(gameServerRule->stringValue, 256, outputBitStream);
                //CoverStringCompressorEncodeString(theStringCompressor,gameServerRule->stringValue, 256, outputBitStream);
                WriteStringToBitStream(gameServerRule->stringValue, 256, outputBitStream);

        }
        else
        {
                //outputBitStream->Write(false);
                CoverBitStreamWriteBool(outputBitStream, false);
                //outputBitStream->WriteCompressed(gameServerRule->intValue);
                CoverBitStreamWriteCompressedInt(outputBitStream, gameServerRule->intValue);
        }
}
void MasterCommon::DeserializePlayerID(PlayerID *playerID, BitStream *inputBitStream)
{
        *playerID=UNASSIGNED_PLAYER_ID;

        //inputBitStream->Read(playerID->binaryAddress);
        //inputBitStream->Read(playerID->port);
        CoverBitStreamReadUnsignedLong(inputBitStream,playerID->binaryAddress );
        CoverBitStreamReadUnsignedShort(inputBitStream,playerID->port);
}
GameServerRule * MasterCommon::DeserializeRule(BitStream *inputBitStream)
{
        char output[256];
        bool isAString;
        GameServerRule *newRule;

        newRule = new GameServerRule;

        //stringCompressor->DecodeString(output, 256, inputBitStream);
        //StringCompressor * theStringCompressor = CoverStringCompressorGetInstance();
        //CoverStringCompressorDecodeString(theStringCompressor,output, 256, inputBitStream);
        ReadStringFromBitStream(output, 256, inputBitStream);

        if (output[0]==0)
        {
#ifdef _DEBUG
                assert(0);
#endif
                return 0;
        }

        newRule->key = new char [strlen(output)+1];
        strcpy(newRule->key, output);
        //if (inputBitStream->Read(isAString)==false)
        if (CoverBitStreamReadBool(inputBitStream, isAString)==false)
        {
#ifdef _DEBUG
                assert(0);
#endif
                return 0;
        }

        if (isAString)
        {
                //stringCompressor->DecodeString(output, 256, inputBitStream);
                //CoverStringCompressorDecodeString(theStringCompressor,output, 256, inputBitStream);
                        ReadStringFromBitStream(output, 256, inputBitStream);
                if (output[0]==0)
                {
#ifdef _DEBUG
                        assert(0);
#endif
                        return 0;
                }
                newRule->stringValue = new char[strlen(output)+1];
                strcpy(newRule->stringValue, output);
        }
        else
        {
                //if (inputBitStream->ReadCompressed(newRule->intValue)==false)
                if (CoverBitStreamReadCompressedInt(inputBitStream, newRule->intValue) ==false)
                {
#ifdef _DEBUG
                        assert(0);
#endif
                        return 0;
                }
        }

        return newRule;
}
void MasterCommon::SerializeServer(GameServer *gameServer, BitStream *outputBitStream)
{
        unsigned serverIndex;
        unsigned short numberOfRulesToWrite;

        numberOfRulesToWrite=0;

        // Find out how many rules to write.
        for (serverIndex=0; serverIndex < gameServer->serverRules.size(); serverIndex++)
        {
                // We don't write reserved identifiers
                if (IsReservedRuleIdentifier(gameServer->serverRules[serverIndex]->key)==false)
                        numberOfRulesToWrite++;
        }

        // Write the server identifier
        SerializePlayerID(&(gameServer->connectionIdentifier), outputBitStream);

        // Write the number of rules
        //outputBitStream->WriteCompressed(numberOfRulesToWrite);
        //printf("%d rules being written\n", numberOfRulesToWrite);

        CoverBitStreamWriteCompressedUnsignedShort(outputBitStream,numberOfRulesToWrite);


        // Write all the rules
        for (serverIndex=0; serverIndex < gameServer->serverRules.size(); serverIndex++)
        {
                if (IsReservedRuleIdentifier(gameServer->serverRules[serverIndex]->key))
                        continue;

                SerializeRule(gameServer->serverRules[serverIndex], outputBitStream);
        }
}
GameServer * MasterCommon::DeserializeServer(BitStream *inputBitStream)
{
        unsigned serverIndex;
        unsigned short numberOfRulesToWrite;
        GameServer *gameServer;
        GameServerRule *gameServerRule;

        gameServer= new GameServer;
        DeserializePlayerID(&(gameServer->connectionIdentifier), inputBitStream);

        // Read the number of rules
        //if (inputBitStream->ReadCompressed(numberOfRulesToWrite)==false)
        if ( CoverBitStreamReadCompressedUnsignedShort(inputBitStream,numberOfRulesToWrite) == false)
        {
                delete gameServer;
                return 0;
        }
        // Read all the rules
        for (serverIndex=0; serverIndex < numberOfRulesToWrite; serverIndex++)
        {
                gameServerRule = DeserializeRule(inputBitStream);
                if (gameServerRule==0)
                {
                        delete gameServer;
                        return 0;
                }
                if (IsReservedRuleIdentifier(gameServerRule->key))
                        delete gameServerRule;
                else
                        gameServer->serverRules.insert(gameServerRule);
        }
        return gameServer;
}

void MasterCommon::UpdateServer(GameServer *destination, GameServer *source, bool deleteSingleRules)
{
        unsigned sourceRuleIndex,destinationRuleIndex;

        destination->lastUpdateTime=CoverGetTime();//RakNet::GetTime();

        // If (deleteSingleRules) then delete any rules that exist in the old and not in the new
        if (deleteSingleRules)
        {
                destinationRuleIndex=0;
                while (destinationRuleIndex < destination->serverRules.size())
                {
                        if (IsReservedRuleIdentifier(destination->serverRules[destinationRuleIndex]->key)==false &&
                                source->FindKey(destination->serverRules[destinationRuleIndex]->key)==false)
                        {
                                delete destination->serverRules[destinationRuleIndex];
                                destination->serverRules.del(destinationRuleIndex);
                        }
                        else
                                destinationRuleIndex++;
                }
        }

        // Go through all the rules.
        for (sourceRuleIndex=0; sourceRuleIndex < source->serverRules.size(); sourceRuleIndex++)
        {
                if (IsReservedRuleIdentifier(source->serverRules[sourceRuleIndex]->key))
                        continue;

                // Add any fields that exist in the new and do not exist in the old
                // Update any fields that exist in both
                UpdateServerRule(destination, source->serverRules[sourceRuleIndex]->key, source->serverRules[sourceRuleIndex]->stringValue, source->serverRules[sourceRuleIndex]->intValue);
        }
}

GameServer* MasterCommon::UpdateServerList(GameServer *gameServer, bool deleteSingleRules, bool *newServerAdded)
{
        int searchIndex;

        if (gameServer==0)
        {
#ifdef _DEBUG
                assert(0);
#endif
                return 0;
        }

        // Find the existing game server that matches this port/address.
        searchIndex = gameServerList.GetIndexByPlayerID(gameServer->connectionIdentifier);
        if (searchIndex<0)
        {
                // If not found, then add it to the list.
                AddDefaultRulesToServer(gameServer, gameServer->connectionIdentifier);
                gameServerList.serverList.insert(gameServer);
                *newServerAdded=true;
                return gameServer;
        }
        else
        {
                // Update the existing server
                UpdateServer(gameServerList.serverList[searchIndex], gameServer, deleteSingleRules);
                delete gameServer;
                *newServerAdded=false;
                return gameServerList.serverList[searchIndex];
        }
}




bool MasterCommon::ReadStringFromBitStream(char *output, int maxCharsToWrite, BitStream *input )
{
        int bytesInStream;

        output[0] = 0;

        if ( CoverBitStreamReadCompressedInt(input, bytesInStream) == false )
                return false;

        // added this to protect from reading zero length array, I am returning true becuase it readsa what is there ok, just that nothing there
		if (bytesInStream == 0)
				return true;

        if ( CoverBitStreamReadArray(input, output, bytesInStream ) == false)
                return false;

        if ( bytesInStream < maxCharsToWrite )
                output[ bytesInStream ] = 0;
        else
                output[ maxCharsToWrite - 1 ] = 0;

        return true;
}



void MasterCommon::WriteStringToBitStream(char *input, int maxCharsToWrite, BitStream *output )
{
        if ( input == 0 )
                return ;

        int charsToWrite;

        if ( ( int ) strlen( input ) < maxCharsToWrite )
                charsToWrite = ( int ) strlen( input );
        else
                charsToWrite = maxCharsToWrite - 1;

        CoverBitStreamWriteCompressedInt(output, charsToWrite);

        CoverBitStreamWriteArray(output, input, charsToWrite );
}



// A message to be called from masterclient or masterserver to be called every few seconds to see if we still have connection
// our network layer does not need to respond to this
/// ===========================================================
void MasterCommon::SendKeepAlivePulse() const
{
	// fill in a structure to send the pulse
	SimpleMessageStruct s;
	// dont care what the id is, just as long as its high enough to not get confused with other packet types
	s.typeId = 250;

	CoverPeerSendData(rakPeer, (char*)&s,sizeof(SimpleMessageStruct), HIGH_PRIORITY, RELIABLE, 0,UNASSIGNED_PLAYER_ID, true);
}

