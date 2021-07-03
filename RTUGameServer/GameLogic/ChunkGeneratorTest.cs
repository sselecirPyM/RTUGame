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
using System;
using System.Collections.Generic;
using System.Text;
using RTUGame.Game;
using RTUGameServer.MathTypes;
using SharpDX;

namespace RTUGameServer.GameLogic
{
    public class ChunkGeneratorTest : IChunkGenerator
    {
        public Chunk GetChunk(int x, int y, int z, World world)
        {
            Chunk chunk = world.GetChunk(x, y, z);
            if (chunk != null)
            {
                return chunk;
            }
            int maxHeight = int.MinValue;
            int[] table = new int[16 * 16];
            for (int rz = 0; rz < 16; rz++)
                for (int rx = 0; rx < 16; rx++)
                {
                    int val = (int)(PerlinNoise.Noise((rx + x) * (1 / 256.0f), (rz + z) * (1 / 256.0f)) * 32) + 16;
                    table[rz * 16 + rx] = val;
                    if (maxHeight < val)
                        maxHeight = val;

                }
            if (y > maxHeight)
                return null;

            chunk = new Chunk();
            chunk.stamp = 1;
            chunk.position = new Int3(x,y,z);
            for (int rz = 0; rz < 16; rz++)
                for (int ry = 0; ry < 16; ry++)
                    for (int rx = 0; rx < 16; rx++)
                    {
                        if ((ry + y) < table[rz * 16 + rx])
                        {
                            if ((ry + y) < 0)
                                chunk.data[rz * 256 + ry * 16 + rx] = 2;
                            else
                                chunk.data[rz * 256 + ry * 16 + rx] = 1;
                        }
                        else
                        {
                            chunk.data[rz * 256 + ry * 16 + rx] = 0;
                        }
                    }

            world.chunks[new Int3(x, y, z)] = chunk;
            return chunk;
        }

        public void FillRegion(int x, int z, World gameContext)
        {
            Int2 position = new Int2(x, z);
            for (int i = -2; i < 4; i++)
            {
                GetChunk(x, i * 16, z, gameContext);
            }
            var regionInfo = GetRegionInfo(x, z, gameContext);
            regionInfo.m_generatorFilled = true;
            gameContext.regionInfos[position] = regionInfo;
        }

        public RegionInfo GetRegionInfo(int x, int z, World gameContext)
        {
            RegionInfo regionInfo = new RegionInfo();
            regionInfo.m_minHeight = -32;
            regionInfo.m_maxHeight = 64;
            return regionInfo;
        }
    }
}
