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
using RTUGameServer.MathTypes;
using System.IO;
using System.IO.Compression;
using System.Xml;

namespace RTUGameServer.GameLogic
{
    public class GameCore
    {
        public IChunkGenerator chunkGenerator = new ChunkGeneratorTest();
        public GameContext gameContext = new GameContext();

        HashSet<INT2> _cover64 = new HashSet<INT2>();

        public Chunk GetExistChunk(int x, int y, int z)
        {
            return gameContext.GetChunk(x, y, z);
        }
        public void GameService()
        {
            DateTime thisFrameTime = DateTime.Now;
            DateTime previousFrameTime = thisFrameTime;
            while (true)
            {
                thisFrameTime = DateTime.Now;
                if (thisFrameTime - previousFrameTime > TimeSpan.FromMilliseconds(gameContext.gameSettings.logicFrameInteral))
                {
                    previousFrameTime = thisFrameTime;
                    GameFrame();
                }
                Thread.Sleep(1);
            }
        }

        void ExpandMap()
        {
            foreach (var playerInfo in gameContext.activatedPlayers.Values)
            {
                INT3 p = playerInfo.positionI;
                INT2 p1 = new INT2(p.x, p.z).AlignedDown(64);
                for (int x = -192; x < 256; x += 64)
                    for (int y = -192; y < 256; y += 64)
                    {
                        _cover64.Add(new INT2(p1.x + x, p1.y + y));
                    }
            }
            Parallel.ForEach(_cover64, (INT2 p64) =>
            {
                int filledCount = 0;
                if (gameContext.regionInfo64.TryGetValue(p64, out var regionInfo64))
                {
                    if (regionInfo64.m_fullGenerated)
                        return;
                }
                else
                {
                    regionInfo64 = new RegionInfo64();
                    gameContext.regionInfo64[p64] = regionInfo64;
                }
                for (int x = 0; x < 64; x += 16)
                    for (int y = 0; y < 64; y += 16)
                    {
                        INT2 position = new INT2(p64.x + x, p64.y + y);
                        if (!gameContext.regionInfo.TryGetValue(position, out var regionInfo) || !regionInfo.m_generatorFilled)
                        {
                            chunkGenerator.FillRegion(position.x, position.y, gameContext);
                        }
                        else
                            filledCount++;
                    }
                if (filledCount == 16)
                    regionInfo64.m_fullGenerated = true;
            });
            _cover64.Clear();
        }

        void GameFrame()
        {
            lock (gameContext.L_playerEnterList)
            {
                foreach (var playerInfo in gameContext.L_playerEnterList)
                    gameContext.activatedPlayers[playerInfo.playerGuid] = playerInfo;
                gameContext.L_playerEnterList.Clear();
            }
            lock (gameContext.L_playerLeaveList)
            {
                foreach (var playerInfo in gameContext.L_playerLeaveList)
                    gameContext.activatedPlayers.Remove(playerInfo.playerGuid);
                gameContext.L_playerLeaveList.Clear();
            }
            ExpandMap();

        }

        public void Load(Stream stream)
        {
            ZipArchive zipArchive = new ZipArchive(stream, ZipArchiveMode.Read);
        }

        public void Save(Stream stream)
        {
            Dictionary<Guid, PlayerInfo> allPlayers;
            lock (gameContext.allPlayers)
                allPlayers = new Dictionary<Guid, PlayerInfo>(gameContext.allPlayers);
            string tmpPath=Path.GetTempPath()+ "/RTUGameServer/";
            var info = Directory.CreateDirectory(tmpPath + Guid.NewGuid());

            using (XmlWriter xmlWriter = XmlWriter.Create(info.FullName + "/settings.xml"))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("settings");
                xmlWriter.WriteEndElement();
            }
            using (XmlWriter xmlWriter = XmlWriter.Create(info.FullName + "/players.xml"))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("players");
                xmlWriter.WriteEndElement();
            }
            System.IO.File.Delete(tmpPath + "/save.zip");
            ZipFile.CreateFromDirectory(info.FullName, tmpPath + "/save.zip");
            System.IO.Directory.Delete(info.FullName,true);
        }
    }
}
