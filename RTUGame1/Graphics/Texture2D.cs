using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Direct3D12;
using Vortice.DXGI;

namespace RTUGame1.Graphics
{
    public class Texture2D : IDisposable
    {
        public ID3D12Resource resource;
        public string Name;
        public ResourceStates resourceStates;
        public ID3D12DescriptorHeap renderTargetView;
        public ID3D12DescriptorHeap depthStencilView;
        public int width;
        public int height;
        public int mipLevels;
        public Format format;
        public Format rtvFormat;
        public Format dsvFormat;
        public Format uavFormat;

        public void StateChange(ID3D12GraphicsCommandList commandList, ResourceStates states)
        {
            if (states != resourceStates)
            {
                commandList.ResourceBarrierTransition(resource, resourceStates, states);
                resourceStates = states;
            }
            else if (states == ResourceStates.UnorderedAccess)
            {
                commandList.ResourceBarrierUnorderedAccessView(resource);
            }
        }

        public void Dispose()
        {
            resource?.Dispose();
            resource = null;
            renderTargetView?.Dispose();
            renderTargetView = null;
            depthStencilView?.Dispose();
            depthStencilView = null;
        }
    }
}
