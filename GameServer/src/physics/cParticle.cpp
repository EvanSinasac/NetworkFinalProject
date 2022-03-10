#include <physics/cParticle.h>
#include <iostream>
#include "../../globalThings.h"

namespace nPhysics
{
	cParticle::cParticle(float mass, const glm::vec3& position)
		: mPosition(position)
		, mVelocity(0.f)
		, mAcceleration(0.f)
		, mDamping(0.995f)
		, mAppliedForce(0.f)
		, mIsAlive(true)
		, mRadius (1.0f)
	{
		SetMass(mass);
		// 0 is the ball
		// 1-4 is the player's paddles
		this->player = 0;
	}
	cParticle::~cParticle()
	{

	}

	void cParticle::SetDamping(float damping)
	{
		mDamping = glm::clamp(damping, 0.f, 1.f);
	}

	float cParticle::GetDamping() const
	{
		return mDamping;
	}

	void cParticle::SetIsAlive(bool isAlive)
	{
		mIsAlive = isAlive;
	}

	bool cParticle::GetIsAlive() const
	{
		return mIsAlive;
	}

	void cParticle::SetMass(float mass)
	{
		if (mass <= 0.f)
		{
			mInverseMass = 0.f;
		}
		else
		{
			mInverseMass = 1.f / mass;
		}
	}

	float cParticle::GetMass() const
	{
		if (mInverseMass == 0.f)
		{
			return mInverseMass;
		}
		return 1.0f / mInverseMass;
	}

	float cParticle::GetInverseMass() const
	{
		return mInverseMass;
	}

	bool cParticle::IsStaticObject() const
	{
		return mInverseMass == 0.f;
	}

	glm::vec3 cParticle::GetPosition() const
	{
		return mPosition;
	}

	void cParticle::GetPosition(glm::vec3& position)
	{
		position = mPosition;
	}

	void cParticle::SetPosition(const glm::vec3& position)
	{
		mPosition = position;
	}

	glm::vec3 cParticle::GetVelocity() const
	{
		return mVelocity;
	}

	void cParticle::GetVelocity(glm::vec3& velocity)
	{
		velocity = mVelocity;
	}

	void cParticle::SetVelocity(const glm::vec3& velocity)
	{
		mVelocity = velocity;
	}

	glm::vec3 cParticle::GetAcceleration() const
	{
		return mAcceleration;
	}

	void cParticle::GetAcceleration(glm::vec3& acceleration)
	{
		acceleration = mAcceleration;
	}

	void cParticle::SetAcceleration(const glm::vec3& acceleration)
	{
		mAcceleration = acceleration;
	}

	void cParticle::ApplyForce(const glm::vec3& force)
	{
		mAppliedForce += force;
	}

	void cParticle::ApplyImpulse(const glm::vec3& impulse)
	{
		mVelocity += impulse * mInverseMass;
	}

	void cParticle::SetRadius(float radius)
	{
		mRadius = radius;
	}
	float cParticle::GetRadius()
	{
		return mRadius;
	}

	void cParticle::ClearAppliedForces()
	{
		glm::set(mAppliedForce, 0.f, 0.f, 0.f);
	}

	void cParticle::Integrate(float deltaTime)
	{
		if (mInverseMass == 0.f)
		{
			return; // static things don't move!
		}

		// Integrating with Euler algorithm
		mPosition += mVelocity * deltaTime;
		// F*(1/m) = a
		mVelocity += (mAcceleration + mAppliedForce * mInverseMass) * deltaTime;

		// Apply damping
		mVelocity *= glm::pow(mDamping, deltaTime);

		// This is super unsafe but it'll apply to all of them
		// Keep the particle within the bounds of the play area
		// Depending on the speed of the server this shouldn't matter, faster means smaller movements per frame
		// Slower means larger movement per frame
		if (mPosition.x > 10.0f)
		{
			mPosition.x = 10.0f;
		}
		else if (mPosition.x < -10.0f)
		{
			mPosition.x = -10.0f;
		}

		if (mPosition.y > 5.0f)
		{
			mPosition.y = 5.0f;
		}
		else if (mPosition.y < -5.0f)
		{
			mPosition.y = -5.0f;
		}

		if (mPosition.z > 1.0f)
		{
			mPosition.z = 1.0f;
		}
		else if (mPosition.z < -1.0f)
		{
			mPosition.z = -1.0f;
		}
		// TODO: Remove this before submission
		//std::cout << GetPlayer() << " is at: X: "
		//	<< mPosition.x
		//	<< " Y: " << mPosition.y
		//	<< " Z: " << mPosition.z << std::endl;
		//if (this->player == 0)
		//{
		//	::ballPosition = this->mPosition;
		//}
		//else
		//{
		//	::playerPositions[this->player - 1] = this->mPosition;
		//}

		// Clear applied forces
		ClearAppliedForces();
	}


	void cParticle::SetPlayer(int pl)
	{
		this->player = pl;
	}
	int cParticle::GetPlayer()
	{
		return this->player;
	}
}