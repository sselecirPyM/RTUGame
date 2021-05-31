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
using System.Threading;
using System.Threading.Tasks;
using System.IO;
using System.IO.Compression;
using System.Xml;
using RTUGame.Mathematics;
using RTUGame.Game;
using System.Collections.Concurrent;
using SharpDX;
using RTUGameServer.Utility;

namespace RTUGameServer.GameLogic
{
    public class GameWorld
    {
        public IChunkGenerator chunkGenerator = new ChunkGeneratorTest();

        HashSet<Int2> _cover64 = new HashSet<Int2>();

        public GameSettings gameSettings;
        public ConcurrentDictionary<Int3, Chunk> chunks = new ConcurrentDictionary<Int3, Chunk>();
        public ConcurrentDictionary<Int2, RegionInfo> regionInfos = new ConcurrentDictionary<Int2, RegionInfo>();
        public ConcurrentDictionary<Int2, RegionInfo> regionInfo64 = new ConcurrentDictionary<Int2, RegionInfo>();
        public ConcurrentQueue<PlayerInfo> playerEnterQueue = new ConcurrentQueue<PlayerInfo>();
        public ConcurrentQueue<PlayerInfo> playerLeaveQueue = new ConcurrentQueue<PlayerInfo>();
        public Dictionary<Guid, PlayerInfo> activatedPlayers = new Dictionary<Guid, PlayerInfo>();

        public Dictionary<Guid, PlayerInfo> allPlayers = new Dictionary<Guid, PlayerInfo>();

        public long frame = 0;


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
            chunks.TryGetValue(new Int3(x, y, z), out var chunk);
            return chunk;
        }

        public Chunk GetChunk(Int3 position)
        {
            chunks.TryGetValue(position, out var chunk);
            return chunk;
        }

        public bool CastRay(Ray rayLocal, out Int3 result, out Int3 surfaceHit)
        {
            result = new Int3();
            surfaceHit = new Int3();
            return false;
        }

        public void GameService()
        {
            DateTime thisFrameTime = DateTime.Now;
            DateTime previousFrameTime = thisFrameTime;
            while (true)
            {
                thisFrameTime = DateTime.Now;
                if (thisFrameTime - previousFrameTime >= TimeSpan.FromMilliseconds(gameSettings.logicFrameInteral))
                {
                    previousFrameTime = thisFrameTime;
                    Tick();
                }
                Thread.Sleep(1);
            }
        }

        public IEnumerable<Int3> VisibleChunks(PlayerInfo playerInfo)
        {
            Int3 aPos = playerInfo.PositionI.AlignedDown(16);
            for (int x = -128; x < 144; x += 16)
                for (int z = -128; z < 144; z += 16)
                {
                    Int2 testPoint = new Int2(x + aPos.X, z + aPos.Z);
                    if (this.regionInfos.TryGetValue(testPoint, out var regionInfo))
                    {
                        for (int y = regionInfo.m_minHeight; y < regionInfo.m_maxHeight; y += 16)
                        {
                            yield return new Int3(aPos.X + x, y, aPos.Z + z);
                        }
                        playerInfo.visibleRegions[testPoint] = regionInfo.m_recentUpdateTimeStamp;
                    }
                }
        }

        void ExpandMap()
        {
            foreach (var playerInfo in activatedPlayers.Values)
            {
                Int3 p = playerInfo.PositionI;
                Int2 p1 = new Int2(p.X, p.Z).AlignedDown(64);
                for (int x = -192; x < 256; x += 64)
                    for (int y = -192; y < 256; y += 64)
                    {
                        _cover64.Add(new Int2(p1.X + x, p1.Y + y));
                    }
            }
            Parallel.ForEach(_cover64, (Int2 p64) =>
            {
                int filledCount = 0;
                if (this.regionInfo64.TryGetValue(p64, out var regionInfo64))
                {
                    if (regionInfo64.m_generatorFilled)
                        return;
                }
                else
                {
                    regionInfo64 = new RegionInfo();
                    this.regionInfo64[p64] = regionInfo64;
                }
                for (int x = 0; x < 64; x += 16)
                    for (int y = 0; y < 64; y += 16)
                    {
                        Int2 position = new Int2(p64.X + x, p64.Y + y);
                        if (!this.regionInfos.TryGetValue(position, out var regionInfo) || !regionInfo.m_generatorFilled)
                        {
                            chunkGenerator.FillRegion(position.X, position.Y, this);
                        }
                        else
                            filledCount++;
                    }
                if (filledCount == 16)
                    regionInfo64.m_generatorFilled = true;
            });
            _cover64.Clear();
        }

        void Tick()
        {
            while (playerEnterQueue.TryDequeue(out var playerInfo))
            {
                activatedPlayers[playerInfo.PlayerGuid] = playerInfo;
            }
            while (playerLeaveQueue.TryDequeue(out var playerInfo))
            {
                activatedPlayers.Remove(playerInfo.PlayerGuid);
            }
            ExpandMap();
            PlayersSync();
            frame++;
        }

        public void PlayersSync()
        {
            DateTime now = DateTime.Now;
            foreach (var playerPair in activatedPlayers)
            {
                var player = playerPair.Value;
                var user = player.currentUser;


                if (player.lastUpdateTime<player.lastActivateTime)
                {
                    player.lastUpdateTime = now;
                    foreach (var chunkPosition in VisibleChunks(player))
                    {
                        var chunk1 = GetChunk(chunkPosition);

                        if (player.visibleChunks.TryGetValue(chunkPosition, out var timeStamp1))
                        {
                            if (chunk1 != null && chunk1.stamp > timeStamp1)
                            {
                                player.visibleChunks[chunkPosition] = chunk1.stamp;
                                chunk1.DelaySend(player.currentUser);
                            }
                        }
                        else if (chunk1 != null)
                        {
                            player.visibleChunks[chunkPosition] = chunk1.stamp;
                            chunk1.DelaySend(player.currentUser);
                        }
                    }
                    player.RemoveUnusedChunks();
                }
            }
        }

        //public void Load(Stream stream)
        //{
        //    ZipArchive zipArchive = new ZipArchive(stream, ZipArchiveMode.Read);
        //}

        //public void Save(Stream stream)
        //{
        //    Dictionary<Guid, PlayerInfo> allPlayers;
        //    lock (this.allPlayers)
        //        allPlayers = new Dictionary<Guid, PlayerInfo>(this.allPlayers);
        //    string tmpPath = Path.GetTempPath() + "/RTUGameServer/";
        //    var info = Directory.CreateDirectory(tmpPath + Guid.NewGuid());

        //    using (XmlWriter xmlWriter = XmlWriter.Create(info.FullName + "/settings.xml"))
        //    {
        //        xmlWriter.WriteStartDocument();
        //        xmlWriter.WriteStartElement("settings");
        //        xmlWriter.WriteEndElement();
        //    }
        //    using (XmlWriter xmlWriter = XmlWriter.Create(info.FullName + "/players.xml"))
        //    {
        //        xmlWriter.WriteStartDocument();
        //        xmlWriter.WriteStartElement("players");
        //        xmlWriter.WriteEndElement();
        //    }
        //    System.IO.File.Delete(tmpPath + "/save.zip");
        //    ZipFile.CreateFromDirectory(info.FullName, tmpPath + "/save.zip");
        //    System.IO.Directory.Delete(info.FullName, true);
        //}
    }
}
