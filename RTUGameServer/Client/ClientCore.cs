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
using RTUGameServer.GameLogic;
using RTUGameServer.MathTypes;

namespace RTUGameServer.Client
{
    public class ClientCore
    {
        public PlayerInfo playerInfo = null;

        public void TrackVisibleChunks(GameContext gameContext)
        {
            INT3 aPos = playerInfo.positionI.AlignedDown(16);
            for (int x = -128; x < 144; x += 16)
                for (int z = -128; z < 144; z += 16)
                {
                    INT2 testPoint = new INT2(x + aPos.x, z + aPos.z);

                    if (gameContext.regionInfo.TryGetValue(testPoint, out var regionInfo))
                    {
                        for (int y = regionInfo.m_minHeight; y < regionInfo.m_maxHeight; y += 16)
                        {
                            var chunkPosition = new INT3(aPos.x + x, y, aPos.z + z);
                            //var chunk1 = gameCore.GetExistChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z);
                            //if (playerInfo.visibleChunks.TryGetValue(chunkPosition, out var timeStamp1))
                            //{
                            //    if (chunk1 != null && chunk1.stamp > timeStamp1)
                            //    {
                            //        playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;
                            //        NetBlock netBlock = netContext.GetNetBlock(1802856547/*'kuhc'*/, Chunk.c_minimumSize);
                            //        GUtility.ToNetBlock(chunk1, netBlock);
                            //        wrapLayout.SendNetBlock(netBlock);
                            //    }
                            //}
                            //else if (chunk1 != null)
                            //{
                            //    playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;

                            //    NetBlock netBlock = netContext.GetNetBlock(1802856547/*'kuhc'*/, Chunk.c_minimumSize);
                            //    GUtility.ToNetBlock(chunk1, netBlock);
                            //    wrapLayout.SendNetBlock(netBlock);
                            //}
                        }
                        playerInfo.visibleRegions[testPoint] = regionInfo.m_recentUpdateTimeStamp;
                    }

                }

            List<INT3> chunkCullResult = new List<INT3>();
            foreach (var pair in playerInfo.visibleChunks)
            {
                INT3 a = pair.Key - playerInfo.positionI + new INT3(8, 8, 8);
                if (Math.Abs(a.x) + Math.Abs(a.z) > 512)
                {
                    chunkCullResult.Add(pair.Key);
                }
            }
            for (int j = 0; j < chunkCullResult.Count; j++)
            {
                playerInfo.visibleChunks.Remove(chunkCullResult[j]);
            }
        }
    }
}
