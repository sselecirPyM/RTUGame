//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game Server.
//
//RTU Game Server is free software : you can redistribute it and/or modify
//it under the terms of the GNU Affero General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game Server is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Affero General Public License for more details.
//
//You should have received a copy of the GNU Affero General Public License
//along with RTU Game Server.If not, see < https://www.gnu.org/licenses/>.
using RTUGameServer.Compression;
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace RTUGameServer.Net
{
    public class NetDataWrapLayout
    {
        Socket m_socket;
        NetContext m_netcontext;
        byte[] m_receiveBuffer = new byte[65536 * 2];
        byte[] m_receiveProcessBuffer = new byte[65536];
        byte[] m_sendBuffer = new byte[65536];
        byte[] m_sendProcessBuffer = new byte[65536];
        int m_receiveDataLength = 0;
        public void Init(NetContext netContext, Socket socket)
        {
            m_socket = socket;
            m_netcontext = netContext;
        }
        const int c_metaLength = 8;
        public void ReceiveService()
        {
            try
            {
                while (true)
                {
                    int length = m_socket.Receive(m_receiveBuffer, m_receiveDataLength, 65536, SocketFlags.None);
                    if (length == 0) break;
                    m_receiveDataLength += length;
                    int peekIndex = 0;
                    while (true)
                    {
                        if (m_receiveDataLength < c_metaLength) break;
                        int blockSize = BitConverter.ToInt32(m_receiveBuffer, peekIndex + 4);
                        if (m_receiveDataLength < blockSize + c_metaLength) break;
                        if (blockSize > 0)
                        {
                            int decodedSize = LZ4.Decode(m_receiveBuffer, peekIndex + c_metaLength, m_receiveProcessBuffer, 0, blockSize, 65536);
                            if (decodedSize > 0)
                            {
                                NetBlock block = m_netcontext.GetNetBlock(BitConverter.ToInt32(m_receiveBuffer, peekIndex), decodedSize);
                                Array.Copy(m_receiveProcessBuffer, block.data, decodedSize);
                                lock (m_netcontext.lock_receivedBlock)
                                {
                                    m_netcontext.l_receivedBlock_w.Add(block);
                                }
                            }
                        }
                        else
                        {
                            NetBlock block = m_netcontext.GetNetBlock(BitConverter.ToInt32(m_receiveBuffer, peekIndex), 0);
                            lock (m_netcontext.lock_receivedBlock)
                            {
                                m_netcontext.l_receivedBlock_w.Add(block);
                            }
                        }
                        peekIndex += blockSize + c_metaLength;
                        m_receiveDataLength -= blockSize + c_metaLength;
                        m_netcontext.receiveEvent.Set();
                    }
                    Array.ConstrainedCopy(m_receiveBuffer, peekIndex, m_receiveBuffer, 0, m_receiveDataLength);
                }
            }
            catch (SocketException)
            {

            }
            catch (Exception e)
            {
                m_netcontext.Log(e);
            }
            finally
            {
                m_netcontext.receiveEvent.Set();
            }
        }

        public void SendStringCompressed(int header, string s)
        {
            int length1 = Encoding.UTF8.GetBytes(s, m_sendProcessBuffer);
            SendCompressed(header, m_sendProcessBuffer, 0, length1);
        }

        public void SendNetBlock(NetBlock netBlock)
        {
            SendCompressed(netBlock.header, netBlock.data, 0, netBlock.length);
#if USE_NET_BLOCK_POOL
            m_netcontext.ReturnToPool(netBlock);
#endif
        }

        public void SendCompressed(int header, byte[] data, int offset, int size)
        {
            int length2 = LZ4.Encode(data, offset, m_sendBuffer, 8, size, 65528);
            BitConverter.TryWriteBytes(new Span<byte>(m_sendBuffer, 4, 4), length2);
            BitConverter.TryWriteBytes(new Span<byte>(m_sendBuffer, 0, 4), header);
            m_socket.Send(m_sendBuffer, length2 + 8, SocketFlags.None);
        }
    }
}
