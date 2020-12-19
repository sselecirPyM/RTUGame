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

namespace RTUGameServer.MathTypes
{
    public struct INT3
    {
        public int x;
        public int y;
        public int z;
        public INT3(int x, int y, int z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public INT3 Aligned(int alignment)
        {
            int as1 = alignment - 1;
            int as2 = ~as1;

            return new INT3((x + as1) & as2, (y + as1) & as2, (z + as1) & as2);
        }

        public INT3 AlignedDown(int alignment)
        {
            int as1 = ~(alignment - 1);
            return new INT3(x & as1, y & as1, z & as1);
        }

        public static INT3 operator +(INT3 left, INT3 right)
        {
            return new INT3(left.x + right.x, left.y + right.y, left.z + right.z);
        }

        public static INT3 operator -(INT3 left, INT3 right)
        {
            return new INT3(left.x - right.x, left.y - right.y, left.z - right.z);
        }
    }
}
