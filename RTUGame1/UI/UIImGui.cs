using RTUGame1.ResourcesManage;
using System;
using System.Collections.Generic;
using System.Text;
using ImGuiNET;

namespace RTUGame1.UI
{
    public static class UIImGui
    {
        public static CommonContext context;
        public static void Render()
        {
            ImGui.Begin("连接到服务器");
            ImGui.End();
        }
    }
}
