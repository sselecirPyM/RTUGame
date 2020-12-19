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
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace RTUGameServer.Compression
{
    public static class LZ4
    {
        private const string c_dllName = "LZ4";

        static bool LibLoaded = false;
        [DllImport("kernel32")]
        private static extern IntPtr LoadLibraryA([MarshalAs(UnmanagedType.LPStr)] string fileName);

        public static void LoadLib()
        {
            if (LibLoaded) throw new Exception();
            var dllFile = Path.Combine(Environment.Is64BitProcess ? "x64" : "x86", c_dllName);
            IntPtr ret = LoadLibraryA(dllFile);
            if (ret.ToInt64() == 0)
            {
                throw new DllNotFoundException("Unable to load DLL 'LZ4'.");
            }
            LibLoaded = true;
        }

        [DllImport(c_dllName, EntryPoint = "LZ4_compress_default")]
        public static extern int Encode(byte[] source, byte[] destination, int sourceSize, int destinationLimit);

        [DllImport(c_dllName, EntryPoint = "LZ4_compress_default")]
        public static extern int Encode(IntPtr source, IntPtr destination, int sourceSize, int destinationLimit);

        public static int Encode(byte[] source, int sourceOffset, byte[] destination, int destOffset, int sourceSize, int destinationLimit)
        {
            return Encode(Marshal.UnsafeAddrOfPinnedArrayElement(source, sourceOffset), Marshal.UnsafeAddrOfPinnedArrayElement(destination, destOffset), sourceSize, destinationLimit);
        }

        [DllImport(c_dllName, EntryPoint = "LZ4_decompress_safe")]
        public static extern int Decode(byte[] source, byte[] destination, int sourceSize, int destinationLimit);

        [DllImport(c_dllName, EntryPoint = "LZ4_decompress_safe")]
        public static extern int Decode(IntPtr source, IntPtr destination, int sourceSize, int destinationLimit);

        public static int Decode(byte[] source, int sourceOffset, byte[] destination, int destOffset, int sourceSize, int destinationLimit)
        {
            return Decode(Marshal.UnsafeAddrOfPinnedArrayElement(source, sourceOffset), Marshal.UnsafeAddrOfPinnedArrayElement(destination, destOffset), sourceSize, destinationLimit);
        }
    }
}
