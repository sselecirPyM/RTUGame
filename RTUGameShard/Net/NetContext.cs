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

using K4os.Compression.LZ4;
using RTUGame.Net;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using RTUGame.Collections;

namespace RTUGame.Net
{
    public class NetContext
    {
        public FlipList<NetPack> receivedBlocks = new FlipList<NetPack>();
        public AutoResetEvent receiveEvent = new AutoResetEvent(false);
        public CancellationTokenSource cancel = new CancellationTokenSource();

        public NetPack GetNetBlock(int header, int size)
        {
            NetPack block;
            block = new NetPack();
            block.data = new byte[size];
            block.header = header;
            block.length = size;
            return block;
        }

        public void Log(object message)
        {
            Console.WriteLine(string.Format("{0} {1}", DateTime.Now, message));
        }


        Socket socket;
        byte[] m_receiveBuffer = new byte[65536 * 2];
        byte[] m_receiveProcessBuffer = new byte[65536];
        byte[] m_sendBuffer = new byte[65536];
        byte[] m_sendProcessBuffer = new byte[65536];
        int m_receiveDataLength = 0;
        public void Init(Socket socket)
        {
            this.socket = socket;
        }
        const int c_metaLength = 8;
        public void ReceiveService()
        {
            try
            {
                while (true)
                {
                    int length = socket.Receive(m_receiveBuffer, m_receiveDataLength, 65536, SocketFlags.None);
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
                            int decodedSize = LZ4Codec.Decode(new Span<byte>(m_receiveBuffer, peekIndex + c_metaLength, blockSize), new Span<byte>(m_receiveProcessBuffer, 0, 65536));
                            if (decodedSize > 0)
                            {
                                NetPack block = GetNetBlock(BitConverter.ToInt32(m_receiveBuffer, peekIndex), decodedSize);
                                Array.Copy(m_receiveProcessBuffer, block.data, decodedSize);
                                receivedBlocks.Add(block);
                            }
                        }
                        else
                        {
                            receivedBlocks.Add(GetNetBlock(BitConverter.ToInt32(m_receiveBuffer, peekIndex), 0));
                        }
                        peekIndex += blockSize + c_metaLength;
                        m_receiveDataLength -= blockSize + c_metaLength;
                        receiveEvent.Set();
                    }
                    Array.ConstrainedCopy(m_receiveBuffer, peekIndex, m_receiveBuffer, 0, m_receiveDataLength);
                }
            }
            catch (SocketException)
            {

            }
            catch (Exception e)
            {
                Log(e);
            }
            finally
            {
                receiveEvent.Set();
            }
        }

        public void SendStringCompressed(int header, string s)
        {
            int length1 = Encoding.UTF8.GetBytes(s, m_sendProcessBuffer);
            SendCompressed(header, m_sendProcessBuffer, 0, length1);
        }

        public void Send(NetPack netPack)
        {
            SendCompressed(netPack.header, netPack.data, 0, netPack.length);
        }

        public void SendCompressed(int header, byte[] data, int offset, int size)
        {
            int encLength = LZ4Codec.Encode(new Span<byte>(data, offset, size), new Span<byte>(m_sendBuffer, 8, 65528));
            BitConverter.TryWriteBytes(new Span<byte>(m_sendBuffer, 4, 4), encLength);
            BitConverter.TryWriteBytes(new Span<byte>(m_sendBuffer, 0, 4), header);
            socket.Send(m_sendBuffer, encLength + 8, SocketFlags.None);
        }
    }
}
