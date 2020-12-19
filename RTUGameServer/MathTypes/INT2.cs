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
    public struct INT2
    {
        public int x;
        public int y;
        public INT2(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public INT2 Aligned(int alignment)
        {
            int as1 = alignment - 1;
            int as2 = ~as1;

            return new INT2((x + as1) & as2, (y + as1) & as2);
        }

        public INT2 AlignedDown(int alignment)
        {
            int as1 = ~(alignment - 1);
            return new INT2(x & as1, y & as1);
        }

        public static INT2 operator +(INT2 left,INT2 right)
        {
            return new INT2(left.x + right.x, left.y + right.y);
        }

        public static INT2 operator *(INT2 left,int right)
        {
            return new INT2(left.x * right, left.y * right);
        }

        public static INT2 operator *(int left,INT2 right)
        {
            return new INT2(left * right.x, left * right.y);
        }
    }
}
