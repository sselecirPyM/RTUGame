using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGame.Net
{
    public enum NetMessageType : int
    {
        Chunk = 1802856547,
        Move = 1702260589,
        License = 1701013868,
    }
    public class NetPack
    {
        public int header;
        public int length;
        public byte[] data;
        public NetPack()
        {

        }
        public NetPack(Span<byte> data1, int header)
        {
            data = new byte[data1.Length];
            this.header = header;
            length = data1.Length;
            data1.CopyTo(data);
        }
        public NetPack(int header,int size)
        {
            this.header = header;
            data = new byte[size];
            length = size;
        }
    }
}
