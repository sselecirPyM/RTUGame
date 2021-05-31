using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Direct3D12;
using Vortice.DXGI;

namespace RTUGame1.Graphics
{
    public class PipelineStateObject : IDisposable
    {
        public List<PSOCombind> PSOCombinds = new List<PSOCombind>();
        public ShaderBytecode vertexShader;
        public ShaderBytecode geometryShader;
        public ShaderBytecode pixelShader;
        public string Name;

        public PipelineStateObject(byte[] vertexShader, byte[] pixelShader)
        {
            this.vertexShader = vertexShader;
            this.pixelShader = pixelShader;
        }

        public PipelineStateObject(Shader vertexShader, Shader pixelShader)
        {
            this.vertexShader = vertexShader.compiledCode;
            this.pixelShader = pixelShader.compiledCode;
        }

        public ID3D12PipelineState GetState(GraphicsDevice device, PSODesc desc, RootSignature rootSignature)
        {
            foreach (var psoCombind in PSOCombinds)
            {
                if (psoCombind.PSODesc == desc && psoCombind.rootSignature == rootSignature)
                {
                    if (psoCombind.pipelineState == null)
                        throw new Exception("pipeline state error");
                    return psoCombind.pipelineState;
                }
            }
            InputLayoutDescription inputLayoutDescription;

            if (device.inputLayouts.TryGetValue(desc.InputLayout, out inputLayoutDescription))
            {

            }
            else
                inputLayoutDescription = new InputLayoutDescription(new InputElementDescription("POSITION", 0, Format.R32G32B32_Float, 0));

            GraphicsPipelineStateDescription graphicsPipelineStateDescription = new GraphicsPipelineStateDescription();
            graphicsPipelineStateDescription.RootSignature = rootSignature.rootSignature;
            graphicsPipelineStateDescription.VertexShader = vertexShader;
            graphicsPipelineStateDescription.GeometryShader = geometryShader;
            graphicsPipelineStateDescription.PixelShader = pixelShader;
            graphicsPipelineStateDescription.PrimitiveTopologyType = PrimitiveTopologyType.Triangle;
            graphicsPipelineStateDescription.InputLayout = inputLayoutDescription;
            graphicsPipelineStateDescription.DepthStencilFormat = desc.DepthStencilFormat;
            graphicsPipelineStateDescription.RenderTargetFormats = new Format[desc.RenderTargetCount];
            Array.Fill(graphicsPipelineStateDescription.RenderTargetFormats, desc.RenderTargetFormat);

            if (desc.BlendState == "Alpha")
                graphicsPipelineStateDescription.BlendState = blendStateAlpha();
            else if (desc.BlendState == "Add")
                graphicsPipelineStateDescription.BlendState = BlendDescription.Additive;
            else
                graphicsPipelineStateDescription.BlendState = BlendDescription.Opaque;


            graphicsPipelineStateDescription.DepthStencilState = new DepthStencilDescription(desc.DepthStencilFormat != Format.Unknown, desc.DepthStencilFormat != Format.Unknown);
            graphicsPipelineStateDescription.SampleMask = uint.MaxValue;
            var RasterizerState = new RasterizerDescription(CullMode.None, FillMode.Solid);
            RasterizerState.DepthBias = desc.DepthBias;
            RasterizerState.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
            graphicsPipelineStateDescription.RasterizerState = RasterizerState;

            var pipelineState = device.device.CreateGraphicsPipelineState<ID3D12PipelineState>(graphicsPipelineStateDescription);
            if (pipelineState == null)
                throw new Exception("pipeline state error");
            PSOCombinds.Add(new PSOCombind { PSODesc = desc, pipelineState = pipelineState, rootSignature = rootSignature });
            return pipelineState;
        }

        BlendDescription blendStateAlpha()
        {
            BlendDescription blendDescription = new BlendDescription(Blend.SourceAlpha, Blend.InverseSourceAlpha, Blend.One, Blend.InverseSourceAlpha);
            return blendDescription;
        }

        public void Dispose()
        {
            foreach (var combine in PSOCombinds)
            {
                combine.pipelineState.Dispose();
            }
            PSOCombinds.Clear();
        }
    }
    public class PSOCombind
    {
        public PSODesc PSODesc;
        public RootSignature rootSignature;
        public ID3D12PipelineState pipelineState;
    }
    public struct PSODesc : IEquatable<PSODesc>
    {
        public int RenderTargetCount;
        public Format RenderTargetFormat;
        public Format DepthStencilFormat;
        public string BlendState;
        public int DepthBias;
        public float SlopeScaledDepthBias;
        public CullMode CullMode;
        public string InputLayout;
        public PrimitiveTopologyType PrimitiveTopologyType;

        public override bool Equals(object obj)
        {
            return obj is PSODesc desc && Equals(desc);
        }

        public bool Equals(PSODesc other)
        {
            return RenderTargetCount == other.RenderTargetCount &&
                   RenderTargetFormat == other.RenderTargetFormat &&
                   DepthStencilFormat == other.DepthStencilFormat &&
                   BlendState == other.BlendState &&
                   DepthBias == other.DepthBias &&
                   SlopeScaledDepthBias == other.SlopeScaledDepthBias &&
                   CullMode == other.CullMode &&
                   InputLayout == other.InputLayout &&
                   PrimitiveTopologyType == other.PrimitiveTopologyType;
        }

        public override int GetHashCode()
        {
            HashCode hash = new HashCode();
            hash.Add(RenderTargetCount);
            hash.Add(RenderTargetFormat);
            hash.Add(DepthStencilFormat);
            hash.Add(BlendState);
            hash.Add(DepthBias);
            hash.Add(SlopeScaledDepthBias);
            hash.Add(CullMode);
            hash.Add(InputLayout);
            hash.Add(PrimitiveTopologyType);
            return hash.ToHashCode();
        }

        public static bool operator ==(PSODesc x, PSODesc y)
        {
            return x.Equals(y);
        }

        public static bool operator !=(PSODesc x, PSODesc y)
        {
            return !(x == y);
        }
        //public InputLayoutDescription InputLayout;
    }
}
