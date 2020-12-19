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
using RTUGameServer.Compression;
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

namespace RTUGameServer
{
    public class AppCore
    {
        public ServerContext serverContext = new ServerContext();
        public Thread connectServiceThread;
        public Socket serverSocket;
        public ManualResetEvent connectServiceThreadBlock = new ManualResetEvent(true);
        public GameCore gameCore;

        public void Initialize(string[] args)
        {
            LZ4.LoadLib();
            serverContext.License = System.IO.File.ReadAllText("COPING.txt");
            serverContext.port = 25565;

            Console.WriteLine("Core Initialize.");
        }

        public void LaunchBaseService()
        {
            gameCore = new GameCore();
            gameCore.gameContext.DefaultSettings();
            gameCore.Save(System.IO.File.OpenWrite("test.zip"));
            Task.Factory.StartNew(gameCore.GameService, TaskCreationOptions.LongRunning);
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
                    ServerUserContext serverUserContext = new ServerUserContext();
                    serverUserContext.socket = socket;
                    var testEndPoint = socket.RemoteEndPoint as IPEndPoint;
                    var testEndPoint2 = socket.LocalEndPoint as IPEndPoint;
                    if (testEndPoint.Address.Equals(testEndPoint2.Address))
                    {
                        serverUserContext.authority = int.MaxValue;
                    }

                    Task.Factory.StartNew(UserConnectService, serverUserContext, TaskCreationOptions.LongRunning);
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
            ServerUserContext serverUserContext = (ServerUserContext)_serverUserContext;
            ref Socket socket = ref serverUserContext.socket;
            NetContext netContext = new NetContext();
            serverContext.Log(string.Format("connected. {0} {1}", socket.RemoteEndPoint, socket.LocalEndPoint));
            PlayerInfo playerInfo = null;
            DateTime lastNetActionTime = DateTime.Now;
            try
            {
                NetDataWrapLayout wrapLayout = new NetDataWrapLayout();
                wrapLayout.Init(netContext, socket);
                Task.Factory.StartNew(wrapLayout.ReceiveService, TaskCreationOptions.LongRunning);
                wrapLayout.SendStringCompressed(1701013868 /*ecil*/, serverContext.License);

                playerInfo = new PlayerInfo();
                playerInfo.playerGuid = Guid.NewGuid();
                lock (gameCore.gameContext.L_playerEnterList)
                {
                    gameCore.gameContext.L_playerEnterList.Add(playerInfo);
                }
                while (true)
                {
                    netContext.receiveEvent.WaitOne();
                    if (!socket.Connected) return;
                    netContext.SwapDynamicContext();
                    for (int i = 0; i < netContext.m_receivedBlock_r.Count; i++)
                    {
                        var block = netContext.m_receivedBlock_r[i];
                        switch (block.header)
                        {
                            case 1702260589/*'evom'*/:
                                playerInfo.positionI.x = BitConverter.ToInt32(block.data, 0);
                                playerInfo.positionI.y = BitConverter.ToInt32(block.data, 4);
                                playerInfo.positionI.z = BitConverter.ToInt32(block.data, 8);
                                playerInfo.positionF.X = BitConverter.ToSingle(block.data, 12);
                                playerInfo.positionF.Y = BitConverter.ToSingle(block.data, 16);
                                playerInfo.positionF.Z = BitConverter.ToSingle(block.data, 20);
                                playerInfo.rotation.X = BitConverter.ToSingle(block.data, 24);
                                playerInfo.rotation.Y = BitConverter.ToSingle(block.data, 28);
                                playerInfo.rotation.Z = BitConverter.ToSingle(block.data, 32);
                                playerInfo.rotation.W = BitConverter.ToSingle(block.data, 36);
                                DateTime now = DateTime.Now;
                                if (now - lastNetActionTime > TimeSpan.FromMilliseconds(10))
                                {
                                    lastNetActionTime = now;
                                    INT3 aPos = playerInfo.positionI.AlignedDown(16);
                                    for (int x = -128; x < 144; x += 16)
                                        for (int z = -128; z < 144; z += 16)
                                        {
                                            INT2 testPoint = new INT2(x + aPos.x, z + aPos.z);

                                            if (gameCore.gameContext.regionInfo.TryGetValue(testPoint, out var regionInfo))
                                            {
                                                for (int y = regionInfo.m_minHeight; y < regionInfo.m_maxHeight; y += 16)
                                                {
                                                    var chunkPosition = new INT3(aPos.x + x, y, aPos.z + z);
                                                    var chunk1 = gameCore.GetExistChunk(chunkPosition.x, chunkPosition.y, chunkPosition.z);
                                                    if (playerInfo.visibleChunks.TryGetValue(chunkPosition, out var timeStamp1))
                                                    {
                                                        if (chunk1 != null && chunk1.stamp > timeStamp1)
                                                        {
                                                            playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;
                                                            NetBlock netBlock = netContext.GetNetBlock(1802856547/*'kuhc'*/, Chunk.c_minimumSize);
                                                            GUtility.ToNetBlock(chunk1, netBlock);
                                                            wrapLayout.SendNetBlock(netBlock);
                                                        }
                                                    }
                                                    else if (chunk1 != null)
                                                    {
                                                        playerInfo.visibleChunks[chunkPosition] = chunk1.stamp;

                                                        NetBlock netBlock = netContext.GetNetBlock(1802856547/*'kuhc'*/, Chunk.c_minimumSize);
                                                        GUtility.ToNetBlock(chunk1, netBlock);
                                                        wrapLayout.SendNetBlock(netBlock);
                                                    }
                                                }
                                                playerInfo.visibleRegions[testPoint] = regionInfo.m_recentUpdateTimeStamp;
                                            }
                                        }

                                    List<INT3> chunkCullResult = new List<INT3>();
                                    foreach (var pair in playerInfo.visibleChunks)
                                    {
                                        INT3 a = pair.Key - playerInfo.positionI + new INT3(8, 8, 8);
                                        if (Math.Abs(a.x) + Math.Abs(a.z) > 512)
                                            chunkCullResult.Add(pair.Key);
                                    }
                                    for (int j = 0; j < chunkCullResult.Count; j++)
                                        playerInfo.visibleChunks.Remove(chunkCullResult[j]);
                                }
                                break;
                        }
                    }
                }
            }
            catch (Exception e)
            {
                serverUserContext.Log(e);
            }
            finally
            {
                serverContext.Log(string.Format("connection close. {0} {1}", socket.RemoteEndPoint, socket.LocalEndPoint));
                if (playerInfo != null)
                {
                    lock (gameCore.gameContext.L_playerLeaveList)
                    {
                        gameCore.gameContext.L_playerLeaveList.Add(playerInfo);
                    }
                }
                socket.Close();
            }
        }
    }
}
