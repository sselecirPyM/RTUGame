//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game Server.
//
//RTU Game Server is free software : you can redistribute it and/or modify
//it under the terms of the GNU Affero General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game Server is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Affero General Public License for more details.
//
//You should have received a copy of the GNU Affero General Public License
//along with RTU Game Server.If not, see < https://www.gnu.org/licenses/>.
using RTUGameServer.GameLogic;
using RTUGameServer.MathTypes;
using RTUGameServer.Net;
using RTUGameServer.Utility;
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using SharpDX;
using RTUGame.Mathematics;
using RTUGame.Game;
using RTUGame.Net;

namespace RTUGameServer
{
    public class AppCore
    {
        public ServerContext serverContext = new ServerContext();
        public Thread connectServiceThread;
        public Socket serverSocket;
        public ManualResetEvent connectServiceThreadBlock = new ManualResetEvent(true);
        public GameWorld world;

        public void Initialize(string[] args)
        {
            serverContext.License = System.IO.File.ReadAllText("COPING.txt");
            serverContext.port = 25565;

            Console.WriteLine("Core Initialize.");
        }

        public void LaunchBaseService()
        {
            world = new GameWorld();
            world.DefaultSettings();
            //world.Save(System.IO.File.OpenWrite("test.zip"));
            Task.Factory.StartNew(world.GameService, TaskCreationOptions.LongRunning);
            connectServiceThreadBlock.Set();
            connectServiceThread = new Thread(ConnectService);
            connectServiceThread.Start();
        }

        public void CommandService()
        {
            while (true)
            {
                string cmd = Console.ReadLine();
                if (cmd == "stopconnecting")
                {
                    serverContext.Log("console: stop connecting.");
                    connectServiceThreadBlock.Reset();
                    if (serverSocket != null)
                    {
                        serverSocket.Close();
                    }
                }
                else if (cmd == "startconnecting")
                {
                    serverContext.Log("console: start connecting.");
                    connectServiceThreadBlock.Set();
                }
                else if (cmd == "gc")
                {
                    serverContext.Log("gc.");
                    GC.Collect();
                }
                else if (cmd == "exit")
                {
                    serverContext.Log("exit program.");
                    return;
                }
                else
                {
                    Console.WriteLine("unknow command.");
                }
            }
        }

        public void ConnectService()
        {
            int retryTimes = 0;
        label_ConnectService:
            connectServiceThreadBlock.WaitOne();
            serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                IPAddress ip = IPAddress.Any;
                IPEndPoint endPoint = new IPEndPoint(ip, serverContext.port);
                serverSocket.Bind(endPoint);
                serverSocket.Listen(10);
                while (true)
                {
                    Socket socket = serverSocket.Accept();
                    User user = new User();
                    user.socket = socket;

                    if ((socket.RemoteEndPoint as IPEndPoint).Address.Equals((socket.LocalEndPoint as IPEndPoint).Address))
                    {
                        user.authority = int.MaxValue;
                    }

                    Task.Factory.StartNew(UserConnectService, user, TaskCreationOptions.LongRunning);
                }
            }
            catch (Exception e)
            {
                serverContext.Log(e);
                Thread.Sleep(retryTimes < 10 ? 500 : 5000);
                retryTimes++;
                goto label_ConnectService;
            }
            finally
            {
                serverSocket.Close();
            }
            return;
        }

        public void UserConnectService(object _serverUserContext)
        {
            User user = (User)_serverUserContext;
            Socket socket = user.socket;
            NetContext netContext = user.netContext;
            serverContext.Log(string.Format("connected. {0} {1}", socket.RemoteEndPoint, socket.LocalEndPoint));
            PlayerInfo playerInfo = null;
            DateTime lastNetActionTime = DateTime.Now;
            try
            {
                netContext.Init(socket);
                Task.Factory.StartNew(netContext.ReceiveService, TaskCreationOptions.LongRunning);
                netContext.SendStringCompressed((int)NetMessageType.License, serverContext.License);

                playerInfo = new PlayerInfo();
                playerInfo.currentUser = user;
                playerInfo.PlayerGuid = Guid.NewGuid();
                world.playerEnterQueue.Enqueue(playerInfo);
                while (true)
                {
                    netContext.receiveEvent.WaitOne();
                    if (!socket.Connected) return;
                    var readList = netContext.receivedBlocks.GetReadList();
                    foreach (var block in readList)
                    {
                        switch (block.header)
                        {
                            case (int)NetMessageType.Move:
                                playerInfo.PositionI = VectorExt.Int3FromBytes(new Span<byte>(block.data, 0, 12));
                                playerInfo.PositionF = VectorExt.Vector3FromBytes(new Span<byte>(block.data, 12, 12));
                                playerInfo.Rotation = VectorExt.QuaternionFromBytes(new Span<byte>(block.data, 24, 16));
                                break;
                        }
                    }
                    DateTime now = DateTime.Now;
                    playerInfo.lastActivateTime = now;
                    user.SendPacks();

                    //if (now - lastNetActionTime > TimeSpan.FromMilliseconds(10))
                    //{
                    //    lastNetActionTime = now;
                    //    foreach (var chunkPosition in world.VisibleChunks(playerInfo))
                    //    {
                    //        var chunk1 = world.GetChunk(chunkPosition);
                    //        if (playerInfo.visibleChunks.TryGetValue(chunkPosition, out var timeStamp1))
                    //        {
                    //            if (chunk1 != null && chunk1.stamp > timeStamp1)
                    //            {
                    //                playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;
                    //                chunk1.Send(netContext);
                    //            }
                    //        }
                    //        else if (chunk1 != null)
                    //        {
                    //            playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;
                    //            chunk1.Send(netContext);
                    //        }
                    //    }
                    //    playerInfo.RemoveUnusedChunks();
                    //}
                }
            }
            catch (Exception e)
            {
                user.Log(e);
            }
            finally
            {
                serverContext.Log(string.Format("connection close. {0} {1}", socket.RemoteEndPoint, socket.LocalEndPoint));
                if (playerInfo != null)
                {
                    world.playerLeaveQueue.Enqueue(playerInfo);
                }
                socket.Close();
            }
        }
    }
}
