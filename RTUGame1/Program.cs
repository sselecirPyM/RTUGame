using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using RTUGame1.Interoperation;
using static RTUGame1.Interoperation.Kernel32;
using static RTUGame1.Interoperation.User32;

namespace RTUGame1
{
    class Program
    {
        const uint PM_REMOVE = 1;

        [STAThread]
        static void Main()
        {
            new Program().Run();
        }
        static Dictionary<IntPtr, AppWindow> windows = new Dictionary<IntPtr, AppWindow>();

        bool quitRequested;


        void Run()
        {
            var moduleHandle = GetModuleHandle(null);

            var wndClass = new WNDCLASSEX
            {
                Size = Unsafe.SizeOf<WNDCLASSEX>(),
                Styles = WindowClassStyles.CS_HREDRAW | WindowClassStyles.CS_VREDRAW | WindowClassStyles.CS_OWNDC,
                WindowProc = WndProc,
                InstanceHandle = moduleHandle,
                CursorHandle = LoadCursor(IntPtr.Zero, SystemCursor.IDC_ARROW),
                BackgroundBrushHandle = IntPtr.Zero,
                IconHandle = IntPtr.Zero,
                ClassName = "WndClass",
            };

            RegisterClassEx(ref wndClass);

            var win32window = new Win32Window(wndClass.ClassName, "RTUGame", 800, 600);
            var mainWindow = new AppWindow(win32window);
            windows.Add(mainWindow.Win32Window.Handle, mainWindow);

            mainWindow.Show();

            while (!quitRequested)
            {
                while (PeekMessage(out var msg, IntPtr.Zero, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(ref msg);
                    DispatchMessage(ref msg);

                    if (msg.Value == (uint)WindowMessage.Quit)
                    {
                        quitRequested = true;
                        goto lable_stop;
                    }
                }

                foreach (var window in windows.Values)
                    window.UpdateAndDraw();
            }
        lable_stop:

            foreach (var window in windows.Values)
                window.Dispose();
            return;
        }

        static IntPtr WndProc(IntPtr hWnd, uint msg, UIntPtr wParam, IntPtr lParam)
        {
            windows.TryGetValue(hWnd, out var window);

            if (window?.ProcessMessage(msg, wParam, lParam) ?? false)
                return IntPtr.Zero;

            switch ((WindowMessage)msg)
            {
                case WindowMessage.Destroy:
                    PostQuitMessage(0);
                    break;
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
}
