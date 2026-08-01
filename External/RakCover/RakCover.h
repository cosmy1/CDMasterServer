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

#ifndef __RAK_NET_COVER_H
#define __RAK_NET_COVER_H

#ifdef RAKCOVER_DLL_EXPORTS
#define RAKCOVERDLL_API __declspec(dllexport)
#else
#define RAKCOVERDLL_API __declspec(dllimport)
#endif

// raknet network factory

extern "C" RAKCOVERDLL_API RakServerInterface*  _stdcall CoverGetRakServerInterface(RakNetworkFactory *theNetworkFactory);
extern "C" RAKCOVERDLL_API void _stdcall CoverDestroyRakServerInterface(RakNetworkFactory *theNetworkFactory, RakServerInterface* i);
extern "C" RAKCOVERDLL_API RakClientInterface*  _stdcall CoverGetRakClientInterface(RakNetworkFactory * theNetworkFactory);
extern "C" RAKCOVERDLL_API void _stdcall CoverDestroyRakClientInterface(RakNetworkFactory *theNetworkFactory, RakClientInterface* i);
extern "C" RAKCOVERDLL_API RakPeerInterface*  _stdcall CoverGetRakPeerInterface(RakNetworkFactory * theNetworkFactory);
extern "C" RAKCOVERDLL_API void _stdcall CoverDestroyRakPeerInterface(RakNetworkFactory *theNetworkFactory, RakPeerInterface* i);


// raknet server

extern "C" RAKCOVERDLL_API void _stdcall CoverSetStaticServerData(RakServerInterface * theServer, char *data, const long length);
extern "C" RAKCOVERDLL_API bool _stdcall CoverServerStart(RakServerInterface * theServer,
														  unsigned short AllowedPlayers, unsigned long connectionValidationInteger,
														  int threadSleepTimer, unsigned short port);
extern "C" RAKCOVERDLL_API Packet* _stdcall CoverServerRecieve(RakServerInterface * theServer);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerSetPassword(RakServerInterface * theServer,char *password);
extern "C" RAKCOVERDLL_API bool _stdcall CoverServerSend(RakServerInterface * theServer, char *data, const long length,
																  PacketPriority priority, PacketReliability reliability,
																  char orderingChannel, PlayerID playerId, bool broadcast);
extern "C" RAKCOVERDLL_API RakNetStatisticsStruct * const _stdcall CoverServerGetStatistics(RakServerInterface * theServer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerGetPlayerIDFromIndex(RakServerInterface * theServer,int index, unsigned long * address, unsigned short * port);
extern "C" RAKCOVERDLL_API int _stdcall CoverServerGetIndexFromPlayerID(RakServerInterface * theServer,PlayerID );
extern "C" RAKCOVERDLL_API int _stdcall CoverServerGetAveragePing(RakServerInterface * theServer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerStartOccasionalPing(RakServerInterface * theServer);
extern "C" RAKCOVERDLL_API unsigned short _stdcall CoverServerGetConnectedPlayers(RakServerInterface * theServer);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerPingPlayer(RakServerInterface * theServer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerKickPlayer(RakServerInterface * theServer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerAddToBanList(RakServerInterface * theServer, const char *IP);
extern "C" RAKCOVERDLL_API RakNet::BitStream * _stdcall CoverServerGetStaticClientData(RakServerInterface * theServer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerDisconnect(RakServerInterface * theServer, unsigned long blockDuration);
extern "C" RAKCOVERDLL_API unsigned int _stdcall CoverServerGetNumberOfAddresses(RakServerInterface * theServer);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerDeallocatePacket(RakServerInterface * theServer,Packet *packet);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerPushBackPacket(RakServerInterface * theServer,Packet *packet);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerSetRelayStaticClientData(RakServerInterface * theServer,bool relay);
extern "C" RAKCOVERDLL_API void _stdcall CoverServerAdvertiseSystem(RakServerInterface * theServer, char *host, unsigned short remotePort);


// raknet client

extern "C" RAKCOVERDLL_API void _stdcall CoverSetStaticClientData(RakClientInterface * theClient, PlayerID playerId, char *data, const long length);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientAllowConnectionResponseIPMigration(RakClientInterface * theClient, bool allow);
extern "C" RAKCOVERDLL_API bool _stdcall CoverClientConnect(RakClientInterface * theClient, char* host, unsigned short serverPort,
															unsigned short clientPort, unsigned long connectionValidationInteger, int threadSleepTimer);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientSetPassword(RakClientInterface * theClient,char *password);
extern "C" RAKCOVERDLL_API RakNetStatisticsStruct* const _stdcall CoverClientGetStatistics(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API int _stdcall CoverClientGetAveragePing(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API bool _stdcall CoverClientIsConnected(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientPingServer(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientStartOccasionalPing(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API bool _stdcall CoverClientSend(RakClientInterface * theClient, char *data, const long length,
														 PacketPriority priority, PacketReliability reliability, char orderingChannel);
extern "C" RAKCOVERDLL_API Packet* _stdcall CoverClientReceive(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API RakNet::BitStream * _stdcall CoverClientGetStaticServerData(RakClientInterface * theClient);
extern "C" RAKCOVERDLL_API RakNet::BitStream * _stdcall CoverClientGetStaticClientData(RakClientInterface * theClient,PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientDeallocatePacket(RakClientInterface * theClient,Packet *packet);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientPushBackPacket(RakClientInterface * theClient, Packet *packet);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientDisconnect(RakClientInterface * theClient, unsigned long blockDuration);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientGetPlayerID(RakClientInterface * theClient, unsigned long * address, unsigned short * port);
extern "C" RAKCOVERDLL_API void _stdcall CoverClientAdvertiseSystem(RakClientInterface * theClient, char *host, unsigned short remotePort );


// raknet peer

extern "C" RAKCOVERDLL_API bool _stdcall CoverPeerInitialize(RakPeerInterface * thePeer, unsigned short MaximumNumberOfPeers, unsigned short localPort, int _threadSleepTimer);
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerSetMaximumIncomingConnections(RakPeerInterface * thePeer, unsigned short numberAllowed);
extern "C" RAKCOVERDLL_API bool _stdcall CoverPeerConnect(RakPeerInterface * thePeer, char* host, unsigned short remotePort, char* passwordData, int passwordDataLength );
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerDisconnect(RakPeerInterface * thePeer, unsigned long blockDuration);
extern "C" RAKCOVERDLL_API bool _stdcall CoverPeerGetConnectionList(RakPeerInterface * thePeer, PlayerID *remoteSystems, unsigned short *numberOfSystems);
extern "C" RAKCOVERDLL_API bool _stdcall CoverPeerSendData(RakPeerInterface * thePeer, 
														   char *data, const long length, PacketPriority priority, PacketReliability reliability, 
														   char orderingChannel, PlayerID playerId, bool broadcast );
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerPing(RakPeerInterface * thePeer, char* host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections );
extern "C" RAKCOVERDLL_API Packet* _stdcall CoverPeerReceive(RakPeerInterface * thePeer);
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerDeallocatePacket(RakPeerInterface * thePeer, Packet *packet );
extern "C" RAKCOVERDLL_API const char* _stdcall CoverPeerPlayerIDToDottedIP(RakPeerInterface * thePeer, PlayerID playerId);
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerSetOccasionalPing(RakPeerInterface * thePeer, bool doPing );
extern "C" RAKCOVERDLL_API RakNetStatisticsStruct * const _stdcall CoverPeerGetStatistics(RakPeerInterface * thePeer, PlayerID playerId );
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerCloseConnection(RakPeerInterface * thePeer, PlayerID target, bool sendDisconnectionNotification, unsigned long blockDuration);
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerAddToBanList(RakPeerInterface * thePeer, const char *IP );
extern "C" RAKCOVERDLL_API void _stdcall CoverPeerIPToPlayerID(RakPeerInterface * thePeer, const char* host, unsigned short remotePort, PlayerID *playerId );


// raknet bitstream

namespace RakNet
{
	extern "C" RAKCOVERDLL_API bool _stdcall CoverPeerSendBitStream(RakPeerInterface * thePeer, 
														   BitStream * bitStream, PacketPriority priority, PacketReliability reliability, 
														   char orderingChannel, PlayerID playerId, bool broadcast );

	extern "C" RAKCOVERDLL_API BitStream * _stdcall CoverBitStreamConstructor();
	extern "C" RAKCOVERDLL_API BitStream * _stdcall CoverBitStreamConstructorInt( int initialBytesToAllocate);
	extern "C" RAKCOVERDLL_API BitStream * _stdcall CoverBitStreamConstructorNet(const char* _data, unsigned int lengthInBytes, bool _copyData);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamDestructor(BitStream * theBitStream);
	extern "C" RAKCOVERDLL_API unsigned char*  _stdcall CoverBitStreamGetData(BitStream * theBitStream);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteBool(BitStream * theBitStream, bool input);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteUnsignedChar(BitStream * theBitStream, unsigned char input);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteUnsignedShort(BitStream * theBitStream, unsigned short input);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteUnsignedLong(BitStream * theBitStream, unsigned long input);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteArray(BitStream * theBitStream, char* input, int numberOfBytes );
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteCompressedInt(BitStream * theBitStream, int input);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteCompressedUnsignedShort(BitStream * theBitStream, unsigned short input);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadBool(BitStream * theBitStream, bool &output);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadUnsignedChar(BitStream * theBitStream, unsigned char &output);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadUnsignedShort(BitStream * theBitStream, unsigned short &output);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadUnsignedLong(BitStream * theBitStream, unsigned long &output);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadArray(BitStream * theBitStream, char* output, int numberOfBytes );
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadCompressedInt(BitStream * theBitStream, int &output);
	extern "C" RAKCOVERDLL_API bool _stdcall CoverBitStreamReadCompressedUnsignedShort(BitStream * theBitStream, unsigned short &output);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamIgnoreBits(BitStream * theBitStream, int numberOfBits);
	extern "C" RAKCOVERDLL_API int _stdcall CoverBitStreamGetNumberOfUnreadBits(BitStream * theBitStream);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamResetReadPointer(BitStream * theBitStream);
	extern "C" RAKCOVERDLL_API int _stdcall CoverBitStreamGetNumberOfBitsUsed(BitStream * theBitStream);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamWriteBits(BitStream * theBitStream, 
													unsigned char* input, int numberOfBitsToWrite, bool rightAlignedBits = true);
	extern "C" RAKCOVERDLL_API void _stdcall CoverBitStreamReset(BitStream * theBitStream);

}


// raknet globals

namespace RakNet
{
	extern "C" RAKCOVERDLL_API void _stdcall CoverStatisticsToString(RakNetStatisticsStruct *s, char *buffer, int verbosityLevel);
}

extern "C" RAKCOVERDLL_API unsigned long _stdcall CoverGetTime(void);

#endif
