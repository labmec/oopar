/***************************************************************************
 *   Copyright (C) 2008 by OOPar Project   *
 *   oopar@oopar   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SOCKET_HHH
#define SOCKET_HHH

/** @ingroup socket */

// definicoes do protocolo
#define STARTUP         0
#define GETPORT         1
#define SETPORT         2
#define FINALIZE        3
#define NOTIFYALL       4
#define NEWPROC         5
#define NOTIFYNEW       6
#define SENDRECV        7
#define BARRIER         8


#define SPD_PORT        15000
#define SRUN_PORT       16000
//256
#define BUFFER_SIZE     256
//200
#define MAX_PROC        200
#define INIT_PORT       20000 // numero inicial do range de portas
#define MAX_QUEUE       100


struct srunEnvelope
{
    int msgType;
    char message[BUFFER_SIZE];
};

struct processTable
{
    char hostName[BUFFER_SIZE];
    int  port;
    char inUse;
};

struct spdProcess
{
    int port;
    int pid;
    char name[BUFFER_SIZE];
    char srun[BUFFER_SIZE];
};

typedef struct
{
    int idOp, idSender, dtype, eCount, tag, cSocket;
} OOP_SOCKET_Envelope;


#endif