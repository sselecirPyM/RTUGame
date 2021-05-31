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
        public const int c_minimumSize = 16448;

        public bool TryGetBlock(Int3 position, out int oValue) => TryGetBlock(position.X, position.Y, position.Z, out oValue);

        public bool TryGetBlock(int _x, int _y, int _z, out int oValue)
        {
            if (data == null)
            {
                oValue = 0;
                return false;
            }
            int loc_x = _x - position.X;
            int loc_y = _y - position.Y;
            int loc_z = _z - position.Z;
            if (loc_x < 0 || loc_x >= 16 || loc_y < 0 || loc_y >= 16 || loc_z < 0 || loc_z >= 16)
            {
                oValue = 0;
                return false;
            }
            else
            {
                oValue = data[loc_z * 256 + loc_y * 16 + loc_x];
                return oValue != 0;
            }
        }
    }
}