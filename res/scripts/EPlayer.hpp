#pragma once
#include "nata.h"
#include "core/ecs/CMeshRenderer.hpp"
#include "core/comp/CBoxCollider.hpp"

namespace Nata
{
	class EPlayer : public EEntity
	{
	public:
		CModelRenderer* MeshRenderer;
		CBoxCollider* BoxCollider;
		NModel* Model;
		bool flipflop;
		vec3 Color;
		bool InputEnabled;

	public:
		EPlayer() : EEntity()
		{
			MeshRenderer = AddComponent<CModelRenderer>();
			AddComponent<CBoxCollider>();

			BoxCollider = GetComponent<CBoxCollider>();

			NShader* shader = new NShader("src\\shaders\\unlit.vert", "src\\shaders\\unlit.frag");
			Model = new NModel("res\\models\\cube.obj");
			MeshRenderer->Init(shader, Model);

			Color = vec3(0.f);
			InputEnabled = true;
			flipflop = true;

			MeshRenderer->SetVisibility(flipflop);
			Model->PropertyLayout.AddVec3("color");
		}

		void Begin() override
		{
		}

		void Tick(float dt) override
		{
			Handles::DrawHandles(Transform, vec3(2.f));

			Movement(dt);
			Model->PropertyLayout.SetVec3("color", Color);

			vec3 hBound = BoxCollider->Bounds / 2.f;
			BoxCollider->Position = Transform->Position;
			BoxCollider->Box = NBox(
				NRange(BoxCollider->Position.x - hBound.x, BoxCollider->Position.x + hBound.x),
				NRange(BoxCollider->Position.y - hBound.y, BoxCollider->Position.y + hBound.y),
				NRange(BoxCollider->Position.z - hBound.z, BoxCollider->Position.z + hBound.z)
			);


			//std::vector<CBoxCollider*> cols = GetAllComponentsOfType<CBoxCollider>(GetWorld());
			//bool intersecting = false;
			//for (CBoxCollider* col : cols)
			//{
			//	if (col == BoxCollider)
			//	{
			//		continue;
			//	}
			//	intersecting = Intersect(BoxCollider->Box, col->Box);
			//}

			//if (intersecting)
			//{
			//	Handles::DrawWireCube(BoxCollider->Position, BoxCollider->Bounds, vec3(0.f, 1.f, 0.f));
			//}
			//else
			//{
			//	Handles::DrawWireCube(BoxCollider->Position, BoxCollider->Bounds, vec3(1.f, 1.f, 1.f));
			//}
		}

		void Movement(float dt)
		{
			if (!InputEnabled)
			{
				return;
			}
			vec2 input = vec2(0.f);
			float speed = 1.f;
			if (NEngine::Input->GetKeyHold(GLFW_KEY_LEFT))
			{
				input.x = -1;
			}
			if (NEngine::Input->GetKeyHold(GLFW_KEY_RIGHT))
			{
				input.x = 1;
			}
			if (NEngine::Input->GetKeyHold(GLFW_KEY_UP))
			{
				input.y = 1;
			}
			if (NEngine::Input->GetKeyHold(GLFW_KEY_DOWN))
			{
				input.y = -1;
			}
			Transform->Position.x += input.x * speed * dt;
			Transform->Position.y += input.y * speed * dt;
			Transform->Rotation = Lerp(Transform->Rotation, vec3(0.f, 0.f, -input.x * 20.f), 5.f * dt);

			if (NEngine::Input->GetKeyDown(GLFW_KEY_SPACE))
			{
				flipflop = !flipflop;
				MeshRenderer->SetVisibility(flipflop);
			}
		}

		vec3 Lerp(vec3 start, vec3 end, float a)
		{
			return (start + a * (end - start));
		}
	};
}