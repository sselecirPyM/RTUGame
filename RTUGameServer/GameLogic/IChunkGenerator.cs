﻿//Copyright (c) 2021 sselecirPyM
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
using RTUGame.Game;
using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGameServer.GameLogic
{
    public interface IChunkGenerator
    {
        public Chunk GetChunk(int x, int y, int z, World gameContext);
        public void FillRegion(int x, int z, World gameContext);
        public RegionInfo GetRegionInfo(int x, int y, World gameContext);
    }
}
