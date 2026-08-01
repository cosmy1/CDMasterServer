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

// TOPOLOGY
// Always running
// Active game servers == active clients

#ifndef __MASTER_SERVER_H
#define __MASTER_SERVER_H

#include "MasterCommon.h"

#include "RakNet/NetworkTypes.h"

class MasterServer : public MasterCommon
{
public:
	MasterServer();
	~MasterServer();

	// Starts the master server
	bool Initialize(unsigned short maximumConnections, unsigned short localPort,int _threadSleepTimer);

	// Stops the master server
	void Disconnect(void);

	// Call once per frame.
	void Update(void);

	// Event when a packet was tampered with mid-stream.  Override.
	void OnModifiedPacket(void);
protected:
	void HandleQuery(Packet *packet);
	void HandleDelistServer(Packet *packet);
	void HandleUpdateServer(Packet *packet);
	void HandleRelayedConnectionNotification(Packet *packet);
	void HandlePortMapRequest(Packet *packet);

};

#endif