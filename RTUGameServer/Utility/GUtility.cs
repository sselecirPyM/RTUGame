using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using RTUGameServer.GameLogic;
using RTUGameServer.Net;
using RTUGame.Game;
using RTUGame.Net;
using RTUGame.Mathematics;

namespace RTUGameServer.Utility
{
    public static class GUtility
    {
        public static void Send(this Chunk chunk, NetContext context)
        {
            NetPack netPack = context.GetNetBlock((int)NetMessageType.Chunk, Chunk.c_minimumSize);
            chunk.ToNetBlock(netPack);
            context.Send(netPack);
        }
        public static void DelaySend(this Chunk chunk, User user)
        {
            if (user.netPacks.Count < 8192)
            {
                NetPack netPack = user.netContext.GetNetBlock((int)NetMessageType.Chunk, Chunk.c_minimumSize);
                chunk.ToNetBlock(netPack);
                user.netPacks.Enqueue(netPack);
            }
        }
    }
}
