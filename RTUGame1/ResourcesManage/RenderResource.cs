using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace RTUGame1.ResourcesManage
{
    public class RenderResource
    {
        [XmlArrayItem("VertexShader")]
        public List<_AssetDefine> VertexShaders;
        [XmlArrayItem("GeometryShader")]
        public List<_AssetDefine> GeometryShaders;
        [XmlArrayItem("PixelShader")]
        public List<_AssetDefine> PixelShaders;
        [XmlArrayItem("PipelineState")]
        public List<_AssetPipelineState> PipelineStates;

        public struct _AssetDefine
        {
            public string Name;
            public string Path;
        }

        public struct _AssetPipelineState
        {
            public string Name;
            public string VertexShader;
            public string GeometryShader;
            public string PixelShader;
        }
    }
}
