using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using RTUGameServer.GameLogic;
using RTUGameServer.Net;

namespace RTUGameServer.Utility
{
    public static class GUtility
    {
        public static void ToNetBlock(Chunk chunk, NetBlock netBlock)
        {
            Debug.Assert(netBlock.data.Length >= Chunk.c_minimumSize);
            netBlock.header = 1802856547/*'kuhc'*/;
            netBlock.length = Chunk.c_minimumSize;
            BitConverter.TryWriteBytes(new Span<byte>(netBlock.data, 0, 4), chunk.x);
            BitConverter.TryWriteBytes(new Span<byte>(netBlock.data, 4, 4), chunk.y);
            BitConverter.TryWriteBytes(new Span<byte>(netBlock.data, 8, 4), chunk.z);
            Span<byte> span1 = MemoryMarshal.Cast<int, byte>(new Span<int>(chunk.data));
            span1.TryCopyTo(new Span<byte>(netBlock.data, 64, 16384));
        }
    }
}
