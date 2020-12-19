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
#pragma once
struct MeshUploadContainer
{
	IRTUMesh* m_mesh;
	std::shared_ptr<RTUMeshLoader> m_meshLoader;
	MeshUploadContainer(IRTUMesh* mesh, std::shared_ptr<RTUMeshLoader>& meshLoader)
	{
		m_mesh = mesh;
		m_meshLoader = meshLoader;
	}
	MeshUploadContainer()
	{
		m_mesh = nullptr;
		m_meshLoader = nullptr;
	}
};
struct Texture2DUploadContainer
{
	IRTUTexture2D* m_texture;
	std::shared_ptr<RTUTexture2DLoader> m_textureLoader;
	Texture2DUploadContainer(IRTUTexture2D* texture, std::shared_ptr<RTUTexture2DLoader>& textureLoader)
	{
		m_texture = texture;
		m_textureLoader = textureLoader;
	}
};

struct PipelineStateUploadContainer
{
	IRTUPipelineState* m_pipelineState;
	std::shared_ptr<RTUPipelineStateLoader> m_pipelineStateLoader;
};

struct DrawContainer0
{
	DrawContainer0(IRTUMesh* _mesh, DirectX::XMFLOAT4X4 _toWorldMatrix)
	{
		m_toWorldMatrix = _toWorldMatrix;
		m_mesh = _mesh;
	};
	DirectX::XMFLOAT4X4 m_toWorldMatrix;
	int m_startIndex = 0;
	int m_indexCount = 0;
	IRTUMesh* m_mesh = nullptr;
};

struct DrawContainer1
{
	DrawContainer1(IRTUMesh* _mesh, DirectX::XMFLOAT4X4 _toWorldMatrix, IRTUPipelineState* _pipelineState)
	{
		m_toWorldMatrix = _toWorldMatrix;
		m_mesh = _mesh;
		m_pipelineState = _pipelineState;
	}
	DirectX::XMFLOAT4X4 m_toWorldMatrix;
	IRTUMesh* m_mesh = nullptr;
	IRTUPipelineState* m_pipelineState;
};
