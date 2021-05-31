using System;
using System.Collections.Generic;
using System.Text;
using SharpDX;

namespace RTUGame.Mathematics
{
    public static class VectorExt
    {
        public static Int4 AlignedDown(this Int4 s, int alignment)
        {
            int as1 = ~(alignment - 1);
            return new Int4(s.X & as1, s.Y & as1, s.Z & as1, s.W & as1);
        }

        public static Int3 AlignedDown(this Int3 s, int alignment)
        {
            int as1 = ~(alignment - 1);
            return new Int3(s.X & as1, s.Y & as1, s.Z & as1);
        }

        public static Int2 AlignedDown(this Int2 s, int alignment)
        {
            int as1 = ~(alignment - 1);
            return new Int2(s.X & as1, s.Y & as1);
        }

        public static Int3 Int3FromBytes(Span<byte> bytes)
        {
            return new Int3(BitConverter.ToInt32(bytes.Slice(0, 4)),BitConverter.ToInt32(bytes.Slice(4, 4)),BitConverter.ToInt32(bytes.Slice(8, 4)));
        }

        public static System.Numerics.Vector3 Vector3FromBytes(Span<byte> bytes)
        {
            return new System.Numerics.Vector3(BitConverter.ToSingle(bytes.Slice(0,4)), BitConverter.ToSingle(bytes.Slice(4, 4)), BitConverter.ToSingle(bytes.Slice(8, 4)));
        }

        public static System.Numerics.Quaternion QuaternionFromBytes(Span<byte> bytes)
        {
            return new System.Numerics.Quaternion(BitConverter.ToSingle(bytes.Slice(0,4)), BitConverter.ToSingle(bytes.Slice(4, 4)), BitConverter.ToSingle(bytes.Slice(8, 4)), BitConverter.ToSingle(bytes.Slice(12, 4)));
        }

        public static void ToBytes(this Int3 s, Span<byte> bytes)
        {
            BitConverter.TryWriteBytes(bytes.Slice(0, 4), s.X);
            BitConverter.TryWriteBytes(bytes.Slice(4, 4), s.Y);
            BitConverter.TryWriteBytes(bytes.Slice(8, 4), s.Z);
        }
    }
}
