using System;
using System.Collections.Generic;
using System.Text;
using Vortice.DXGI;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace RTUGame1.Graphics
{
    public class GPUUpload
    {
        public byte[] vertexData;
        public byte[] indexData;
        public byte[] textureData;
        public string name;
        public Format format;
        public int stride;
        public Mesh mesh;
        public Texture2D texture2D;
        public void Quad()
        {
            Vector4[] position = new Vector4[]
            {
                new Vector4(-1, -1, 0.0f,1),
                new Vector4(-1, 1, 0.0f,1),
                new Vector4(1, -1, 0.0f,1),
                new Vector4(1, 1, 0.0f,1),
            };
            int[] index = new int[]
            {
                0,1,2,
                2,1,3,
            };
            int sizeofVector4 = 16;
            vertexData = new byte[position.Length * sizeofVector4];
            indexData = new byte[index.Length * sizeof(int)];
            MemCpy(Marshal.UnsafeAddrOfPinnedArrayElement(vertexData, 0), position, (uint)vertexData.Length);
            MemCpy(Marshal.UnsafeAddrOfPinnedArrayElement(indexData, 0), index, (uint)indexData.Length);
            stride = 16;
            format = Format.R32_UInt;
        }

        public static unsafe void MemCpy<T>(IntPtr destination, T[] source, uint count)
            where T : struct
        {
            GCHandle gcHandle = GCHandle.Alloc(source, GCHandleType.Pinned);
            Unsafe.CopyBlock((void*)destination, (void*)gcHandle.AddrOfPinnedObject(), count);
            gcHandle.Free();
        }

        public static unsafe void MemCpy<T>(IntPtr destination, T source, uint count)
        {
            IntPtr sourcePtr = (IntPtr)Unsafe.AsPointer(ref source);
            Unsafe.CopyBlock((void*)destination, (void*)sourcePtr, count);
        }
    }
}
