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
using System.Numerics;
using System.Text;

namespace RTUGameServer.MathTypes
{
    public static class MathTypeExtension
    {
        public static Vector2 ToVector2(this INT2 i)
        {
            return new Vector2(i.x, i.y);
        }
        public static Vector3 ToVector3(this INT3 i)
        {
            return new Vector3(i.x, i.y, i.z);
        }
    }
}
