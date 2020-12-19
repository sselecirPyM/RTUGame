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
using RTUGameServer.MathTypes;

namespace RTUGameServer.GameLogic
{
    public class Chunk
    {
        public int x;
        public int y;
        public int z;
        public int chunkFlag;
        public long stamp;

        public int[] data = new int[16 * 16 * 16];
        public const int c_minimumSize = 16448;

        public bool TryGetBlock(INT3 position, out int oValue) => TryGetBlock(position.x, position.y, position.z, out oValue);

        public bool TryGetBlock(int _x, int _y, int _z, out int oValue)
        {
            if (data == null)
            {
                oValue = 0;
                return false;
            }
            int loc_x = _x - x;
            int loc_y = _y - y;
            int loc_z = _z - z;
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