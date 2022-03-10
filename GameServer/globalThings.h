#ifndef _globalThings_HG_
#define _globalThings_HG_

#include "GLCommon.h"
#include <physics/cParticleWorld.h>

// Physics
extern nPhysics::cParticleWorld* g_pWorld;
extern std::vector<nPhysics::cParticle*> g_pVecParticles;

// 4 Player Pong sutff...
extern glm::vec3 ballPosition;
extern glm::vec3 playerPositions[4];



// Here's a more useful templated one, based on this one:
//https://stackoverflow.com/questions/686353/random-float-number-generation
template <class T>
T gGetRandBetween(T LO, T HI)
{
    float fLO = static_cast<float>(LO);
    float fHI = static_cast<float>(HI);
    float r3 = fLO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (fHI - fLO)));
    return r3;
}


#endif // _globalThings_HG_