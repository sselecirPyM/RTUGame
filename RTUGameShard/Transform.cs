using System;
using System.Collections.Generic;
using System.Text;
//using SharpDX;
using System.Numerics;

namespace RTUGame.Game
{
    public struct Transform
    {
        public SharpDX.Int3 iPosition;
        public Vector3 fPosition;
        public Vector3 scale;
        public Quaternion rotation;
        public Vector3 boundingSize;
    }
}
