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
#include "UserInput.h"
#include "../Math/MathCommon.h"
#include "imgui.h"
using namespace DirectX;
void inline _pressButton(InputDesc1& desc1, uint64_t timeStamp)
{
	if (!desc1.pressed)
	{
		desc1.pressed = true;
		desc1.timeStamp = timeStamp;
	}
}

void UserInput::Process(ClientContext* context, AppMessages* messages)
{
	ClientPlayer& playerInfo = context->m_clientPlayer;
	AppMessages& m_messages = *messages;
	auto& io = ImGui::GetIO();

	{
		std::lock_guard<std::mutex> guard(m_messages.lock_sequenceInputs);
		std::swap(m_messages.L_sequenceInputs_w, m_messages.m_sequenceInputs_r);
	}
	auto& input = *m_messages.m_sequenceInputs_r;
	glm::vec2 mouseDelta = {};

	for (int i = 0; i < input.size(); i++)
	{
		auto& inputValue = input[i];
		if (inputValue.Flag == SEQUENCE_INPUT_FLAG_MOUSE_MOVE)
		{
			io.MousePos = ImVec2(inputValue.X, inputValue.Y);
			if (playerInfo.mouseLPressed)
			{
				mouseDelta.x += float(inputValue.X) - playerInfo.mousePosition.x;
				mouseDelta.y += float(inputValue.Y) - playerInfo.mousePosition.y;
				playerInfo.mousePosition = XMUINT2(inputValue.X, inputValue.Y);
			}
		}
		else if (inputValue.Flag == SEQUENCE_INPUT_FLAG_KEYBOARD_DOWN)
		{
			switch (inputValue.Key)
			{
			case VK_LEFT:
			case 'A':
				_pressButton(playerInfo.left, inputValue.TimeStamp);
				break;
			case VK_UP:
			case 'W':
				_pressButton(playerInfo.up, inputValue.TimeStamp);
				break;
			case VK_RIGHT:
			case 'D':
				_pressButton(playerInfo.right, inputValue.TimeStamp);
				break;
			case VK_DOWN:
			case 'S':
				_pressButton(playerInfo.down, inputValue.TimeStamp);
				break;
			case VK_SPACE:
				_pressButton(playerInfo.space, inputValue.TimeStamp);
				break;
			}
		}
		else if (inputValue.Flag == SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_DOWN)
		{
			io.MouseDown[0] = true;

			playerInfo.mouseLPressed = true;
			playerInfo.mousePosition = XMUINT2(inputValue.X, inputValue.Y);
			io.MousePos = ImVec2(inputValue.X, inputValue.Y);
		}
		else if (inputValue.Flag == SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_UP)
		{
			io.MouseDown[0] = false;

			playerInfo.mouseLPressed = false;
			playerInfo.mousePosition = XMUINT2(inputValue.X, inputValue.Y);
			io.MousePos = ImVec2(inputValue.X, inputValue.Y);
		}
		else if (inputValue.Flag == SEQUENCE_INPUT_FLAG_KEYBOARD_UP)
		{
			switch (inputValue.Key)
			{
			case VK_LEFT:
			case 'A':
				playerInfo.left.pressed = false;
				break;
			case VK_UP:
			case 'W':
				playerInfo.up.pressed = false;
				break;
			case VK_RIGHT:
			case 'D':
				playerInfo.right.pressed = false;
				break;
			case VK_DOWN:
			case 'S':
				playerInfo.down.pressed = false;
				break;
			case VK_SPACE:
				playerInfo.space.pressed = false;
				break;
			}
		}
	}
	m_messages.m_sequenceInputs_r->clear();

	float lr = 0.0f;
	float ud = 0.0f;
	if (playerInfo.left.pressed)
	{
		lr -= 1;
	}
	if (playerInfo.up.pressed)
	{
		ud += 1;
	}
	if (playerInfo.right.pressed)
	{
		lr += 1;
	}
	if (playerInfo.down.pressed)
	{
		ud -= 1;
	}

	playerInfo.m_angle.y += mouseDelta.x * 0.01;
	playerInfo.m_angle.x += mouseDelta.y * 0.01;
	playerInfo.m_angle.x = std::min(std::max(playerInfo.m_angle.x, -XM_PI / 2), XM_PI / 2);
	playerInfo.m_angle.y = fmod(playerInfo.m_angle.y, XM_2PI);

	XMStoreFloat4(&playerInfo.m_rotation, XMQuaternionRotationRollPitchYaw(playerInfo.m_angle.x, playerInfo.m_angle.y, playerInfo.m_angle.z));

	XMFLOAT3 _addSpeed(lr, 0, ud);
	auto speed = XMLoadFloat3(&playerInfo.m_currentSpeed);

	speed *= powf(0.005f, std::max(messages->m_deltaTime, 0.0001));

	auto addSpeed = XMLoadFloat3(&_addSpeed) * 80.0f;
	auto rotMatrix = XMMatrixRotationRollPitchYaw(playerInfo.m_angle.x, playerInfo.m_angle.y, playerInfo.m_angle.z);
	addSpeed = XMVector3TransformNormal(addSpeed, rotMatrix);

	speed += addSpeed * messages->m_deltaTime;
	speed = XMVector3ClampLength(speed, 0, 20);

	XMFLOAT3 deltaPos;
	XMStoreFloat3(&deltaPos, speed * messages->m_deltaTime);
	context->m_physicsDevice->SceneMoveController(context->m_physicsScene.get(), context->m_physicsController.get(), glm::vec3(deltaPos.x, deltaPos.y, deltaPos.z), messages->m_deltaTime);
	XMFLOAT3 realPos;
	XMFLOAT4 realRot;
	context->m_physicsDevice->GetControllerTransform(context->m_physicsController.get(), (glm::vec3*)&realPos, (glm::vec4*)&realRot);

	auto position = XMLoadFloat3(&realPos);
	auto p1 = XMVectorRound(position);
	position -= p1;
	XMStoreFloat3(&playerInfo.m_currentSpeed, speed);
	XMStoreFloat3(&playerInfo.m_position_f, position);
	glm::i32vec3 deltaPos_i;
	XMFLOAT3 deltaPos_if;
	XMStoreSInt3((XMINT3*)&deltaPos_i, p1);
	XMStoreFloat3(&deltaPos_if, p1);
	playerInfo.m_position_i = MathAdd(playerInfo.m_position_i, deltaPos_i);
	context->m_physicsDevice->SceneShiftOrigin(context->m_physicsScene.get(), glm::vec3(deltaPos_if.x, deltaPos_if.y, deltaPos_if.z));

}
