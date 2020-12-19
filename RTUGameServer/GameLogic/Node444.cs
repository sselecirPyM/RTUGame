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
using RTUGameServer.MathTypes;
using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGameServer.GameLogic
{
    public class Node444
    {
        public INT3 basePosition;
        public Chunk[] chunks = new Chunk[64];
        public Chunk ChunkGlobal(INT3 globalPosition)
        {
            INT3 localPos = (globalPosition - basePosition).AlignedDown(16);
            int index = (localPos.x >> 4) + (localPos.y >> 4) * 4 + (localPos.z >> 4) * 16;
            return chunks[index];
        }
        public void AddChunkByIndex(int x,int y,int z, Chunk chunk)
        {
            chunks[z * 16 + y * 4 + x] = chunk;
        }
    }
}
