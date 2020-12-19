//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
#pragma once
#include <WinSock2.h>
#include "NetContext.h"
class NetDataWrapLayout
{
public:
	void Init(NetContext* netContext);
	void Close();
	void Uninit();
	bool Connect(const char* address,const char* port);
	void ReceiveService();
	void SendService();
	//bool Send(int header, const char* data, int size);
	WSADATA wsaData;
	SOCKET m_socket;
	NetContext* m_netContext;
	bool m_connected;
	bool m_destroy;

	char m_sendingBuffer[65536 * 2];
	char m_recevingBuffer[65536 * 2];
	char m_recevingProcessBuffer[65536];

	int m_receiveDataLength;
	~NetDataWrapLayout();
};

