using System;
using System.Collections.Generic;
using System.Text;
using SharpDX;
using RTUGame.Game;
using RTUGame.Mathematics;

namespace RTUGame1.Game
{
    public class World
    {
        public Dictionary<Int3, Chunk> chunks = new Dictionary<Int3, Chunk>();
        public Dictionary<long, Entity> entities = new Dictionary<long, Entity>();

        public bool TryGetBlock(Int3 position, out int block)
        {
            Int3 chunkPos = position.AlignedDown(16);
            if (chunks.TryGetValue(chunkPos, out var chunk))
            {
                return chunk.TryGetBlock(chunkPos, out block);
            }
            else
            {
                block = 0;
                return false;
            }
        }
    }
}
