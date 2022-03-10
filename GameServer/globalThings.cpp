#include "globalThings.h"

// Physics
nPhysics::cParticleWorld* g_pWorld = new nPhysics::cParticleWorld(50);
std::vector<nPhysics::cParticle*> g_pVecParticles;

// 4 Player pong stuff
glm::vec3 ballPosition;
glm::vec3 playerPositions[4];
