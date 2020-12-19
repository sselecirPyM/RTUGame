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
    public static class PerlinNoise
    {
        public static float Noise(float x)
        {
            int X = (int)MathF.Floor(x) & 0xff;
            x -= MathF.Floor(x);
            var u = Fade(x);
            return Lerp(u, Grad(perm[X], x), Grad(perm[X + 1], x - 1)) * 2;
        }

        public static float Noise(float x, float y)
        {
            var x1 = MathF.Floor(x);
            var y1 = MathF.Floor(y);
            int _x = (int)x1 & 0xff;
            int _y = (int)y1 & 0xff;
            x -= x1;
            y -= y1;
            var u = Fade(x);
            var v = Fade(y);
            int A = (perm[_x] + _y) & 0xff;
            int B = (perm[_x + 1] + _y) & 0xff;
            return Lerp(v, Lerp(u, Grad(perm[A], x, y), Grad(perm[B], x - 1, y)),
                           Lerp(u, Grad(perm[A + 1], x, y - 1), Grad(perm[B + 1], x - 1, y - 1)));
        }

        public static float Noise(Vector2 pos)
        {
            return Noise(pos.X, pos.Y);
        }

        public static float Noise(float x, float y, float z)
        {
            var x1 = MathF.Floor(x);
            var y1 = MathF.Floor(y);
            var z1 = MathF.Floor(z);
            int _x = (int)x1 & 0xff;
            int _y = (int)y1 & 0xff;
            int _z = (int)z1 & 0xff;
            x -= x1;
            y -= y1;
            z -= z1;
            var u = Fade(x);
            var v = Fade(y);
            var w = Fade(z);
            int A = (perm[_x] + _y) & 0xff;
            int B = (perm[_x + 1] + _y) & 0xff;
            int AA = (perm[A] + _z) & 0xff;
            int BA = (perm[B] + _z) & 0xff;
            int AB = (perm[A + 1] + _z) & 0xff;
            int BB = (perm[B + 1] + _z) & 0xff;
            return Lerp(w, Lerp(v, Lerp(u, Grad(perm[AA], x, y, z), Grad(perm[BA], x - 1, y, z)),
                                   Lerp(u, Grad(perm[AB], x, y - 1, z), Grad(perm[BB], x - 1, y - 1, z))),
                           Lerp(v, Lerp(u, Grad(perm[AA + 1], x, y, z - 1), Grad(perm[BA + 1], x - 1, y, z - 1)),
                                   Lerp(u, Grad(perm[AB + 1], x, y - 1, z - 1), Grad(perm[BB + 1], x - 1, y - 1, z - 1))));
        }

        public static float Noise(Vector3 pos)
        {
            return Noise(pos.X, pos.Y, pos.Z);
        }

        public static float Fbm(float x, int octave)
        {
            var f = 0.0f;
            var w = 0.5f;
            for (int i = 0; i < octave; i++)
            {
                f += w * Noise(x);
                x *= 2.0f;
                w *= 0.5f;
            }
            return f;
        }

        public static float Fbm(Vector2 pos, int octave)
        {
            var f = 0.0f;
            var w = 0.5f;
            for (int i = 0; i < octave; i++)
            {
                f += w * Noise(pos);
                pos *= 2.0f;
                w *= 0.5f;
            }
            return f;
        }

        public static float Fbm(float x, float y, int octave)
        {
            return Fbm(new Vector2(x, y), octave);
        }

        public static float Fbm(Vector3 pos, int octave)
        {
            var f = 0.0f;
            var w = 0.5f;
            for (int i = 0; i < octave; i++)
            {
                f += w * Noise(pos);
                pos *= 2.0f;
                w *= 0.5f;
            }
            return f;
        }

        public static float Fbm(float x, float y, float z, int octave)
        {
            return Fbm(new Vector3(x, y, z), octave);
        }


        static float Fade(float t)
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        static float Lerp(float t, float a, float b)
        {
            return a + t * (b - a);
        }

        static float Grad(int hash, float x)
        {
            return (hash & 1) == 0 ? x : -x;
        }

        static float Grad(int hash, float x, float y)
        {
            return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
        }

        static float Grad(int hash, float x, float y, float z)
        {
            int h = hash & 0xf;
            var u = h < 8 ? x : y;
            var v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }

        static readonly byte[] perm =
        {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
            140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
            247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
            57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
            74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
            60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
            65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
            200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,
            52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,
            206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,
            248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,
            22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
            246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,
            51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,
            84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,
            114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,
        };

    }
}
