using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Direct3D12;
using Vortice.DXGI;
using RTUGame1.Graphics;
using RTUGame1.ResourcesManage;

namespace RTUGame1.RenderPipeline
{
    static class Pipeline12Util
    {
        public static RootSignature FromString(CommonContext context, string s)
        {
            RootSignature rootSignature;
            if(context.rootSignatures.TryGetValue(s, out rootSignature))
            {
                return rootSignature;
            }


            rootSignature = new RootSignature();
            context.rootSignatures[s] = rootSignature;
            RootSignatureParamP[] desc = new RootSignatureParamP[s.Length];

            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];
                switch (c)
                {
                    case 'C':
                        desc[i] = RootSignatureParamP.CBV;
                        break;
                    case 'c':
                        desc[i] = RootSignatureParamP.CBVTable;
                        break;
                    case 'S':
                        desc[i] = RootSignatureParamP.SRV;
                        break;
                    case 's':
                        desc[i] = RootSignatureParamP.SRVTable;
                        break;
                    case 'U':
                        desc[i] = RootSignatureParamP.UAV;
                        break;
                    case 'u':
                        desc[i] = RootSignatureParamP.UAVTable;
                        break;
                    default:
                        throw new NotImplementedException("error root signature desc.");
                        break;
                }
            }
            context.device.CreateRootSignature(rootSignature, desc);
            return rootSignature;
        }
    }
}
