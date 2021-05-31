using RTUGame.Game;
using RTUGame.Mathematics;
using SharpDX;
using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGame.Game
{
    public class Node444
    {
        public Int3 basePosition;
        public Chunk[] chunks = new Chunk[64];
        public Chunk ChunkGlobal(Int3 globalPosition)
        {
            Int3 localPos = (globalPosition - basePosition).AlignedDown(16);
            int index = (localPos.X >> 4) + (localPos.Y >> 4) * 4 + (localPos.Z >> 4) * 16;
            return chunks[index];
        }
        public void AddChunkByIndex(int x,int y,int z, Chunk chunk)
        {
            chunks[z * 16 + y * 4 + x] = chunk;
        }
    }
}
