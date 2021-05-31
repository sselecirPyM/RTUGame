using RTUGameServer.GameLogic;
using RTUGameServer.Net;
using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Net.Sockets;
using System.Text;
using RTUGame.Net;

namespace RTUGameServer
{
    public class User
    {
        public Socket socket;
        public NetContext netContext = new NetContext();
        public int authority;
        public PlayerInfo currentPlayer;
        public ConcurrentQueue<NetPack> netPacks = new ConcurrentQueue<NetPack>();

        public void SendPacks()
        {
            while(netPacks.TryDequeue(out var pack))
            {
                netContext.Send(pack);
            }
        }

        public void Log(object message)
        {
            Console.WriteLine(string.Format("{0} {1}", DateTime.Now, message));
        }
    }
}
