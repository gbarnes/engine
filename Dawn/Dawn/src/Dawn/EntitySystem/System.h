#pragma once

namespace Dawn
{
	class World;
	class DawnType;

	class ISystem
	{
	public:
		~ISystem() = default;

		virtual DawnType* AccessType() const = 0;
		virtual void Update(World* InWorld) = 0;
	};

	/*class MovementSystem : public ISystem
	{
		REGISTER_TYPE(MovementSystem)
	public:

		Type* AccessType() const override
		{
			return MovementSystem::GetType();
		}

		// Geerbt über ISystem
		virtual void Update(Dawn::World * InWorld) override;
	};*/

}