//
// Copyright 2020 Electronic Arts Inc.
//
// TiberianDawn.DLL and RedAlert.dll and corresponding source code is free
// software: you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.

// TiberianDawn.DLL and RedAlert.dll and corresponding source code is distributed
// in the hope that it will be useful, but with permitted additional restrictions
// under Section 7 of the GPL. See the GNU General Public License in LICENSE.TXT
// distributed with this program. You should have received a copy of the
// GNU General Public License along with permitted additional restrictions
// with this program. If not, see https://github.com/electronicarts/CnC_Remastered_Collection

/* $Header: /CounterStrike/IPXCONN.CPP 1     3/03/97 10:24a Joe_bostic $ */
/***************************************************************************
 **   C O N F I D E N T I A L --- W E S T W O O D    S T U D I O S        **
 ***************************************************************************
 *                                                                         *
 *                 Project Name : Command & Conquer                        *
 *                                                                         *
 *                    File Name : IPXCONN.CPP                              *
 *                                                                         *
 *                   Programmer : Bill Randolph                            *
 *                                                                         *
 *                   Start Date : December 20, 1994                        *
 *                                                                         *
 *                  Last Update : April 9, 1995 [BRR]								*
 *                                                                         *
 *-------------------------------------------------------------------------*
 * Functions:    			                                                   *
 *   IPXConnClass::IPXConnClass -- class constructor           				*
 *   IPXConnClass::~IPXConnClass -- class destructor           				*
 *   IPXConnClass::Init -- hardware-specific initialization routine			*
 *   IPXConnClass::Configure -- One-time initialization routine				*
 *   IPXConnClass::Start_Listening -- commands IPX to listen               *
 *   IPXConnClass::Stop_Listening -- commands IPX to stop  listen          *
 *   IPXConnClass::Send -- sends a packet; invoked by SequencedConnection	*
 *   IPXConnClass::Open_Socket -- opens communications socket    				*
 *   IPXConnClass::Close_Socket -- closes the socket 								*
 *   IPXConnClass::Send_To -- sends the packet to the given address			*
 *   IPXConnClass::Broadcast -- broadcasts the given packet						*
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "function.h"
#include <stdio.h>
#include <string.h>
#include "ipxconn.h"

#ifdef WINSOCK_IPX
#include "wsproto.h"
#endif // WINSOCK_IPX

/*
********************************* Globals ***********************************
*/
unsigned short IPXConnClass::Socket;
int IPXConnClass::ConnectionNum;
ECBType* IPXConnClass::ListenECB;
IPXHeaderType* IPXConnClass::ListenHeader;
char* IPXConnClass::ListenBuf;
ECBType* IPXConnClass::SendECB;
IPXHeaderType* IPXConnClass::SendHeader;
char* IPXConnClass::SendBuf;
int IPXConnClass::Handler;
int IPXConnClass::Configured = 0;
int IPXConnClass::SocketOpen = 0;
int IPXConnClass::Listening = 0;
int IPXConnClass::PacketLen;

/***************************************************************************
 * IPXConnClass::IPXConnClass -- class constructor             				*
 *                                                                         *
 * INPUT:                                                                  *
 *		numsend			desired # of entries for the send queue					*
 *		numreceive		desired # of entries for the receive queue				*
 *		maxlen			max length of an application packet							*
 *		magicnum			the packet "magic number" for this connection			*
 *		address			address of destination (NULL = no address)				*
 *		id					connection's unique numerical ID								*
 *		name				connection's name													*
 *		extralen			max size of app-specific extra bytes (optional)			*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/20/1994 BR : Created.                                              *
 *=========================================================================*/
IPXConnClass::IPXConnClass(int numsend,
                           int numreceive,
                           int maxlen,
                           unsigned short magicnum,
                           IPXAddressClass* address,
                           int id,
                           const char* name,
                           int extralen)
    : ConnectionClass(numsend,
                      numreceive,
                      maxlen,
                      magicnum,
                      2,        // retry delta
                      -1,       // max retries
                      60,       // timeout
                      extralen) // (currently, this is only used by the Global Channel)
{
    NetNumType net;
    NetNodeType node;

    /*------------------------------------------------------------------------
    Save the values passed in
    ------------------------------------------------------------------------*/
    if (address)
        Address = (*address);
    ID = id;
    strcpy(Name, name);

#ifdef WINSOCK_IPX
    Address.Get_Address(net, node);
    memcpy(ImmediateAddress, node, 6);
    Immed_Set = 0;
#endif // WINSOCK_IPX
} /* end of IPXConnClass */

/***************************************************************************
 * IPXConnClass::Init -- hardware-specific initialization routine				*
 *                                                                         *
 * INPUT:                                                                  *
 *		none.																						*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/20/1994 BR : Created.                                              *
 *=========================================================================*/
void IPXConnClass::Init(void)
{
    /*------------------------------------------------------------------------
    Invoke the parent's Init routine
    ------------------------------------------------------------------------*/
    ConnectionClass::Init();

} /* end of Init */

/***************************************************************************
 * IPXConnClass::Configure -- One-time initialization routine					*
 *                                                                         *
 * This routine sets up static members that are shared by all IPX				*
 * connections (ie those variables used by the Send/Listen/Broadcast			*
 * routines).																					*
 *																									*
 * INPUT:                                                                  *
 *		socket				socket ID for sending & receiving						*
 *		conn_num				local IPX Connection Number (0 = not logged in)		*
 *		listen_ecb			ptr to ECBType for listening								*
 *		send_ecb				ptr to ECBType for sending									*
 *		listen_header		ptr to IPXHeaderType for listening						*
 *		send_header			ptr to IPXHeaderType for sending							*
 *		listen_buf			ptr to buffer for listening								*
 *		send_buf				ptr to buffer for sending									*
 *		handler_rm_ptr		REAL-MODE pointer to event service routine			*
 *								(high word = segment, low word = offset)				*
 *		maxpacketlen		max packet size to listen for								*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		- All pointers must be protected-mode pointers, but must point to		*
 *		  DOS real-mode memory (except the Handler segment/offset)				*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/20/1994 BR : Created.                                              *
 *=========================================================================*/
void IPXConnClass::Configure(unsigned short socket,
                             int conn_num,
                             ECBType* listen_ecb,
                             ECBType* send_ecb,
                             IPXHeaderType* listen_header,
                             IPXHeaderType* send_header,
                             char* listen_buf,
                             char* send_buf,
                             int handler_rm_ptr,
                             int maxpacketlen)
{
    /*------------------------------------------------------------------------
    Save the values passed in
    ------------------------------------------------------------------------*/
    Socket = socket;
    ConnectionNum = conn_num;
    ListenECB = listen_ecb;
    SendECB = send_ecb;
    ListenHeader = listen_header;
    SendHeader = send_header;
    ListenBuf = listen_buf;
    SendBuf = send_buf;
    Handler = handler_rm_ptr;
    PacketLen = maxpacketlen;

    Configured = 1;

} /* end of Configure */

/***************************************************************************
 * IPXConnClass::Start_Listening -- commands IPX to listen                 *
 *                                                                         *
 * This routine may be used to start listening in polled mode (if the 		*
 * ECB's Event_Service_Routine is NULL), or in interrupt mode; it's			*
 * up to the caller to fill the ECB in.  If in polled mode, Listening		*
 * must be restarted every time a packet comes in.									*
 *                                                                         *
 * INPUT:                                                                  *
 *		none.																						*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		- The ListenECB must have been properly filled in by the IPX Manager.*
 *		- Configure must be called before calling this routine.					*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Start_Listening(void)
{
#ifdef WINSOCK_IPX
    /*
    ** Open the socket.
    */
    if (!Open_Socket(Socket))
        return (false);

    /*
    ** start listening on the socket.
    */
    if (PacketTransport->Start_Listening()) {
        Listening = 1;
        return (true);
    } else {
        Close_Socket(Socket);
        return (false);
    }

#else
    return (false);
#endif // WINSOCK_IPX
} /* end of Start_Listening */

/***************************************************************************
 * IPXConnClass::Stop_Listening -- commands IPX to stop  listen            *
 *                                                                         *
 * INPUT:                                                                  *
 *		none.																						*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		- This routine MUST NOT be called if IPX is not listening already!	*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Stop_Listening(void)
{
#ifdef WINSOCK_IPX
    if (PacketTransport)
        PacketTransport->Stop_Listening();
    Listening = 0;

    //	All done.
    return (1);
#else
    return (1);
#endif // WINSOCK_IPX

} /* end of Stop_Listening */

/***************************************************************************
 * IPXConnClass::Send -- sends a packet; invoked by SequencedConnection		*
 *                                                                         *
 * INPUT:                                                                  *
 *		buf			buffer to send															*
 *		buflen		length of buffer to send											*
 *		extrabuf		(not used by this class)											*
 *		extralen		(not used by this class)											*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		1 = OK, 0 = error																		*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Send(char* buf, int buflen, void*, int)
{
    /*------------------------------------------------------------------------
    Invoke our own Send_To routine, filling in our Address as the destination.
    ------------------------------------------------------------------------*/
    if (Immed_Set) {
        return (Send_To(buf, buflen, &Address, ImmediateAddress));
    } else {
        return (Send_To(buf, buflen, &Address, NULL));
    }

} /* end of Send */

/***************************************************************************
 * IPXConnClass::Open_Socket -- opens communications socket    				*
 *                                                                         *
 * INPUT:                                                                  *
 *		socket		desired socket ID number											*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		1 = OK, 0 = error																		*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Open_Socket(unsigned short socket)
{
    int rc;
#ifdef WINSOCK_IPX
    rc = PacketTransport->Open_Socket(socket);

    SocketOpen = rc;
    return (rc);

#else  // WINSOCK_IPX
    return (0);
#endif // WINSOCK_IPX

} /* end of Open_Socket */

/***************************************************************************
 * IPXConnClass::Close_Socket -- closes the socket 								*
 *                                                                         *
 * INPUT:                                                                  *
 *		socket		desired socket ID number											*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		none.																						*
 *                                                                         *
 * WARNINGS:                                                               *
 *		Calling this routine when the sockets aren't open may crash!			*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
void IPXConnClass::Close_Socket(unsigned short socket)
{
#ifdef WINSOCK_IPX
    socket = socket;
    PacketTransport->Close_Socket();
    SocketOpen = 0;
#endif // WINSOCK_IPX
} /* end of Close_Socket */

/***************************************************************************
 * IPXConnClass::Send_To -- sends the packet to the given address				*
 *                                                                         *
 * The "ImmediateAddress" field of the SendECB must be filled in with the	*
 * address of a bridge, or the node address of the destination if there		*
 * is no bridge.  The NETX call to find this address will always crash		*
 * if NETX isn't loaded (ConnectionNum is 0), so this case is trapped &		*
 * prevented.																					*
 * Also, if the address of this IPX connection is known when the 				*
 * constructor is called, and Configure has been called, Get_Local_Target 	*
 * is called to precompute the ImmediateAddress; this case is detected & 	*
 * if the value is already computed, it's just memcpy'd over.					*
 *                                                                         *
 * INPUT:                                                                  *
 *		buf			buffer to send															*
 *		buflen		length of buffer														*
 *		address		Address to send to													*
 *		immed			ImmediateAddress value, NULL if none							*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		1 = OK, 0 = error																		*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Send_To(char* buf, int buflen, IPXAddressClass* address, NetNodeType immed)
{
#ifdef WINSOCK_IPX

    immed = immed;
    assert(immed == NULL);
    PacketTransport->WriteTo((void*)buf, buflen, (void*)address);
    return (true);

#else  // WINSOCK_IPX
    return (false);
#endif // WINSOCK_IPX

} /* end of Send_To */

/***************************************************************************
 * IPXConnClass::Broadcast -- broadcasts the given packet						*
 *                                                                         *
 * INPUT:                                                                  *
 *		socket		desired socket ID number											*
 *                                                                         *
 * OUTPUT:                                                                 *
 *		1 = OK, 0 = error																		*
 *                                                                         *
 * WARNINGS:                                                               *
 *		none.																						*
 *                                                                         *
 * HISTORY:                                                                *
 *   12/16/1994 BR : Created.                                              *
 *=========================================================================*/
int IPXConnClass::Broadcast(char* buf, int buflen)
{
#ifdef WINSOCK_IPX
    PacketTransport->Broadcast(buf, buflen);
    return (true);

#else  // WINSOCK_IPX
    return (false);
#endif // WINSOCK_IPX
} /* end of Broadcast */

/************************** end of ipxconn.cpp *****************************/
