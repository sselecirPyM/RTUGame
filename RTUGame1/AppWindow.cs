using ImGuiNET;
using RTUGame1.Graphics;
using RTUGame1.Interoperation;
using RTUGame1.RenderPipeline;
using RTUGame1.ResourcesManage;
using RTUGame1.UI;
using System;
using System.Collections.Generic;
using System.Text;
using Vortice.Mathematics;

namespace RTUGame1
{
    public class AppWindow : IDisposable
    {
        CommonContext context = new CommonContext();
        CommonRenderPipeline commonRenderPipeline = new CommonRenderPipeline();
        ImGuiRender GUIRender = new ImGuiRender();

        DateTime currentTime;
        public Win32Window Win32Window;
        public AppWindow(Win32Window Win32Window)
        {
            this.Win32Window = Win32Window;
        }
        public void Initialize()
        {
            commonRenderPipeline.context = context;
            GUIRender.context = context;
            context.LoadDefaultResource();
            commonRenderPipeline.Initialize();



            GPUUpload uploadTest = new GPUUpload();
            uploadTest.mesh = context.GetMesh("quad");
            uploadTest.Quad();
            context.uploadQueue.Enqueue(uploadTest);

            GUIRender.Init();
            context.imguiInputHandler = new ImGuiInputHandler();
            context.imguiInputHandler.hwnd = Win32Window.Handle;

            context.graphicsContext.Initialize(context.device);
            context.device.SetupSwapChain((IntPtr)Win32Window.Handle);
        }
        public void Show()
        {
            User32.ShowWindow(Win32Window.Handle, ShowWindowCommand.Normal);
            Initialize();
        }
        public void UpdateAndDraw()
        {
            if (Win32Window.Width != context.device.width || Win32Window.Height != context.device.height)
            {
                context.device.Resize(Win32Window.Width, Win32Window.Height);
                ImGui.GetIO().DisplaySize = new System.Numerics.Vector2(context.device.width, context.device.height);
            }

            var graphicsContext = context.graphicsContext;
            context.device.Begin();
            graphicsContext.BeginCommand();
            context.GPUUploadDatas();
            graphicsContext.SetDescriptorHeapDefault();
            graphicsContext.ScreenBeginRender();
            graphicsContext.SetRenderTargetScreen();
            graphicsContext.ClearRenderTargetScreen(new Color4(0.5f, 0.5f, 1, 1));

            //just test
            //int index1 = context.uploadBuffer.Upload<System.Numerics.Vector4>(new[] { new System.Numerics.Vector4(1, 0, 1, 1) });
            //PSODesc testDesc = new PSODesc();
            //testDesc.CullMode = Vortice.Direct3D12.CullMode.None;
            //testDesc.RenderTargetFormat = Vortice.DXGI.Format.R8G8B8A8_UNorm;
            //testDesc.RenderTargetCount = 1;
            //testDesc.PrimitiveTopologyType = Vortice.Direct3D12.PrimitiveTopologyType.Triangle;
            //testDesc.InputLayout = "";
            //graphicsContext.SetRootSignature(Pipeline12Util.FromString(context, "Cssss"));
            //graphicsContext.SetPipelineState(context.pipelineStateObjects["Test"], testDesc);
            //graphicsContext.SetMesh(context.meshes["quad"]);
            //graphicsContext.SetSRV(context.renderTargets["imgui_font"], 0);
            //context.uploadBuffer.SetCBV(graphicsContext, index1, 0);

            //graphicsContext.DrawIndexedInstanced(6, 1, 0, 0, 0);

            commonRenderPipeline.Prepare();
            commonRenderPipeline.Render();
            ImGui.SetCurrentContext(context.imguiContext);
            var previous = currentTime;
            currentTime = DateTime.Now;
            float delta = (float)(currentTime - previous).TotalSeconds;
            ImGui.GetIO().DeltaTime = delta;
            context.imguiInputHandler.Update();
            GUIRender.Render();
            graphicsContext.ScreenEndRender();
            graphicsContext.EndCommand();
            graphicsContext.Execute();
            context.device.Present(true);
        }

        public virtual bool ProcessMessage(uint msg, UIntPtr wParam, IntPtr lParam)
        {
            if (context.imguiInputHandler != null && context.imguiInputHandler.ProcessMessage((WindowMessage)msg, wParam, lParam))
                return true;

            switch ((WindowMessage)msg)
            {
                case WindowMessage.Size:
                    switch ((SizeMessage)wParam)
                    {
                        case SizeMessage.SIZE_RESTORED:
                        case SizeMessage.SIZE_MAXIMIZED:
                            Win32Window.IsMinimized = false;

                            var lp = (int)lParam;
                            Win32Window.Width = Utils.Loword(lp);
                            Win32Window.Height = Utils.Hiword(lp);
                            break;
                        case SizeMessage.SIZE_MINIMIZED:
                            Win32Window.IsMinimized = true;
                            break;
                        default:
                            break;
                    }
                    break;
            }

            return false;
        }

        public void Dispose()
        {
            context.Dispose();
        }
    }
}
