using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using RTUGame.Mathematics;
using RTUGame.Net;

namespace RTUGame.Game
{
    static class NetExt
    {
        public static void ToNetBlock(this Chunk chunk, NetPack netPack)
        {
            Debug.Assert(netPack.data.Length >= Chunk.c_minimumSize);
            netPack.length = Chunk.c_minimumSize;
            chunk.position.ToBytes(new Span<byte>(netPack.data, 0, 12));
            MemoryMarshal.Cast<byte, int>(new Span<byte>(netPack.data, 12, 4))[0] = (int)chunk.chunkFlags;
            MemoryMarshal.Cast<byte, long>(new Span<byte>(netPack.data, 16, 8))[0] = chunk.stamp;
            Span<byte> span1 = MemoryMarshal.Cast<int, byte>(new Span<int>(chunk.data));
            span1.TryCopyTo(new Span<byte>(netPack.data, 64, 16384));
        }

        public static Chunk GetChunk(this NetPack pack)
        {
            Chunk chunk = new Chunk();
            chunk.position = VectorExt.Int3FromBytes(new Span<byte>(pack.data, 0, 12));
            chunk.chunkFlags = (ChunkFlag)BitConverter.ToInt32(pack.data, 12);
            chunk.stamp = BitConverter.ToInt64(pack.data, 16);
            new Span<byte>(pack.data, 64, 16384).CopyTo(MemoryMarshal.Cast<int, byte>(new Span<int>(chunk.data)));

            return chunk;
        }
    }
}
