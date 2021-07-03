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
using System.Text.Json;
using System.Text.Json.Serialization;
using Int3 = SharpDX.Int3;
using Int2 = SharpDX.Int2;

namespace RTUGameServer.GameLogic
{
    public class PlayerInfo
    {
        public Guid PlayerGuid { get; set; }
        public string Name { get; set; }
        public string Email { get; set; }
        public byte[] PasswordHash { get; set; }
        public long RegisterTime { get; set; }
        public long TotalPlayTime { get; set; }

        public long EntityID { get; set; }
        public Int3 PositionI { get; set; }
        public Vector3 PositionF { get; set; }
        public Quaternion Rotation { get; set; }

        public User currentUser;
        public DateTime lastActivateTime;
        public DateTime lastUpdateTime;

        [JsonExtensionData]
        Dictionary<string, JsonElement> additionalData { get; set; }

        public Dictionary<Int3, long> visibleChunks = new Dictionary<Int3, long>();
        public Dictionary<Int2, long> visibleRegions = new Dictionary<Int2, long>();

        public void RemoveUnusedChunks()
        {
            List<Int3> chunkCullResult = new List<Int3>();
            foreach (var pair in visibleChunks)
            {
                Int3 a = pair.Key - PositionI + new Int3(8, 8, 8);
                if (Math.Abs(a.X) + Math.Abs(a.Z) > 512)
                    chunkCullResult.Add(pair.Key);
            }
            for (int j = 0; j < chunkCullResult.Count; j++)
                visibleChunks.Remove(chunkCullResult[j]);
        }
    }
}
