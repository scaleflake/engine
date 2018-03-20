#include "../Precompiled.h"

Collision::Collision(Body* a, Body* b) {
	this->A = a;
	this->B = b;
}

void Collision::Solve() {
	ShapeToShape(A, B, this);
}

void Collision::Initialize() {
	// Calculate average restitution
	restitution = std::min(A->restitution, B->restitution);

	// Calculate static and dynamic friction
	staticFriction = std::sqrt(A->staticFriction * B->staticFriction);
	dynamicFriction = std::sqrt(A->dynamicFriction * B->dynamicFriction);

#ifdef GRAVITY
	for (byte i = 0; i < contactCount; ++i) {
		// Calculate radii from COM to contact
		vec2 ra = contacts[i] - A->position;
		vec2 rb = contacts[i] - B->position;

		float relative = glm::length(B->velocity + cross(B->angularVelocity, rb)
								   - A->velocity - cross(A->angularVelocity, ra));
		float gravityLength = glm::length(gravity * CONST_DELTATIME);

		// Determine if we should perform a resting collision or not
		// The idea is if the only thing moving this object is gravity,
		// then the collision should be performed without any restitution
		if (relative * relative < gravityLength * gravityLength + CONST_EPSILON) // LENSQR
			restitution = 0.0f;
	}
#endif // GRAVITY
}

void Collision::ApplyImpulse() {
    for (byte i = 0; i < contactCount; ++i) {
		// Calculate radii from COM to contact
		vec2 rposa = contacts[i] - A->position;
		vec2 rposb = contacts[i] - B->position;

		// Relative velocity
		vec2 relative = B->velocity + cross(B->angularVelocity, rposb)
					  - A->velocity - cross(A->angularVelocity, rposa);

		// Relative velocity along the normal
		float contactVel = glm::dot(relative, normal);

		// Do not resolve if velocities are separating
		if (contactVel > 0) return;

		float raCrossN = cross(rposa, normal);
		float rbCrossN = cross(rposb, normal);
		float invInvMassSum = 1.0f / (A->invMass + raCrossN * raCrossN * A->invMoment
									+ B->invMass + rbCrossN * rbCrossN * B->invMoment);
		float invContactCount = 1.0f / contactCount;

		// Calculate impulse scalar
		float j = -(1.0f + restitution) * contactVel * invInvMassSum / invContactCount;

		// Apply impulse
		vec2 impulse = normal * j;
		A->velocity += A->invMass * -impulse;
		A->angularVelocity += A->invMoment * cross(rposa, -impulse);
		B->velocity += B->invMass *  impulse;
		B->angularVelocity += B->invMoment * cross(rposb, impulse);

		// Friction impulse
		relative = B->velocity + cross(B->angularVelocity, rposb)
				 - A->velocity - cross(A->angularVelocity, rposa);

		vec2 t = Normalize(relative - (normal * glm::dot(relative, normal)));

		// j tangent magnitude
		float jt = -1.0f * glm::dot(relative, t) / invInvMassSum / invContactCount;

		// Don't apply tiny friction impulses
		if (Equal(jt, 0.0f)) return;

		// Coulumb's law
		vec2 tangentImpulse;
		if (std::abs(jt) < j * staticFriction)
			tangentImpulse = t * jt;
		else
			tangentImpulse = t * -j * dynamicFriction;

		// Apply friction impulse
		A->velocity += A->invMass * -tangentImpulse;
		A->angularVelocity += A->invMoment * cross(rposa, -tangentImpulse);
		B->velocity += B->invMass *  tangentImpulse;
		B->angularVelocity += B->invMoment * cross(rposb, tangentImpulse);
	}
}

void Collision::PositionalCorrection() {
	float a = penetration - CONST_SLOP;
	if (a > 0.0f) {
		vec2 correction = (a / (A->invMass + B->invMass)) * normal * CONST_CORRECTION;
		A->position -= correction * A->invMass;
		B->position += correction * B->invMass;
	}
}

void Collision::Draw() const {
	for (byte i = 0; i < contactCount; ++i) {
		vec2 contact = contacts[i];
		Renderer::RenderRay(contact, 0.3f * normal);
		Renderer::RenderPoint(contact);
	}
}
