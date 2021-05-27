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
#include "stdafx.h"
#include "NetDataWrapLayout.h"
#include <ws2tcpip.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

void NetDataWrapLayout::Init(NetContext* netContext)
{
	m_netContext = netContext;
	m_netContext->m_sendEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	char str1[128] = {};
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		int length = sprintf_s(str1, "WSAStartup failed with error: %d\n", iResult);
		OutputDebugStringA(str1);
	}
}

void NetDataWrapLayout::Close()
{
	m_destroy = true;
	closesocket(m_socket);
	SetEvent(m_netContext->m_sendEvent);
}

void NetDataWrapLayout::Uninit()
{
	if (m_connected)
	{
		m_destroy = true;
		closesocket(m_socket);
		WSACleanup();
		SetEvent(m_netContext->m_sendEvent);
	}
}

bool NetDataWrapLayout::Connect(const char* address, const char* port)
{
	char str1[128] = {};
	int iResult;
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo  hints = {};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(address, port, &hints, &result);
	if (iResult != 0) {
		sprintf_s(str1, "WSAStartup failed with error: %d\n", iResult);
		OutputDebugStringA(str1);
	}


	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		m_socket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (m_socket == INVALID_SOCKET) {
			sprintf_s(str1, "socket failed with error: %ld\n", WSAGetLastError());
			OutputDebugStringA(str1);
			return false;
		}

		iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET) {
		sprintf_s(str1, "Unable to connect to server!\n");
		OutputDebugStringA(str1);
		return false;
	}

	m_connected = true;
	return true;
}

void NetDataWrapLayout::ReceiveService()
{
	int iResult;
	int recvbuflen = 65536;
	const static int c_metaLength = 8;
	do
	{
		iResult = recv(m_socket, &m_recevingBuffer[m_receiveDataLength], recvbuflen, 0);
		if (iResult <= 0)break;
		m_receiveDataLength += iResult;
		if (m_receiveDataLength < c_metaLength)continue;
		int peekIndex = 0;
		while (true)
		{
			if (m_receiveDataLength < c_metaLength)
				break;
			int messageHeader = *((int*)&m_recevingBuffer[peekIndex]);
			int messageLength = std::max<int>(std::min<int>(*((int*)&m_recevingBuffer[peekIndex + 4]), 65528), 0);
			if (messageLength == 0)
			{
				m_receiveDataLength -= messageLength + c_metaLength;
				peekIndex += messageLength + c_metaLength;
			}
			else if (m_receiveDataLength < messageLength + c_metaLength)
			{
				break;
			}
			else if (m_receiveDataLength >= messageLength + c_metaLength)
			{
				int actualLength = LZ4_decompress_safe(&m_recevingBuffer[peekIndex + c_metaLength], m_recevingProcessBuffer, messageLength, sizeof(m_recevingProcessBuffer));
				if (actualLength > 0)
				{
					if (actualLength < sizeof(NetBlock64k::m_data))
					{
						std::unique_ptr<NetBlock64k> netBlock = std::unique_ptr<NetBlock64k>(new NetBlock64k);
						netBlock->m_header = messageHeader;
						netBlock->m_size = actualLength;
						memcpy(netBlock->m_data, m_recevingProcessBuffer, actualLength);
						{
							std::lock_guard<std::mutex> guard(m_netContext->lock_bigSizeReceiedData);
							m_netContext->L_bigSizeReceiedDatas_w->emplace_back(*netBlock);
						}
					}
					else
					{
						int dosomething = 0;
					}
				}
				m_receiveDataLength -= messageLength + c_metaLength;
				peekIndex += messageLength + c_metaLength;
			}
			else if (messageLength < 0)
			{
				perror("net error. NetDataWrapLayout.cpp ReceiveService()\n");
				closesocket(m_socket);
				return;
			}
		}
		if (peekIndex)
		{
			memmove(m_recevingBuffer, &m_recevingBuffer[peekIndex], m_receiveDataLength);
		}
	} while (iResult > 0);
}

void NetDataWrapLayout::SendService()
{
	char str1[128] = {};
	const static int c_metaLength = 8;
	int iResult;
	while (true)
	{
		WaitForSingleObjectEx(m_netContext->m_sendEvent, INFINITE, false);
		if (m_destroy)
			return;

		{
			std::lock_guard<std::mutex> guard1(m_netContext->lock_smallSizeSendData);
			std::swap(m_netContext->L_smallSizeSendDatas_w, m_netContext->m_smallSizeSendDatas_r);
		}
		for (int i = 0; i < m_netContext->m_smallSizeSendDatas_r->size(); i++)
		{
			int compressedSize = LZ4_compress_default((*m_netContext->m_smallSizeSendDatas_r)[i].m_data, &m_sendingBuffer[8], (*m_netContext->m_smallSizeSendDatas_r)[i].m_size, 65528);
			*(int*)&m_sendingBuffer[0] = (*m_netContext->m_smallSizeSendDatas_r)[i].m_header;
			*(int*)&m_sendingBuffer[4] = compressedSize;
			iResult = send(m_socket, m_sendingBuffer, compressedSize + c_metaLength, 0);
			if (iResult < 0)
			{
				m_netContext->m_smallSizeSendDatas_r->clear();
				return;
			}
		}
		m_netContext->m_smallSizeSendDatas_r->clear();
	}
}

NetDataWrapLayout::~NetDataWrapLayout()
{
	Uninit();
}
