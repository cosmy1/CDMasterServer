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

// This is my own internal test program for the master server but serves as a good example.

#include "MasterCommon.h"
#include "MasterServer.h"

#define _WIN32_WINNT 0x0400

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define MASTERSERVER_VERSION "1.1"

int main(void)
{
        MasterServer masterServer;
        int serverListSize;
        const char *outputString;
        int outputInt;
        bool identiferFound;
        int index;
        char ch;

        if (masterServer.Initialize(1000, MASTERSERVERPORT, 5))
                printf("Master server version %s started.\n", MASTERSERVER_VERSION);
        else
                printf("Master server version %s failed to start.\n", MASTERSERVER_VERSION);

        printf("(p)rint\n(q)uit\n");

        while (1)
        {
                if (_kbhit())
                {
                        ch=_getch();
                        if (ch=='q')
                                break;
                        else if (ch=='p')
                        {
                                serverListSize=masterServer.GetServerListSize();
                                if (serverListSize==0)
                                {
                                        printf("No servers in list\n");
                                }
                                else
                                {
                                        for (index=0; index < serverListSize; index++)
                                        {
                                                printf("Server : %i\n", index);
                                                outputString=masterServer.GetServerListRuleAsString(index, "IP", &identiferFound);
                                                if (identiferFound)
                                                        printf("IP: %s ", outputString);
                                                else
                                                        printf("NO_IP ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Port", &identiferFound);
                                                if (identiferFound)
                                                        printf("Port: %i ", outputInt);
                                                else
                                                        printf("NO_PORT ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Ping", &identiferFound);
                                                if (identiferFound)
                                                        printf("Ping: %i ", outputInt);
                                                else
                                                        printf("NO_PING ");
                                                outputString=masterServer.GetServerListRuleAsString(index, "Name", &identiferFound);
                                                if (identiferFound)
                                                        printf("Game Name: %s\n", outputString);
                                                else
                                                        printf("NO_NAME\n");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Pub", &identiferFound);
                                                if (identiferFound)
                                                        printf("Public flag: %i ", outputInt);
                                                else
                                                        printf("NO_PUBLICFLAG ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Pass", &identiferFound);
                                                if (identiferFound)
                                                        printf(" Password needed: %i ", outputInt);
                                                else
                                                        printf(" NO_PASS ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "State", &identiferFound);
                                                if (identiferFound)
                                                        printf("GameState: %i\n", outputInt);
                                                else
                                                        printf("NO_STATE\n");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "MaxPly", &identiferFound);
                                                if (identiferFound)
                                                        printf("Max Players: %i ", outputInt);
                                                else
                                                        printf("NO_MAXPLAYER ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "CurPly", &identiferFound);
                                                if (identiferFound)
                                                        printf("Current Players: %i ", outputInt);
                                                else
                                                        printf("NO_CURPLAYER ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Mode", &identiferFound);
                                                if (identiferFound)
                                                        printf("GameMode: %i", outputInt);
                                                else
                                                        printf("NO_MODE");
                                                outputString=masterServer.GetServerListRuleAsString(index, "Track", &identiferFound);
                                                if (identiferFound)
                                                        printf(" Track Name: %s\n", outputString);
                                                else
                                                        printf(" NO_TRACK\n");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "CarCls", &identiferFound);
                                                if (identiferFound)
                                                        printf("Car Classes: %i ", outputInt);
                                                else
                                                        printf("NO_CARCLASS ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Lang", &identiferFound);
                                                if (identiferFound)
                                                        printf("Language: %i ", outputInt);
                                                else
                                                        printf("NO_LANGUAGE ");
                                                outputInt=masterServer.GetServerListRuleAsInt(index, "Ver", &identiferFound);
                                                if (identiferFound)
                                                        printf("Version: %i\n", outputInt);
                                                else
                                                        printf("NO_VERSION\n");
                                        }
                                        printf("\n");
                                }
                        }
                        ch=0;
                }

                masterServer.Update();

                Sleep(3);
                /*
                HANDLE hTimer = NULL;
                LARGE_INTEGER liDueTime;
                liDueTime.QuadPart=-1000000;   // 0.1 sec
                //hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
                hTimer = CreateWaitableTimer(NULL, FALSE, "WaitableTimer");
                if (hTimer)
                {
                        SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
                        if (WaitForSingleObject(hTimer, INFINITE) == WAIT_OBJECT_0)
                        {
                                printf("0.10 second elapsed\n");
                        }
                }
                */


        }

        return 0;
}
