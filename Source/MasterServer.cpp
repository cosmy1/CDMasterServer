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

#include "MasterServer.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetworkFactory.h"
#include "RakNet/PacketEnumerations.h"
#include "RakNet/StringCompressor.h"
#include "RakNet/GetTime.h"

// Uncomment this define for debugging printfs
#define _SHOW_MASTER_SERVER_PRINTF
#ifdef _SHOW_MASTER_SERVER_PRINTF
#include <stdio.h>
#endif

#include "RakCover.h"

MasterServer::MasterServer()
{
}

MasterServer::~MasterServer()
{
        ClearServerList();
        //RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
        CoverDestroyRakPeerInterface(&theNetworkFactory, rakPeer);
}

bool MasterServer::Initialize(unsigned short maximumConnections, unsigned short localPort,int _threadSleepTimer)
{
        //if (rakPeer->Initialize(maximumConnections, localPort, _threadSleepTimer)==false)
        if (CoverPeerInitialize(rakPeer, maximumConnections, localPort, _threadSleepTimer) == false)


                return false;
        //rakPeer->SetOccasionalPing(true);
        CoverPeerSetOccasionalPing(rakPeer, true );

        //originally commented out from raknet... blake ....   rakPeer->StartSynchronizedRandomInteger(); // Keepalive

        //rakPeer->SetMaximumIncomingConnections(maximumConnections);
		CoverPeerSetMaximumIncomingConnections(rakPeer, maximumConnections);

        return true;
}

void MasterServer::Disconnect(void)
{
        //rakPeer->Disconnect(200);
        CoverPeerDisconnect(rakPeer, 300);
}

void MasterServer::Update(void)
{
        Packet *packet;
        unsigned serverIndex;
        unsigned long time;
        RakNetStatisticsStruct *rss;
        unsigned long connectionTime;
   		unsigned short numberOfSystems;


        //packet = rakPeer->Receive();
		packet = CoverPeerReceive(rakPeer);

        time = CoverGetTime();//RakNet::GetTime();

        while (packet)
        {
                // Quick and dirty flood attack security:
                // If a client has been connected for more than 5 seconds,
                // and has sent more than 1000 bytes per second on average then ban them
                //rss=rakPeer->GetStatistics(packet->playerId);
                rss = CoverPeerGetStatistics(rakPeer, packet->playerId );
                if (rss)
                {
                        connectionTime=time-rss->connectionStartTime;
                        if (connectionTime > FLOOD_ATTACK_CHECK_DELAY &&
                                (float)(rss->bitsReceived/8) / (float) connectionTime > FLOOD_ATTACK_BYTES_PER_MS)
                        {
                                //rakPeer->CloseConnection(packet->playerId, true,0);
                                CoverPeerCloseConnection(rakPeer, packet->playerId, true,0);
#ifdef _SHOW_MASTER_SERVER_PRINTF
                                printf("%s banned for session due to for flood attack\n", (char*)CoverPeerPlayerIDToDottedIP(rakPeer, packet->playerId));
#endif

                                //rakPeer->AddToBanList(rakPeer->PlayerIDToDottedIP(packet->playerId));
                                CoverPeerAddToBanList(rakPeer, CoverPeerPlayerIDToDottedIP(rakPeer, packet->playerId) );

                                // Find all servers with this IP and kill them.
                                serverIndex=0;
                                while (serverIndex < gameServerList.serverList.size())
                                {
                                        if (gameServerList.serverList[serverIndex]->connectionIdentifier.binaryAddress==packet->playerId.binaryAddress)
                                        {
                                                delete gameServerList.serverList[serverIndex];
                                                gameServerList.serverList.del(serverIndex);
                                        }
                                        else
                                                serverIndex++;
                                }
                                break;
                        }

                }

                switch(packet->data[0])
                {
                case ID_NEW_INCOMING_CONNECTION:
                		   #ifdef _SHOW_MASTER_SERVER_PRINTF

								// if connected
  								CoverPeerGetConnectionList(rakPeer, 0, &numberOfSystems);
    				            printf("New Connection , total connected : %d.\n", numberOfSystems);

							#endif

                        break;
                case ID_DISCONNECTION_NOTIFICATION:
                case ID_CONNECTION_LOST:


			        		int existingServerIndex;
					        existingServerIndex=gameServerList.GetIndexByPlayerIP(packet->playerId);

                  		   #ifdef _SHOW_MASTER_SERVER_PRINTF
								   printf("Deleting server index %i\n",existingServerIndex);
							#endif


					        if (existingServerIndex>=0)
					        {

								gameServerList.serverList[existingServerIndex]->Clear();
								delete gameServerList.serverList[existingServerIndex];
				            	gameServerList.serverList.del(existingServerIndex);

	                   		   #ifdef _SHOW_MASTER_SERVER_PRINTF
						           printf("Lost Connection.\n");
                 		           printf("%i servers on the list\n", gameServerList.serverList.size());


								#endif
							}

                    		#ifdef _SHOW_MASTER_SERVER_PRINTF
								CoverPeerGetConnectionList(rakPeer, 0, &numberOfSystems);
   					            printf("After disconnect, total connected : %d.\n", numberOfSystems);
							#endif



                        break;
                case ID_RECEIVED_STATIC_DATA:
                        break;
                case ID_MODIFIED_PACKET:
                        OnModifiedPacket();
                        break;
                case ID_REMOTE_PORT_REFUSED:
                        break;
                case ID_QUERY_MASTER_SERVER:
                  		   #ifdef _SHOW_MASTER_SERVER_PRINTF
					        printf("Incoming query.\n");
							#endif

                        HandleQuery(packet);
                        break;
                case ID_MASTER_SERVER_DELIST_SERVER:
                        HandleDelistServer(packet);
                        break;
                case ID_MASTER_SERVER_SET_SERVER:
                        HandleUpdateServer(packet);
                        break;
                case ID_PONG:
                        HandlePong(packet);
                        break;
                case ID_RELAYED_CONNECTION_NOTIFICATION:
                        HandleRelayedConnectionNotification(packet);
                        break;
                case ID_PORT_MAP:
                        HandlePortMapRequest(packet);
                		break;
                case 250:
	                	// do nothing with keep alive message....
                		break;
                default:
                        #ifdef _SHOW_MASTER_SERVER_PRINTF
                        printf("MasterServer Unknown type %i\n", packet->data[0]);
                        #endif
                        break;
                }
                //rakPeer->DeallocatePacket(packet);
                //packet = rakPeer->Receive();
                CoverPeerDeallocatePacket(rakPeer, packet );
                packet = CoverPeerReceive(rakPeer);

        }

        serverIndex=0;
        while (serverIndex < gameServerList.serverList.size())
        {
                if (time >= gameServerList.serverList[serverIndex]->nextPingTime)
                {
                		/*
                        if (gameServerList.serverList[serverIndex]->failedPingResponses>=NUMBER_OF_MISSED_PINGS_TO_DROP)
                        {
                                #ifdef _SHOW_MASTER_SERVER_PRINTF
                                //printf("Deleting %s for lack of ping response.\n", (char*)rakPeer->PlayerIDToDottedIP(gameServerList.serverList[serverIndex]->connectionIdentifier));
                                printf("Deleting %s for lack of ping response.\n", CoverPeerPlayerIDToDottedIP(rakPeer, gameServerList.serverList[serverIndex]->connectionIdentifier) );

                                #endif
                                gameServerList.serverList[serverIndex]->Clear();
                                delete gameServerList.serverList[serverIndex];
                                gameServerList.serverList.del(serverIndex);
                        }
                        else
                        */
                        {
                                //rakPeer->Ping((char*)rakPeer->PlayerIDToDottedIP(gameServerList.serverList[serverIndex]->connectionIdentifier),
                                        //gameServerList.serverList[serverIndex]->connectionIdentifier.port, false);
                                CoverPeerPing(rakPeer,(char *)CoverPeerPlayerIDToDottedIP(rakPeer, gameServerList.serverList[serverIndex]->connectionIdentifier),
                        					gameServerList.serverList[serverIndex]->connectionIdentifier.port, false);

                                gameServerList.serverList[serverIndex]->nextPingTime = time + KEEP_ALIVE_PING_FREQUENCY;
                                gameServerList.serverList[serverIndex]->failedPingResponses++;
                                /*
                                #ifdef _SHOW_MASTER_SERVER_PRINTF
                                if (gameServerList.serverList[serverIndex]->failedPingResponses > 1 )
                                {
	                                printf("Pinging %s. %i failed responses inclusive\n", (char*)CoverPeerPlayerIDToDottedIP(rakPeer, gameServerList.serverList[serverIndex]->connectionIdentifier), gameServerList.serverList[serverIndex]->failedPingResponses );
	                            }
                                #endif
                                */
                                serverIndex++;
                        }
                }
                else
                        serverIndex++;
        }

       	// here we internally check if its time to send a keep alive packet...
		if (CoverGetTime() -  lastKeepAlivePulseTimeSent >= PULSETIME_KEEPCONNECT)
		{
			SendKeepAlivePulse();
			lastKeepAlivePulseTimeSent = CoverGetTime();
		}

}

void MasterServer::HandleDelistServer(Packet *packet)
{
        PlayerID serverPlayerID;
        int existingServerIndex;

        //BitStream bitStream((const char*)packet->data, packet->length, false);
        BitStream * bitStream = CoverBitStreamConstructorNet((const char*)packet->data, packet->length, false);


        //bitStream.IgnoreBits(sizeof(unsigned char)*8); // Ignore the packet type enum
        //bitStream.Read(serverPlayerID.port);
        CoverBitStreamIgnoreBits(bitStream, sizeof(unsigned char)*8);
        CoverBitStreamReadUnsignedShort(bitStream, serverPlayerID.port);

        serverPlayerID.binaryAddress=packet->playerId.binaryAddress;

        existingServerIndex=gameServerList.GetIndexByPlayerID(serverPlayerID);
        if (existingServerIndex>=0)
        {
                gameServerList.serverList[existingServerIndex]->Clear();
                delete gameServerList.serverList[existingServerIndex];
                gameServerList.serverList.del(existingServerIndex);
        }
        //else
                // Server does not already exist

        #ifdef _SHOW_MASTER_SERVER_PRINTF
        printf("%i servers on the list\n", gameServerList.serverList.size());
        #endif
}

void MasterServer::HandleQuery(Packet *packet)
{
        BasicDataStructures::List<GameServer*> serversWithKeysList;
        char ruleIdentifier[256];
        unsigned index, serverIndex;
        int key;
        bool queryAll;

        //StringCompressor * theStringCompressor = CoverStringCompressorGetInstance();

        //BitStream outputBitStream;
        //BitStream compressedString((const char*)packet->data, packet->length, false);
        //compressedString.IgnoreBits(8*sizeof(unsigned char));
        BitStream * outputBitStream = CoverBitStreamConstructor();
        BitStream * compressedString = CoverBitStreamConstructorNet((const char*)packet->data, packet->length, false);
        CoverBitStreamIgnoreBits(compressedString, sizeof(unsigned char)*8);


        queryAll=true;

        //while (compressedString.GetNumberOfUnreadBits()>0)
        while (CoverBitStreamGetNumberOfUnreadBits(compressedString) > 0 )
        {
                // Generate a list of the indices of the servers that have one or more of the specified keys.
                //stringCompressor->DecodeString(ruleIdentifier, 256, &compressedString);
				//CoverStringCompressorDecodeString(theStringCompressor,ruleIdentifier, 256, compressedString);
                ReadStringFromBitStream(ruleIdentifier, 256, compressedString);
                if (ruleIdentifier[0]==0)
                        // If we fail to read the first string, queryAll remains true.
                        break;

                queryAll=false;

                if (IsReservedRuleIdentifier(ruleIdentifier))
                        continue;

                for (index=0; index < gameServerList.serverList.size(); index++)
                {
                        //if (gameServerList.serverList[index]->connectionIdentifier==UNASSIGNED_PLAYER_ID)
						if (gameServerList.serverList[index]->connectionIdentifier.binaryAddress ==UNASSIGNED_PLAYER_ID.binaryAddress &&
							gameServerList.serverList[index]->connectionIdentifier.port ==UNASSIGNED_PLAYER_ID.port )
                        	//0xFFFFFFFF, 0xFFFF
                                continue;

                        if (gameServerList.serverList[index]->FindKey(ruleIdentifier))
                        {
                                serverIndex=serversWithKeysList.getIndexOf(gameServerList.serverList[index]);
                                if (serverIndex==MAX_UNSIGNED_LONG)
                                {
                                        gameServerList.serverList[index]->numberOfKeysFound=1;
                                        serversWithKeysList.insert(gameServerList.serverList[index]);
                                }
                                else
                                {
                                        serversWithKeysList[serverIndex]->numberOfKeysFound++;
                                }
                        }
                }
        }

        // Write the packet id
        if (queryAll)
                //outputBitStream.Write((unsigned char) ID_MASTER_SERVER_SET_SERVER);
                CoverBitStreamWriteUnsignedChar(outputBitStream, (unsigned char) ID_MASTER_SERVER_SET_SERVER);
        else
                //outputBitStream.Write((unsigned char) ID_MASTER_SERVER_UPDATE_SERVER);
                CoverBitStreamWriteUnsignedChar(outputBitStream, (unsigned char) ID_MASTER_SERVER_UPDATE_SERVER);
        if (queryAll)
        {
                // Write the number of servers
                //outputBitStream.WriteCompressed((unsigned short)gameServerList.serverList.size());
                CoverBitStreamWriteCompressedUnsignedShort(outputBitStream, (unsigned short)gameServerList.serverList.size() );

                for (index=0; index < gameServerList.serverList.size(); index++)
                {
                	// Write the whole server
                    SerializeServer(gameServerList.serverList[index], outputBitStream);
                }
        }
        else
        {

                //compressedString.ResetReadPointer();
                //compressedString.IgnoreBits(8*sizeof(unsigned char));
                CoverBitStreamResetReadPointer(compressedString);
                CoverBitStreamIgnoreBits(compressedString, sizeof(unsigned char)*8);


                // Write the number of servers with requested keys
                // outputBitStream.WriteCompressed((unsigned short)serversWithKeysList.size());
                CoverBitStreamWriteCompressedUnsignedShort(outputBitStream, (unsigned short)serversWithKeysList.size() );

                // For each server, write the header which consists of the IP/PORT.
                // Then go through the list of requested keys and write those
                for (index=0; index < serversWithKeysList.size(); index++)
                {
                        SerializePlayerID(&(serversWithKeysList[index]->connectionIdentifier), outputBitStream);

                        //outputBitStream.WriteCompressed((unsigned short)serversWithKeysList[index]->numberOfKeysFound);
                        CoverBitStreamWriteCompressedUnsignedShort(outputBitStream, (unsigned short)serversWithKeysList[index]->numberOfKeysFound );
                        //while (compressedString.GetNumberOfUnreadBits()>0)
                        while (CoverBitStreamGetNumberOfUnreadBits(compressedString) > 0 )
                        {
                                // Generate a list of the indices of the servers that have one or more of the specified keys.
                                //stringCompressor->DecodeString(ruleIdentifier, 256, &compressedString);
                   				//CoverStringCompressorDecodeString(theStringCompressor,ruleIdentifier, 256, compressedString);
                   				ReadStringFromBitStream(ruleIdentifier, 256, compressedString);


                                if (ruleIdentifier[0]==0)
                                        break;
                                if (IsReservedRuleIdentifier(ruleIdentifier))
                                        continue;

                                serversWithKeysList[index]->FindKey(ruleIdentifier);
                                key=serversWithKeysList[index]->keyIndex;
                                if (key>=0)
                                        SerializeRule(serversWithKeysList[index]->serverRules[key], outputBitStream);
                        }
                }
        }

        //rakPeer->Send(outputBitStream, MEDIUM_PRIORITY, RELIABLE, 0, packet->playerId, false);
        CoverPeerSendBitStream(rakPeer,outputBitStream, MEDIUM_PRIORITY, RELIABLE, 0, packet->playerId, false);
}

void MasterServer::HandleUpdateServer(Packet *packet)
{
        GameServer *gameServer;
        bool newServerAdded;
        //BitStream incomingBitStream((const char*)packet->data, packet->length, false);
        //incomingBitStream.IgnoreBits(8*sizeof(unsigned char));
        BitStream * incomingBitStream = CoverBitStreamConstructorNet((const char*)packet->data, packet->length, false);
        CoverBitStreamIgnoreBits(incomingBitStream, sizeof(unsigned char)*8);


        gameServer = DeserializeServer(incomingBitStream);
        gameServer->connectionIdentifier.binaryAddress=packet->playerId.binaryAddress;

        UpdateServerList(gameServer, true, &newServerAdded);

        if (newServerAdded)
        {
                #ifdef _SHOW_MASTER_SERVER_PRINTF
                printf("Server added. %i servers on the list\n", gameServerList.serverList.size());
                #endif
                gameServer->originationId=packet->playerId;
        }
        #ifdef _SHOW_MASTER_SERVER_PRINTF
        //else
                //printf("Server updated. %i servers on the list\n", gameServerList.serverList.size());
        #endif
}

void MasterServer::OnModifiedPacket(void)
{
#ifdef _SHOW_MASTER_SERVER_PRINTF
        printf("Modified packet.\n");
#endif
}

void MasterServer::HandleRelayedConnectionNotification(Packet *packet)
{
        char str[22];
        unsigned short clientGamePort, serverGamePort;
        //BitStream incomingBitStream((const char*)packet->data, packet->length, false);
        //incomingBitStream.IgnoreBits(8*sizeof(unsigned char));
        BitStream * incomingBitStream = CoverBitStreamConstructorNet((const char*)packet->data, packet->length, false);
        CoverBitStreamIgnoreBits(incomingBitStream, sizeof(unsigned char)*8);

        //incomingBitStream.Read(clientGamePort);
        //incomingBitStream.Read(serverGamePort);
        CoverBitStreamReadUnsignedShort(incomingBitStream, clientGamePort);
        CoverBitStreamReadUnsignedShort(incomingBitStream, serverGamePort);

        // stringCompressor->DecodeString(str, 22, &incomingBitStream);
        //StringCompressor * theStringCompressor = CoverStringCompressorGetInstance();
		//CoverStringCompressorDecodeString(theStringCompressor,str, 22, incomingBitStream);
        ReadStringFromBitStream(str, 22, incomingBitStream);


        //BitStream outgoingBitStream;
        BitStream * outgoingBitStream = CoverBitStreamConstructor();

        //outgoingBitStream.Write((unsigned char)ID_RELAYED_CONNECTION_NOTIFICATION);
        // Assumes the game client is on the same computer as the master client
        //outgoingBitStream.Write(packet->playerId.binaryAddress);
        //outgoingBitStream.Write(clientGamePort);

        CoverBitStreamWriteUnsignedChar(outgoingBitStream, (unsigned char) ID_RELAYED_CONNECTION_NOTIFICATION);
        CoverBitStreamWriteUnsignedLong(outgoingBitStream, packet->playerId.binaryAddress );
        CoverBitStreamWriteUnsignedShort(outgoingBitStream,clientGamePort);

        #ifdef _SHOW_MASTER_SERVER_PRINTF
	        printf("Relaying Connection attepmt, in MasterServer::HandleRelayedConnectionNotification !\n");
	        printf("Client info requesting connection is ... %i, %i\n", packet->playerId.binaryAddress, clientGamePort);
        #endif


        PlayerID targetID;
        //rakPeer->IPToPlayerID(str, serverGamePort, &targetID);
        CoverPeerIPToPlayerID(rakPeer, str, serverGamePort, &targetID );

        #ifdef _SHOW_MASTER_SERVER_PRINTF
	        printf("The (ip,port) of the server we reuesting a connection from is ( %s , %i )\n", str, serverGamePort);
	        printf("The resulting targetID of the server is ( %i , %i )\n", targetID.binaryAddress , targetID.port);
        #endif


        // Given the IP and port of the game system, give me the index into the game server list
        int serverIndex = gameServerList.GetIndexByPlayerID(targetID);

        #ifdef _SHOW_MASTER_SERVER_PRINTF
	        printf("The server index from the target ID is %i\n", serverIndex);
        #endif

        if (serverIndex>=0)
        {
                //rakPeer->Send(outgoingBitStream, HIGH_PRIORITY, RELIABLE, 0, gameServerList.serverList[serverIndex]->originationId, false);
        		CoverPeerSendBitStream(rakPeer,outgoingBitStream, HIGH_PRIORITY, RELIABLE, 0, gameServerList.serverList[serverIndex]->originationId, false);
        }

}



// A message to be called from masterclient or masterserver to be called every few seconds to see if we still have connection
// our network layer does not need to respond to this
/// ===========================================================
void MasterServer::HandlePortMapRequest(Packet *packet)
{
	// fill in a structure to send the port mapping id request
	InformHostIDMessageStruct s;
	s.typeId = ID_PORT_MAP;
	s.mappedId = packet->playerId;

	CoverPeerSendData(rakPeer, (char*)&s,sizeof(InformHostIDMessageStruct), HIGH_PRIORITY, RELIABLE, 0,packet->playerId, false);
}

