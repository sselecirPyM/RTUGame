using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using System.Xml.Serialization;
using RTUGame1.Graphics;

namespace RTUGame1.ResourcesManage
{
    public class PassSetting
    {
        [XmlArrayItem("RenderTarget")]
        public List<RenderTarget> RenderTargets;
        [XmlArrayItem("Pass")]
        public List<PassMatch1> RenderSequence;
        [XmlArrayItem("Pass")]
        public List<Pass> Passes;
        [XmlArrayItem("PipelineState")]
        public List<PSPS> PipelineStates;
        [XmlArrayItem("VertexShader")]
        public List<_AssetDefine> VertexShaders;
        [XmlArrayItem("GeometryShader")]
        public List<_AssetDefine> GeometryShaders;
        [XmlArrayItem("PixelShader")]
        public List<_AssetDefine> PixelShaders;
        [XmlArrayItem("Texture2D")]
        public List<_AssetDefine> Texture2Ds;

        //[XmlArrayItem("RayTracingStateObject")]
        public PSRTSO RayTracingStateObject;

        [XmlIgnore]
        public bool configured;

        public bool Verify()
        {
            if (RenderTargets == null || RenderTargets.Count == 0)
                return false;
            if (RenderSequence == null || RenderSequence.Count == 0)
                return false;
            if (Passes == null || Passes.Count == 0)
                return false;
            if (PipelineStates == null)
                PipelineStates = new List<PSPS>();
            foreach (var passMatch in RenderSequence)
            {
                if (passMatch.Name != null)
                {
                    if (passMatch.Type == null)
                        passMatch.DrawObjects = true;
                    foreach (var pass in Passes)
                    {
                        if (passMatch.Name == pass.Name)
                            passMatch.Pass = pass;
                    }
                    if (passMatch.Pass == null)
                        return false;
                }
                //else if (passMatch.Foreach != null)
                //{
                //}
                else
                    return false;
            }

            return true;
        }
    }
    public class PassMatch1
    {
        public string Name;
        public int DepthBias;
        public float SlopeScaledDepthBias;
        public string Type;
        [XmlElement("RenderTarget")]
        public List<string> RenderTargets;

        [XmlElement("Parameter")]
        public List<PassParameter> passParameters;
        public string DepthStencil;
        public EBlendState BlendMode;
        public bool ClearDepth;
        public bool ClearRenderTarget;
        public CullMode CullMode;
        public string Filter;

        //public string Foreach;
        //[XmlElement("Pass")]
        //public List<PassMatch1> passes;

        [XmlIgnore]
        public string[] RayGenShaders;
        [XmlIgnore]
        public string[] MissShaders;
        [XmlIgnore]
        public Texture2D[] renderTargets;
        [XmlIgnore]
        public Texture2D depthSencil;
        [XmlIgnore]
        public PipelineStateObject PSODefault;
        [XmlIgnore]
        public bool DrawObjects;
        [XmlIgnore]
        public Dictionary<string, float> passParameters1;
        [XmlIgnore]
        public Pass Pass;
        [XmlIgnore]
        public string rootSignatureKey;
    }
    public enum CullMode
    {
        Unspecified = 0,
        None = 1,
        Front = 2,
        Back = 3,
    }
    public enum EBlendState
    {
        Unspecified = 0,
        None = 1,
        Front = 2,
        Back = 3,
    }
    public class Pass
    {
        public string Name;
        public string Camera;
        public string VertexShader;
        public string GeometryShader;
        public string PixelShader;
        [XmlElement(ElementName = "SRV")]
        public List<SRVSlotRes> SRVs;
        [XmlElement(ElementName = "CBV")]
        public List<CBVSlotRes> CBVs;
    }
    public class PSPS
    {
        public string Name;
        public string VertexShader;
        public string GeometryShader;
        public string PixelShader;
    }

    public struct SRVSlotRes
    {
        public int Index;
        public string ResourceType;
        public string Resource;
    }
    public struct CBVSlotRes
    {
        public int Index;
        [XmlArrayItem(ElementName = "Data")]
        public List<string> Datas;
    }
    public struct RenderTarget
    {
        public string Name;
        public VarSize Size;
        public DxgiFormat Format;
    }
    public class PTopAccelerateStructure
    {
        public string Name;
        public string Filter;
        [XmlElement(ElementName = "SRV")]
        public List<SRVSlotRes> SRVs;
        [XmlElement(ElementName = "CBV")]
        public List<CBVSlotRes> CBVs;
    }
    public class VarSize
    {
        public string Source;

        [DefaultValue(1.0f)]
        public float Multiplier = 1.0f;
        [DefaultValue(1)]
        public int x = 1;
        [DefaultValue(1)]
        public int y = 1;
        [DefaultValue(1)]
        public int z = 1;
    }
    public class PSRTSO
    {
        public string Name;
        public string Path;
        public int MaxPayloadSize;
        public int MaxAttributeSize;
        public int MaxRecursionDepth;
        [XmlElement(ElementName = "RayGenShader")]
        public RTShader[] rayGenShaders;
        [XmlElement(ElementName = "MissShader")]
        public List<RTShader> missShaders;
        [XmlElement("HitGroupSubobject")]
        public List<RTHitGroup> hitGroups;

    }
    public class RTShader
    {
        public string Name;
    }
    public class RTHitGroup
    {
        public string Name;
        public string AnyHitShader;
        public string ClosestHitShader;
    }
    public struct _AssetDefine
    {
        public string Name;
        public string Path;
    }
    public struct PassParameter
    {
        public string Name;
        public float Value;
    }
    public enum DxgiFormat
    {
        DXGI_FORMAT_FORCE_UINT = -1,
        DXGI_FORMAT_UNKNOWN = 0,
        DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
        DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
        DXGI_FORMAT_R32G32B32A32_UINT = 3,
        DXGI_FORMAT_R32G32B32A32_SINT = 4,
        DXGI_FORMAT_R32G32B32_TYPELESS = 5,
        DXGI_FORMAT_R32G32B32_FLOAT = 6,
        DXGI_FORMAT_R32G32B32_UINT = 7,
        DXGI_FORMAT_R32G32B32_SINT = 8,
        DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
        DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
        DXGI_FORMAT_R16G16B16A16_UNORM = 11,
        DXGI_FORMAT_R16G16B16A16_UINT = 12,
        DXGI_FORMAT_R16G16B16A16_SNORM = 13,
        DXGI_FORMAT_R16G16B16A16_SINT = 14,
        DXGI_FORMAT_R32G32_TYPELESS = 15,
        DXGI_FORMAT_R32G32_FLOAT = 16,
        DXGI_FORMAT_R32G32_UINT = 17,
        DXGI_FORMAT_R32G32_SINT = 18,
        DXGI_FORMAT_R32G8X24_TYPELESS = 19,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
        DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
        DXGI_FORMAT_R10G10B10A2_UNORM = 24,
        DXGI_FORMAT_R10G10B10A2_UINT = 25,
        DXGI_FORMAT_R11G11B10_FLOAT = 26,
        DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
        DXGI_FORMAT_R8G8B8A8_UNORM = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        DXGI_FORMAT_R8G8B8A8_UINT = 30,
        DXGI_FORMAT_R8G8B8A8_SNORM = 31,
        DXGI_FORMAT_R8G8B8A8_SINT = 32,
        DXGI_FORMAT_R16G16_TYPELESS = 33,
        DXGI_FORMAT_R16G16_FLOAT = 34,
        DXGI_FORMAT_R16G16_UNORM = 35,
        DXGI_FORMAT_R16G16_UINT = 36,
        DXGI_FORMAT_R16G16_SNORM = 37,
        DXGI_FORMAT_R16G16_SINT = 38,
        DXGI_FORMAT_R32_TYPELESS = 39,
        DXGI_FORMAT_D32_FLOAT = 40,
        DXGI_FORMAT_R32_FLOAT = 41,
        DXGI_FORMAT_R32_UINT = 42,
        DXGI_FORMAT_R32_SINT = 43,
        DXGI_FORMAT_R24G8_TYPELESS = 44,
        DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
        DXGI_FORMAT_R8G8_TYPELESS = 48,
        DXGI_FORMAT_R8G8_UNORM = 49,
        DXGI_FORMAT_R8G8_UINT = 50,
        DXGI_FORMAT_R8G8_SNORM = 51,
        DXGI_FORMAT_R8G8_SINT = 52,
        DXGI_FORMAT_R16_TYPELESS = 53,
        DXGI_FORMAT_R16_FLOAT = 54,
        DXGI_FORMAT_D16_UNORM = 55,
        DXGI_FORMAT_R16_UNORM = 56,
        DXGI_FORMAT_R16_UINT = 57,
        DXGI_FORMAT_R16_SNORM = 58,
        DXGI_FORMAT_R16_SINT = 59,
        DXGI_FORMAT_R8_TYPELESS = 60,
        DXGI_FORMAT_R8_UNORM = 61,
        DXGI_FORMAT_R8_UINT = 62,
        DXGI_FORMAT_R8_SNORM = 63,
        DXGI_FORMAT_R8_SINT = 64,
        DXGI_FORMAT_A8_UNORM = 65,
        DXGI_FORMAT_R1_UNORM = 66,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
        DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
        DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
        DXGI_FORMAT_BC1_TYPELESS = 70,
        DXGI_FORMAT_BC1_UNORM = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB = 72,
        DXGI_FORMAT_BC2_TYPELESS = 73,
        DXGI_FORMAT_BC2_UNORM = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB = 75,
        DXGI_FORMAT_BC3_TYPELESS = 76,
        DXGI_FORMAT_BC3_UNORM = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB = 78,
        DXGI_FORMAT_BC4_TYPELESS = 79,
        DXGI_FORMAT_BC4_UNORM = 80,
        DXGI_FORMAT_BC4_SNORM = 81,
        DXGI_FORMAT_BC5_TYPELESS = 82,
        DXGI_FORMAT_BC5_UNORM = 83,
        DXGI_FORMAT_BC5_SNORM = 84,
        DXGI_FORMAT_B5G6R5_UNORM = 85,
        DXGI_FORMAT_B5G5R5A1_UNORM = 86,
        DXGI_FORMAT_B8G8R8A8_UNORM = 87,
        DXGI_FORMAT_B8G8R8X8_UNORM = 88,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
        DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
        DXGI_FORMAT_BC6H_TYPELESS = 94,
        DXGI_FORMAT_BC6H_UF16 = 95,
        DXGI_FORMAT_BC6H_SF16 = 96,
        DXGI_FORMAT_BC7_TYPELESS = 97,
        DXGI_FORMAT_BC7_UNORM = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB = 99,
        DXGI_FORMAT_AYUV = 100,
        DXGI_FORMAT_Y410 = 101,
        DXGI_FORMAT_Y416 = 102,
        DXGI_FORMAT_NV12 = 103,
        DXGI_FORMAT_P010 = 104,
        DXGI_FORMAT_P016 = 105,
        DXGI_FORMAT_420_OPAQUE = 106,
        DXGI_FORMAT_YUY2 = 107,
        DXGI_FORMAT_Y210 = 108,
        DXGI_FORMAT_Y216 = 109,
        DXGI_FORMAT_NV11 = 110,
        DXGI_FORMAT_AI44 = 111,
        DXGI_FORMAT_IA44 = 112,
        DXGI_FORMAT_P8 = 113,
        DXGI_FORMAT_A8P8 = 114,
        DXGI_FORMAT_B4G4R4A4_UNORM = 115,
        DXGI_FORMAT_P208 = 130,
        DXGI_FORMAT_V208 = 131,
        DXGI_FORMAT_V408 = 132
    }
}
