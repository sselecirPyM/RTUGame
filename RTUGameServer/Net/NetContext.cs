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

//#define USE_NET_BLOCK_POOL
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace RTUGameServer.Net
{
    public class NetContext
    {
        public List<NetBlock> l_receivedBlock_w = new List<NetBlock>();
        public List<NetBlock> m_receivedBlock_r = new List<NetBlock>();
        public object lock_receivedBlock = new object();
        public AutoResetEvent receiveEvent = new AutoResetEvent(false);
        public AutoResetEvent sendEvent = new AutoResetEvent(false);
        public CancellationTokenSource m_stopAll = new CancellationTokenSource();

        const int c_blockSize17k = 17400;
        const int c_blockSize64k = 65528;

        public ConcurrentStack<NetBlock> pool17k = new ConcurrentStack<NetBlock>();
        public ConcurrentStack<NetBlock> pool64k = new ConcurrentStack<NetBlock>();

        public NetBlock GetNetBlock(int header, int size)
        {
            NetBlock block;
#if USE_NET_BLOCK_POOL
            if (size == 0)
            {
                block = new NetBlock();
            }
            else if (size <= c_blockSize17k)
            {
                if (!pool17k.TryPop(out block))
                {
                    block = new NetBlock();
                    block.data = new byte[c_blockSize17k];
                }
            }
            else if (size <= c_blockSize64k)
            {
                if (!pool64k.TryPop(out block))
                {
                    block = new NetBlock();
                    block.data = new byte[c_blockSize64k];
                }
            }
            else
            {
                throw new ArgumentOutOfRangeException("the net block size out of range.");
            }
#else
            block = new NetBlock();
            block.data = new byte[size];
#endif
            block.header = header;
            block.length = size;
            return block;
        }

        public void ReturnToPool(NetBlock netBlock)
        {
            if (netBlock.data != null)
                switch (netBlock.data.Length)
                {
                    case c_blockSize17k:
                        if (pool17k.Count < 16)
                            pool17k.Push(netBlock);
                        break;
                    case c_blockSize64k:
                        if (pool64k.Count < 16)
                            pool64k.Push(netBlock);
                        break;
                    default:
                        break;
                }
        }

        public void SwapDynamicContext()
        {
#if USE_NET_BLOCK_POOL
            for (int i = 0; i < m_receivedBlock_r.Count; i++)
            {
                ReturnToPool(m_receivedBlock_r[i]);
            }
#endif
            m_receivedBlock_r.Clear();
            lock (lock_receivedBlock)
            {
                var temp = l_receivedBlock_w;
                l_receivedBlock_w = m_receivedBlock_r;
                m_receivedBlock_r = temp;
            }
        }

        public void Log(object message)
        {
            Console.WriteLine(string.Format("{0} {1}", DateTime.Now, message));
        }
    }
}
