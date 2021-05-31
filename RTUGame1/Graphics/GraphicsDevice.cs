using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using Vortice.Direct3D12;
using Vortice.Direct3D12.Debug;
using Vortice.Dxc;
using Vortice.DXGI;
using Vortice.Mathematics;

namespace RTUGame1.Graphics
{
    public struct ResourceDelayDestroy
    {
        public ulong destroyFrame;
        public ID3D12Resource resource;
        public ID3D12PipelineState pipelineState;
    }
    public class GraphicsDevice : IDisposable
    {
        public void Initialize()
        {
#if DEBUG
            if (D3D12.D3D12GetDebugInterface<ID3D12Debug>(out var pDx12Debug).Success)
            {
                pDx12Debug.EnableDebugLayer();
            }
#endif
            ThrowIfFailed(DXGI.CreateDXGIFactory1(out dxgiFactory));
            int index1 = 0;
            while (true)
            {
                var hr = dxgiFactory.EnumAdapterByGpuPreference(index1, GpuPreference.HighPerformance, out adapter);
                if (hr == SharpGen.Runtime.Result.Ok)
                {
                    break;
                }
                index1++;
            }
            ThrowIfFailed(D3D12.D3D12CreateDevice(this.adapter, out device));
            CommandQueueDescription description;
            description.Flags = CommandQueueFlags.None;
            description.Type = CommandListType.Direct;
            description.NodeMask = 0;
            description.Priority = 0;
            ThrowIfFailed(device.CreateCommandQueue(description, out commandQueue));
            DescriptorHeapDescription descriptorHeapDescription;
            descriptorHeapDescription.DescriptorCount = CBVSRVUAVDescriptorCount;
            descriptorHeapDescription.Type = DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView;
            descriptorHeapDescription.Flags = DescriptorHeapFlags.ShaderVisible;
            descriptorHeapDescription.NodeMask = 0;
            ThrowIfFailed(device.CreateDescriptorHeap(descriptorHeapDescription, out cbvsrvuavHeap));

            descriptorHeapDescription.DescriptorCount = 16;
            descriptorHeapDescription.Type = DescriptorHeapType.DepthStencilView;
            descriptorHeapDescription.Flags = DescriptorHeapFlags.None;
            ThrowIfFailed(device.CreateDescriptorHeap(descriptorHeapDescription, out dsvHeap));

            descriptorHeapDescription.DescriptorCount = 16;
            descriptorHeapDescription.Type = DescriptorHeapType.RenderTargetView;
            descriptorHeapDescription.Flags = DescriptorHeapFlags.None;
            ThrowIfFailed(device.CreateDescriptorHeap(descriptorHeapDescription, out rtvHeap));
            waitHandle = new EventWaitHandle(false, EventResetMode.AutoReset);

            cbvsrvuavHeapIncrementSize = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.ConstantBufferViewShaderResourceViewUnorderedAccessView);
            dsvHeapIncrementSize = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.DepthStencilView);
            rtvHeapIncrementSize = device.GetDescriptorHandleIncrementSize(DescriptorHeapType.RenderTargetView);
            commandAllocators = new List<ID3D12CommandAllocator>();
            for (int i = 0; i < bufferCount; i++)
            {
                ThrowIfFailed(device.CreateCommandAllocator(CommandListType.Direct, out ID3D12CommandAllocator commandAllocator));
                commandAllocators.Add(commandAllocator);
            }
            ThrowIfFailed(device.CreateFence(executeCount, FenceFlags.None, out fence));
            executeCount++;
        }

        public void SetupSwapChain(IntPtr hwnd)
        {
            this.hwnd = hwnd;
        }

        public void Resize(int width, int height)
        {
            WaitForGpu();
            this.width = Math.Max(width, 1);
            this.height = Math.Max(height, 1);
            if (swapChain == null)
            {
                SwapChainDescription1 swapChainDescription;
                swapChainDescription.Width = width;
                swapChainDescription.Height = height;
                swapChainDescription.Format = swapChainFormat;
                swapChainDescription.Stereo = false;
                swapChainDescription.SampleDescription.Count = 1;
                swapChainDescription.SampleDescription.Quality = 0;
                swapChainDescription.Usage = Usage.RenderTargetOutput;
                swapChainDescription.BufferCount = bufferCount;
                swapChainDescription.SwapEffect = SwapEffect.FlipDiscard;
                swapChainDescription.Flags = SwapChainFlags.AllowTearing;
                swapChainDescription.Scaling = Scaling.Stretch;
                swapChainDescription.AlphaMode = AlphaMode.Ignore;
                IDXGISwapChain1 swapChain1 = dxgiFactory.CreateSwapChainForHwnd(commandQueue, hwnd, swapChainDescription);
                swapChain = swapChain1.QueryInterface<IDXGISwapChain3>();
                swapChain1.Dispose();
            }
            else
            {
                foreach (var screenResource in screenResources)
                    screenResource.Dispose();
                ThrowIfFailed(swapChain.ResizeBuffers(bufferCount, width, height, swapChainFormat, SwapChainFlags.AllowTearing));
            }
            screenResources = new List<ID3D12Resource>();
            CpuDescriptorHandle handle = rtvHeap.GetCPUDescriptorHandleForHeapStart();

            for (int i = 0; i < bufferCount; i++)
            {
                ThrowIfFailed(swapChain.GetBuffer(i, out ID3D12Resource res));
                screenResources.Add(res);
                device.CreateRenderTargetView(res, null, handle);
                handle.Ptr += rtvHeapIncrementSize;
            }
        }

        public ID3D12CommandAllocator GetCommandAllocator()
        {
            return commandAllocators[executeIndex];
        }

        public CpuDescriptorHandle GetRenderTargetScreen()
        {
            CpuDescriptorHandle handle = rtvHeap.GetCPUDescriptorHandleForHeapStart();
            handle.Ptr += swapChain.GetCurrentBackBufferIndex() * rtvHeapIncrementSize;
            return handle;
        }

        public ID3D12Resource GetScreenResource()
        {
            return screenResources[swapChain.GetCurrentBackBufferIndex()];
        }

        public void Begin()
        {
            GetCommandAllocator().Reset();
        }

        public void Present(bool vsync)
        {
            if (vsync)
                ThrowIfFailed(swapChain.Present(1, PresentFlags.None));
            else
                ThrowIfFailed(swapChain.Present(0, PresentFlags.AllowTearing));
            commandQueue.Signal(fence, executeCount);
            executeIndex = (executeIndex + 1) % bufferCount;
            if (fence.CompletedValue < executeCount - (uint)bufferCount + 1)
            {
                fence.SetEventOnCompletion(executeCount - (uint)bufferCount + 1, waitHandle);
                waitHandle.WaitOne();
            }
            DestroyResourceInternal(fence.CompletedValue);
            executeCount++;
        }

        public void WaitForGpu()
        {
            commandQueue.Signal(fence, executeCount);
            fence.SetEventOnCompletion(executeCount, waitHandle);
            waitHandle.WaitOne();
            DestroyResourceInternal(fence.CompletedValue);
            executeCount++;
        }

        public void CreateRootSignature(RootSignature rootSignature, IList<RootSignatureParamP> types)
        {
            //static samplers
            StaticSamplerDescription[] samplerDescription = new StaticSamplerDescription[4];
            samplerDescription[0] = new StaticSamplerDescription(ShaderVisibility.All, 0, 0)
            {
                AddressU = TextureAddressMode.Clamp,
                AddressV = TextureAddressMode.Clamp,
                AddressW = TextureAddressMode.Clamp,
                BorderColor = StaticBorderColor.OpaqueBlack,
                ComparisonFunction = ComparisonFunction.Never,
                Filter = Filter.MinMagMipLinear,
                MipLODBias = 0,
                MaxAnisotropy = 0,
                MinLOD = 0,
                MaxLOD = float.MaxValue,
                ShaderVisibility = ShaderVisibility.All,
                RegisterSpace = 0,
                ShaderRegister = 0,
            };
            samplerDescription[1] = samplerDescription[0];
            samplerDescription[2] = samplerDescription[0];
            samplerDescription[3] = samplerDescription[0];

            samplerDescription[1].ShaderRegister = 1;
            samplerDescription[2].ShaderRegister = 2;
            samplerDescription[3].ShaderRegister = 3;
            samplerDescription[1].MaxAnisotropy = 16;
            samplerDescription[1].Filter = Filter.Anisotropic;
            samplerDescription[2].ComparisonFunction = ComparisonFunction.Less;
            samplerDescription[2].Filter = Filter.ComparisonMinMagMipLinear;
            samplerDescription[3].Filter = Filter.MinMagMipPoint;

            RootParameter1[] rootParameters = new RootParameter1[types.Count];

            int cbvCount = 0;
            int srvCount = 0;
            int uavCount = 0;
            rootSignature.cbv.Clear();
            rootSignature.srv.Clear();
            rootSignature.uav.Clear();

            for (int i = 0; i < types.Count; i++)
            {
                RootSignatureParamP t = types[i];
                switch (t)
                {
                    case RootSignatureParamP.CBV:
                        rootParameters[i] = new RootParameter1(RootParameterType.ConstantBufferView, new RootDescriptor1(cbvCount, 0), ShaderVisibility.All);
                        rootSignature.cbv[cbvCount] = i;
                        cbvCount++;
                        break;
                    case RootSignatureParamP.SRV:
                        rootParameters[i] = new RootParameter1(RootParameterType.ShaderResourceView, new RootDescriptor1(srvCount, 0), ShaderVisibility.All);
                        rootSignature.srv[srvCount] = i;
                        srvCount++;
                        break;
                    case RootSignatureParamP.UAV:
                        rootParameters[i] = new RootParameter1(RootParameterType.UnorderedAccessView, new RootDescriptor1(uavCount, 0), ShaderVisibility.All);
                        rootSignature.uav[uavCount] = i;
                        uavCount++;
                        break;
                    case RootSignatureParamP.CBVTable:
                        rootParameters[i] = new RootParameter1(new RootDescriptorTable1(new DescriptorRange1(DescriptorRangeType.ConstantBufferView, 1, cbvCount)), ShaderVisibility.All);
                        rootSignature.cbv[cbvCount] = i;
                        cbvCount++;
                        break;
                    case RootSignatureParamP.SRVTable:
                        rootParameters[i] = new RootParameter1(new RootDescriptorTable1(new DescriptorRange1(DescriptorRangeType.ShaderResourceView, 1, srvCount)), ShaderVisibility.All);
                        rootSignature.srv[srvCount] = i;
                        srvCount++;
                        break;
                    case RootSignatureParamP.UAVTable:
                        rootParameters[i] = new RootParameter1(new RootDescriptorTable1(new DescriptorRange1(DescriptorRangeType.UnorderedAccessView, 1, uavCount)), ShaderVisibility.All);
                        rootSignature.uav[uavCount] = i;
                        uavCount++;
                        break;
                }
            }

            RootSignatureDescription1 rootSignatureDescription = new RootSignatureDescription1();
            rootSignatureDescription.StaticSamplers = samplerDescription;
            rootSignatureDescription.Flags = RootSignatureFlags.AllowInputAssemblerInputLayout;
            rootSignatureDescription.Parameters = rootParameters;

            rootSignature.rootSignature = device.CreateRootSignature<ID3D12RootSignature>(0, rootSignatureDescription);
        }

        public void RenderTexture(Texture2D texture)
        {
            ResourceDescription resourceDescription = new ResourceDescription
            {
                Width = (ulong)texture.width,
                Height = texture.height,
                MipLevels = 1,
                SampleDescription = new SampleDescription(1, 0),
                Dimension = ResourceDimension.Texture2D,
                DepthOrArraySize = 1,
                Format = texture.format,
            };
            if (texture.dsvFormat != 0)
            {
                DestroyResource(texture.resource);
                resourceDescription.Flags = ResourceFlags.AllowDepthStencil;
                ThrowIfFailed(device.CreateCommittedResource<ID3D12Resource>(HeapProperties.DefaultHeapProperties,
                     HeapFlags.None,
                     resourceDescription,
                     ResourceStates.GenericRead,
                     new ClearValue(texture.dsvFormat, new DepthStencilValue(1.0f, 0)), out texture.resource));
                if (texture.depthStencilView == null)
                {
                    DescriptorHeapDescription descriptorHeapDescription;
                    descriptorHeapDescription.DescriptorCount = 1;
                    descriptorHeapDescription.Type = DescriptorHeapType.DepthStencilView;
                    descriptorHeapDescription.Flags = DescriptorHeapFlags.None;
                    descriptorHeapDescription.NodeMask = 0;
                    ThrowIfFailed(device.CreateDescriptorHeap(descriptorHeapDescription, out texture.depthStencilView));
                }

                device.CreateDepthStencilView(texture.resource, null, texture.depthStencilView.GetCPUDescriptorHandleForHeapStart());
            }
            else if (texture.rtvFormat != 0)
            {
                DestroyResource(texture.resource);
                resourceDescription.Flags = ResourceFlags.AllowRenderTarget | ResourceFlags.AllowUnorderedAccess;
                ThrowIfFailed(device.CreateCommittedResource<ID3D12Resource>(HeapProperties.DefaultHeapProperties,
                     HeapFlags.None,
                     resourceDescription,
                     ResourceStates.GenericRead,
                     new ClearValue(texture.dsvFormat, new Color4(0, 0, 0, 0)), out texture.resource));
                if (texture.renderTargetView == null)
                {
                    DescriptorHeapDescription descriptorHeapDescription;
                    descriptorHeapDescription.DescriptorCount = 1;
                    descriptorHeapDescription.Type = DescriptorHeapType.RenderTargetView;
                    descriptorHeapDescription.Flags = DescriptorHeapFlags.None;
                    descriptorHeapDescription.NodeMask = 0;
                    ThrowIfFailed(device.CreateDescriptorHeap(descriptorHeapDescription, out texture.renderTargetView));
                }

                device.CreateRenderTargetView(texture.resource, null, texture.renderTargetView.GetCPUDescriptorHandleForHeapStart());
            }
            else
            {
                throw new NotImplementedException();
            }
            texture.resourceStates = ResourceStates.GenericRead;
        }

        public void CreateUploadBuffer(UploadBuffer uploadBuffer, int size)
        {
            DestroyResource(uploadBuffer.resource);
            uploadBuffer.resource = device.CreateCommittedResource<ID3D12Resource>(
                HeapProperties.UploadHeapProperties,
                HeapFlags.None,
                ResourceDescription.Buffer(new ResourceAllocationInfo((ulong)size, 0)),
                ResourceStates.GenericRead);
            uploadBuffer.size = size;
        }

        public void DestroyResource(ID3D12PipelineState res)
        {
            if (res != null)
                delayDestroy.Enqueue(new ResourceDelayDestroy { pipelineState = res, destroyFrame = executeCount });
        }

        public void DestroyResource(ID3D12Resource res)
        {
            if (res != null)
                delayDestroy.Enqueue(new ResourceDelayDestroy { resource = res, destroyFrame = executeCount });
        }

        private void DestroyResourceInternal(ulong completedFrame)
        {
            while (delayDestroy.Count > 0)
                if (delayDestroy.Peek().destroyFrame <= completedFrame)
                {
                    var p = delayDestroy.Dequeue();
                    p.pipelineState?.Dispose();
                    p.resource?.Dispose();
                }
                else
                    break;
        }

        public ID3D12Device2 device;
        public IDXGIAdapter adapter;
        public IDXGIFactory7 dxgiFactory;
        public ID3D12CommandQueue commandQueue;
        public ID3D12DescriptorHeap cbvsrvuavHeap;
        public int cbvsrvuavAllocatedCount;
        public ID3D12DescriptorHeap dsvHeap;
        public ID3D12DescriptorHeap rtvHeap;
        public IDXGISwapChain3 swapChain;
        public List<ID3D12CommandAllocator> commandAllocators;
        public EventWaitHandle waitHandle;
        public ID3D12Fence fence;

        public Queue<ResourceDelayDestroy> delayDestroy = new Queue<ResourceDelayDestroy>();

        public int cbvsrvuavHeapIncrementSize;
        public int dsvHeapIncrementSize;
        public int rtvHeapIncrementSize;

        public int executeIndex = 0;
        public ulong executeCount = 3;//greater equal than 'bufferCount'

        public int width;
        public int height;
        public IntPtr hwnd;

        public Format swapChainFormat = Format.R8G8B8A8_UNorm;
        public List<ID3D12Resource> screenResources;
        public int bufferCount = 3;
        public int CBVSRVUAVDescriptorCount = 65536;

        public Dictionary<string, InputLayoutDescription> inputLayouts = new Dictionary<string, InputLayoutDescription>();

        void ThrowIfFailed(SharpGen.Runtime.Result hr)
        {
            if (hr != SharpGen.Runtime.Result.Ok)
            {
                throw new NotImplementedException(hr.ToString());
            }
        }

        public void Dispose()
        {
            WaitForGpu();
            while (delayDestroy.Count > 0)
            {
                var p = delayDestroy.Dequeue();
                p.pipelineState?.Dispose();
                p.resource?.Dispose();
            }
            foreach (var commandAllocator in commandAllocators)
                commandAllocator.Dispose();
            if (screenResources != null)
                foreach (var screenResource in screenResources)
                    screenResource.Dispose();
            dxgiFactory?.Dispose();
            commandQueue?.Dispose();
            cbvsrvuavHeap?.Dispose();
            dsvHeap?.Dispose();
            rtvHeap?.Dispose();
            swapChain?.Dispose();
            fence?.Dispose();
            device?.Dispose();
            adapter?.Dispose();
        }
    }
}
