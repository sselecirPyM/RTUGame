using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Direct3D12;

namespace RTUGame1.Graphics
{
    public class UploadBuffer : IDisposable
    {
        public ID3D12Resource resource;
        public int size;

        public void Dispose()
        {
            resource?.Dispose();
        }
    }
}
