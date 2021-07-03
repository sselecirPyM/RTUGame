using System;
using System.Collections.Generic;
using System.Text;
using SharpDX;

namespace RTUGame.Game
{
    [Flags]
    public enum ChunkFlag
    {
        Chunk8 = 1,
        Chunk16 = 2,
        Chunk32 = 4,
    }
    public class Chunk
    {
        public Int3 position;
        public ChunkFlag chunkFlags;
        public long stamp;

        public int[] data = new int[16 * 16 * 16];
        public short[] data16;
        public byte[] data8;
        public const int c_minimumSize = 16448;

        public bool TryGetBlock(Int3 position, out int block) => TryGetBlock(position.X, position.Y, position.Z, out block);

        public bool TryGetBlock(int _x, int _y, int _z, out int block)
        {
            if (data == null && data16 == null && data8 == null)
            {
                block = 0;
                return false;
            }
            int loc_x = _x - position.X;
            int loc_y = _y - position.Y;
            int loc_z = _z - position.Z;
            if (loc_x < 0 || loc_x >= 16 || loc_y < 0 || loc_y >= 16 || loc_z < 0 || loc_z >= 16)
            {
                block = 0;
                return false;
            }
            else if (data != null)
            {
                block = data[loc_z * 256 + loc_y * 16 + loc_x];
                return block != 0;
            }
            else if (data16 != null)
            {
                block = data16[loc_z * 256 + loc_y * 16 + loc_x];
                return block != 0;
            }
            else if (data8 != null)
            {
                block = data8[loc_z * 256 + loc_y * 16 + loc_x];
                return block != 0;
            }
            else
            {
                block = 0;
                return false;
            }
        }
    }
}