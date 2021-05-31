using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Direct3D12;

namespace RTUGame1.Graphics
{
    public enum RootSignatureParamP
    {
        CBV,
        SRV,
        UAV,
        CBVTable,
        SRVTable,
        UAVTable,
    }
    public class RootSignature :IDisposable
    {
        public Dictionary<int, int> cbv = new Dictionary<int, int>();
        public Dictionary<int, int> srv = new Dictionary<int, int>();
        public Dictionary<int, int> uav = new Dictionary<int, int>();
        public ID3D12RootSignature rootSignature;
        public string Name;

        public void Dispose()
        {
            rootSignature?.Dispose();
            rootSignature = null;
        }
    }
}
