using System;
using System.Collections.Generic;
using System.Text;
using SharpDX;
using RTUGame.Game;

namespace RTUGame1.Game
{
    public class World
    {
        public Dictionary<Int3, Chunk> chunks = new Dictionary<Int3, Chunk>();
    }
}
