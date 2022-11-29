#pragma once
#include <vector>
#include "core/glm_math.h"
#include <chrono>
#include <iostream>

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


namespace Nata
{
	class NGameMode;
	class NWorld;
	class NObject;
	class EEntity;
	class CComponent;

	// Everything that is instanced, has ID and is serialized in engine
	class NObject
	{
	public:
		unsigned int ID;
	};

	// Entity components
	class CComponent : public NObject
	{
	protected:
		EEntity* m_Owner;

		friend class EEntity;

	public:
		CComponent()
		{
			m_Owner = nullptr;
		}

		CComponent(EEntity* owner)
		{
			m_Owner = owner;
		}

		EEntity* GetOwner() { return m_Owner; }
		void SetOwner(EEntity* owner) { m_Owner = owner; }

		virtual void Begin() {};
		virtual void Tick(float dt) {};
	};

	class CTransform : public CComponent
	{
	public:
		vec3 Position;
		vec3 Scale;
		vec3 Rotation;

		CTransform()
		{
			Position = vec3(0.f, 0.f, 0.f);
			Scale = vec3(1.f, 1.f, 1.f);
			Rotation = vec3(0.f, 0.f, 0.f);
		}
	};

	class EEntity : public NObject
	{
	public:
		class CTransform* Transform;

	protected:
		NWorld* m_World;
		vector<CComponent*> m_Components;
		friend class NWorld;

	public:
		EEntity()
		{
			Transform = AddComponent<CTransform>();
			m_World = nullptr;
		}

		~EEntity()
		{
			for (CComponent* component : m_Components)
			{
				delete component;
			}
		}

		NWorld* GetWorld() { return m_World; }
		void SetWorld(NWorld* world) { m_World = world; }

		// Create and add component of type
		template<typename T, class = typename std::enable_if<std::is_base_of<CComponent, T>::value>::type>
		T* AddComponent()
		{
			T* comp = new T();
			comp->m_Owner = this;
			m_Components.push_back(comp);
			return comp;
		}

		template<typename T>
		T* GetComponent()
		{
			for (CComponent* comp : m_Components)
			{
				if (typeid(*comp).name() == typeid(T).name())
				{
					return (T*)comp;
				}
			}
		}

		vector<CComponent*> GetAllComponents()
		{
			return m_Components;
		}

		virtual void Begin(){};
		virtual void Tick(float dt){};
	};

	// behaviour in level lifetime
	class NGameMode
	{
	protected:
		NWorld* m_World;
		friend class NWorld;

	public:
		NGameMode()
		{
			m_World = nullptr;
		}

		NWorld* GetWorld() { return m_World; }
		void SetWorld(NWorld* world) { m_World = world; }

		virtual void Begin(){};
		virtual void Tick(float dt){};
	};

	// behaviour in application lifetime
	class NGameInstance
	{
	public:
		virtual void Begin(){};
		virtual void Tick(float dt){};
	};

	// space where entities are inserted in
	class NWorld
	{
	protected:
		NGameMode* m_GameMode;
		vector<EEntity*> m_Entities;

	public:
		NWorld()
		{
			m_GameMode = nullptr;
		}

		vector<EEntity*> GetAllEntities() { return m_Entities; }
		void SetGameMode(NGameMode* gameMode) 
		{
			gameMode->m_World = this;
			m_GameMode = gameMode; 
		}

		template<typename T, class = typename std::enable_if<std::is_base_of<EEntity, T>::value>::type>
		T* Instantiate()
		{
			T* entity = new T();
			m_Entities.push_back(entity);
			entity->m_World = this;
			entity->Transform->Position = vec3(0.f);
			return entity;
		}

		template<typename T, class = typename std::enable_if<std::is_base_of<EEntity, T>::value>::type>
		T* Instantiate(vec3 position)
		{
			T* entity = new T();
			m_Entities.push_back(entity);
			entity->m_World = this;
			entity->Transform->Position = position;
			entity->Transform->Rotation = vec3(0.f);
			return entity;
		}

		template<typename T, class = typename std::enable_if<std::is_base_of<EEntity, T>::value>::type>
		T* Instantiate(vec3 position, vec3 rotation)
		{
			T* entity = new T();
			m_Entities.push_back(entity);
			entity->m_World = this;
			entity->Transform->Position = position;
			entity->Transform->Rotation = rotation;
			return entity;
		}

		void Destroy(EEntity* entity)
		{
			for (unsigned int i = 0; i < m_Entities.size(); i++)
			{
				if (m_Entities[i] == entity) 
				{
					m_Entities.erase(m_Entities.begin() + i);
					return;
				}
			}
			std::cout << "WARNING::DESTROY: OBJECT IS NOT INSTANTIATED" << std::endl;
		}

		void Begin()
		{
			m_GameMode->Begin();
			for (EEntity* entity : m_Entities)
			{
				entity->Begin();
				for (CComponent* comp : entity->m_Components)
				{
					comp->Begin();
				}
			}
		}

		void Tick(float dt)
		{
			m_GameMode->Tick(dt);
			for (EEntity* entity : m_Entities)
			{
				entity->Tick(dt);
				for (CComponent* comp : entity->m_Components)
				{
					comp->Tick(dt);
				}
			}
		}
	};
} 