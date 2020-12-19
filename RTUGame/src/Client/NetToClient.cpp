//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
#include "stdafx.h"
#include "NetToClient.h"

void NetToClient::Process(NetContext* netContext, ClientContext* clientContext)
{
	netContext->SwapDynamicContext();

	for (int i = 0; i < netContext->m_smallSizeReceiedDatas_r->size(); i++)
	{
		auto& data = (*netContext->m_smallSizeReceiedDatas_r)[i];
		switch (data.m_header)
		{
		case 'kuhc':
			if (data.m_size >= Chunk16::c_minimumSize)
			{
				auto* chunkc = new ClientChunk();
				Chunk16& chunk = chunkc->m_chunk;
				chunk.m_position = *(DirectX::XMINT3*)&data.m_data[0];
				chunk.m_flags = *(CHUNK_FLAGS*)&data.m_data[12];
				memcpy(chunk.m_data, &data.m_data[Chunk16::c_headerSize], sizeof(Chunk16::m_data));
				clientContext->m_preloadChunks.push(chunkc);
			}
			break;
		}
	}
	if (clientContext->m_currentFrameStamp - netContext->m_lastSendTimeStamp > (10000000 / 20))
	{
		netContext->m_lastSendTimeStamp = clientContext->m_currentFrameStamp;
		char movePkgData[120] = {};
		*(int*)&movePkgData[0] = clientContext->m_clientPlayer.m_position_i.x;
		*(int*)&movePkgData[4] = clientContext->m_clientPlayer.m_position_i.y;
		*(int*)&movePkgData[8] = clientContext->m_clientPlayer.m_position_i.z;
		*(float*)&movePkgData[12] = clientContext->m_clientPlayer.m_position_f.x;
		*(float*)&movePkgData[16] = clientContext->m_clientPlayer.m_position_f.y;
		*(float*)&movePkgData[20] = clientContext->m_clientPlayer.m_position_f.z;
		*(float*)&movePkgData[24] = clientContext->m_clientPlayer.m_rotation.x;
		*(float*)&movePkgData[28] = clientContext->m_clientPlayer.m_rotation.y;
		*(float*)&movePkgData[32] = clientContext->m_clientPlayer.m_rotation.z;
		*(float*)&movePkgData[36] = clientContext->m_clientPlayer.m_rotation.w;

		netContext->Send('evom', movePkgData, 120);
	}
	netContext->m_smallSizeReceiedDatas_r->clear();
	netContext->m_bigSizeReceiedDatas_r->clear();
}
