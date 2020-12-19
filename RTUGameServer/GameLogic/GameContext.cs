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
using System.Numerics;
using RTUGameServer.MathTypes;
using System.Collections.Concurrent;

namespace RTUGameServer.GameLogic
{
    public class GameContext
    {
        public GameSettings gameSettings;
        public ConcurrentDictionary<INT3, Chunk> chunks = new ConcurrentDictionary<INT3, Chunk>();
        public ConcurrentDictionary<INT2, RegionInfo> regionInfo = new ConcurrentDictionary<INT2, RegionInfo>();
        public ConcurrentDictionary<INT2, RegionInfo64> regionInfo64 = new ConcurrentDictionary<INT2, RegionInfo64>();
        public List<PlayerInfo> L_playerEnterList = new List<PlayerInfo>();
        public List<PlayerInfo> L_playerLeaveList = new List<PlayerInfo>();
        public Dictionary<Guid,PlayerInfo> activatedPlayers = new Dictionary<Guid, PlayerInfo>();
        public Dictionary<Guid, PlayerInfo> allPlayers = new Dictionary<Guid, PlayerInfo>();


        public void DefaultSettings()
        {
            gameSettings = new GameSettings();
            gameSettings.InitAsDefault();
        }

        public bool TryGetBlock(int x, int y, int z, out int oValue)
        {
            Chunk chunk = GetChunk(x, y, z);
            if (chunk != null)
            {
                return chunk.TryGetBlock(x, y, z, out oValue);
            }
            else
            {
                oValue = 0;
                return false;
            }
        }

        public Chunk GetChunk(int x, int y, int z)
        {
            chunks.TryGetValue(new INT3(x, y, z), out var chunk);
            return chunk;
        }

        public bool CastRay(int filter,out INT3 result,out INT3 surfaceHit)
        {
            result = new INT3();
            surfaceHit = new INT3();
            return false;
        }
    }
}
