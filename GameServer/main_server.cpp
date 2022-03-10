#include <conio.h>
#include <iostream>

#include "udp_server.h"
#include "globalThings.h"

#include <physics/cParticleWorld.h>
#include <physics/random_helpers.h>
#include <physics/particle_contact_generators.h>

#include <stdlib.h>
#include <string>
#include <vector>
#include "GLCommon.h"
#include "GLMCommon.h"

int main(int argc, char** argv)
{
	UDPServer server;
	int ch;

	// Camera will be in -z and facing the xy-plane, paddles will be locked at z=0 and move along the xy-plane
	// The specific numbers will need to be tweaked but esentially the left wall will be player 1 (+x - plane faces -x) maybe 10 units
	// The right wall will be for player 2 (-x - plane faces +x) maybe 10 units away
	// Top is player 3 (+y - plane faces -y) maybe 5 units away
	// and bottom is player 4 (-y - plane faces +y) maybe 5 units away
	// As well as a "front" and "back" to the box just to keep the ball in play (+-1 z should be more than enough)
	// 6 walls to keep the ball in play
	nPhysics::cPlaneParticleContactGenerator* leftWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(-1.0f, 0.0f, 0.0f), -10.0f);
	::g_pWorld->AddContactGenerator(leftWall);

	nPhysics::cPlaneParticleContactGenerator* rightWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(1.0f, 0.0f, 0.0f), -10.0f);
	::g_pWorld->AddContactGenerator(rightWall);

	nPhysics::cPlaneParticleContactGenerator* topWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(0.0f, -1.0f, 0.0f), -5.0f);
	::g_pWorld->AddContactGenerator(topWall);

	nPhysics::cPlaneParticleContactGenerator* bottomWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(0.0f, 1.0f, 0.0f), -5.0f);
	::g_pWorld->AddContactGenerator(bottomWall);

	nPhysics::cPlaneParticleContactGenerator* frontWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f);
	::g_pWorld->AddContactGenerator(frontWall);

	nPhysics::cPlaneParticleContactGenerator* backWall = new nPhysics::cPlaneParticleContactGenerator(glm::vec3(0.0f, 0.0f, -1.0f), -1.0f);
	::g_pWorld->AddContactGenerator(backWall);

	nPhysics::cParticle2ParticleContactGenerator* particleContacts = new nPhysics::cParticle2ParticleContactGenerator();
	::g_pWorld->AddContactGenerator(particleContacts);

	nPhysics::cParticle* ball = new nPhysics::cParticle(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	ball->SetRadius(0.5f);
	ball->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	ball->SetPlayer(0);
	::g_pWorld->AddParticle(ball);
	::g_pVecParticles.push_back(ball);

	// Add the particles for the player's paddles
	nPhysics::cParticle* player1 = new nPhysics::cParticle(0.0f, glm::vec3(10.0f, 0.0f, 0.0f));
	player1->SetRadius(0.5f);
	player1->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	player1->SetPlayer(1);
	::g_pWorld->AddParticle(player1);
	::g_pVecParticles.push_back(player1);

	nPhysics::cParticle* player2 = new nPhysics::cParticle(0.0f, glm::vec3(-10.0f, 0.0f, 0.0f));
	player2->SetRadius(0.5f);
	player2->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	player2->SetPlayer(2);
	::g_pWorld->AddParticle(player2);
	::g_pVecParticles.push_back(player2);

	nPhysics::cParticle* player3 = new nPhysics::cParticle(0.0f, glm::vec3(0.0f, 5.0f, 0.0f));
	player3->SetRadius(0.5f);
	player3->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	player3->SetPlayer(3);
	::g_pWorld->AddParticle(player3);
	::g_pVecParticles.push_back(player3);

	nPhysics::cParticle* player4 = new nPhysics::cParticle(0.0f, glm::vec3(0.0f, -5.0f, 0.0f));
	player4->SetRadius(0.5f);
	player4->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	player4->SetPlayer(4);
	::g_pWorld->AddParticle(player4);
	::g_pVecParticles.push_back(player4);

	const double MAX_DELTA_TIME = 0.1;	//100 ms
	double previousTime = glfwGetTime();
	double deltaTime;

	while (1)
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		deltaTime = (deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime);
		previousTime = currentTime;

		// Server is actually running too fast for the physics system, deltaTime here is 0
		//::g_pWorld->TimeStep(deltaTime);
		//std::cout << deltaTime << std::endl;

		if (_kbhit())
		{
			ch = _getch();

			if (ch == 27) break;

			//https://cherrytree.at/misc/vk.htm
			// 1 Sets update frequency to 1hz
			if (ch == 49)
			{
				server.UPDATES_PER_SEC = 1;
				std::cout << "Server Frequency Now: 1Hz" << std::endl;
			}
			// 2 Sets update frequency to 10hz
			if (ch == 50)
			{
				server.UPDATES_PER_SEC = 10;
				std::cout << "Server Frequency Now: 10Hz" << std::endl;
			}
			// 3 Sets update frequency to 60hz
			if (ch == 51)
			{
				server.UPDATES_PER_SEC = 60;
				std::cout << "Server Frequency Now: 60Hz" << std::endl;
			}
			// 4 Sets update frequency to 100hz
			if (ch == 52)
			{
				server.UPDATES_PER_SEC = 100;
				std::cout << "server Frequency Now: 100Hz" << std::endl;
			}

		}
		server.Update();
	}

	delete ::g_pWorld;

	return 0;
}