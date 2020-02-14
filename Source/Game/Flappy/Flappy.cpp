#include "Flappy.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>


using namespace std;

Flappy::Flappy()
{
}

Flappy::~Flappy()
{
}

void Flappy::Init()
{
	camera = new MyFlappy::Camera();
	camera->Set(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 4, 0));


	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("bird");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "bird.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("wings");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "teapot.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("point");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("skybox");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	camera->TranslateRight(3 * pipeWidth);

	resetWorld();
}


void Flappy::FrameStart()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

double Flappy::Distance(glm::vec2 p1, glm::vec2 p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void Flappy::resetWorld()
{
	system("CLS");
	srand(time(NULL));

	// Randomly arrange each pipe
	for (int i = 0; i <= nOfPipes; i++)
	{
		float randomY = (rand() % (int)(pipeHeight)) / 3;
		pipePos[i].x = pipeDistance * (i + initialXOffset * 1.2);
		pipePos[i].y = randomY + initialYOffset;
	}

	// Reset all movement variables
	birdPos.x = middleX;
	birdPos.y = 0;
	birdVelocity = 0;
	birdVerticalVelocity = 1;

	rotation = 0;
	acceleration = 15;
	pipeSpeed = 2.5;
	pipeAcceleration = 0.1;
	score = 0;
	skyboxRotation = 0;
	pipeDistance = 5 * pipeWidth;
	pipeRotation = 0;
	pipeVerticalVelocity = 0;
	pipeHorizontalVelocity = 0;
}


#define MAX_ROTATION (AI_MATH_HALF_PI * 0.6)
#define MAX_WING_ROTATION (MAX_ROTATION * 0.5)
void Flappy::Update(float deltaTimeSeconds)
{
	if (gameStart) {
		// Update pipe position
		for (int i = 0; i < nOfPipes; i++)
		{
			// The pipe didn't leave the screen yet
			if (pipePos[i].x > -pipeWidth) {
				// move it leftwards
				pipePos[i].x -= pipeSpeed * deltaTimeSeconds;

				pipePos[i].x += pipeHorizontalVelocity * deltaTimeSeconds * pipeHorizontalFactor * (float)i / 10.0f;
				pipePos[i].y += pipeVerticalVelocity * deltaTimeSeconds * pipeVerticalFactor;
			}
			else
			{
				// If it's out of the screen, move it back into the pipeline
				pipePos[i].x = nOfPipes * pipeDistance - pipeWidth;
				pipePos[i].y = (rand() % (int)(pipeHeight)) / 3 + initialYOffset;

				// They will start moving vertivally as the game progresses
				if (score % 10 == 0 && score < 30)
					pipeVerticalVelocity = (float)score / 20.0f;


				if (i % 2) {
					pipeVerticalFactor *= -1;
					pipeHorizontalFactor *= -1;
				}

				// Also horizontally as the game gets harder
				if (score >= 30 && score < 60 && score % 10 == 0)
					pipeHorizontalVelocity = (float)score / 40.0f;
			}
		}
		// The pipes get faster overtime
		pipeSpeed += pipeAcceleration * deltaTimeSeconds;

		birdPos.y -= birdVelocity * deltaTimeSeconds;
		birdVelocity += acceleration * deltaTimeSeconds;
		acceleration += pipeAcceleration * deltaTimeSeconds;
		//pipeDistance += pipeAcceleration * deltaTimeSeconds;

		if (birdVelocity < -birdJumpHeight * 0.8)
			rotation += birdVelocity * 1.2 * deltaTimeSeconds;
		else
			rotation += birdVelocity * deltaTimeSeconds;

		if (rotation > MAX_ROTATION)
			rotation = MAX_ROTATION;
		else if (rotation < -MAX_ROTATION)
			rotation = -MAX_ROTATION;

	}
	else
	{
		// The bird also falls faster, to increase reaction speeds
		birdPos.y += birdVerticalVelocity * deltaTimeSeconds * birdVerticalFactor;
		if (birdVerticalVelocity < 4)
			birdVerticalVelocity += deltaTimeSeconds / 10;
		if (abs(birdPos.y) > pipeHeight / 3 && birdVerticalVelocity <= 4)
			birdVerticalFactor *= -1;

	}

	// Check for collisions
	glm::mat4 modelMatrix = glm::mat4(1);
	for (int i = 0; i < nOfPipes; i++) {

		// Only one pair of pipes can be collided with at any given time
		if (birdPos.x >= pipePos[i].x - birdWidth && birdPos.x <= pipePos[i].x + pipeWidth + birdWidth)
		{
			if (lastPipe != i)
			{
				score++;
				cout << "SCORE: " << score << endl;
				lastPipe = i;
			}

			glm::vec2 pipeCollision = pipePos[i];
			pipeCollision.y -= pipeHeight / 2;
			// Check the collising with multiple circles along the length of the pipes
			for (int j = 0; j * (pipeWidth / 2) <= pipeHeight; j++)
			{
				pipeCollision.y += (pipeWidth / 2);

				// Bottom Pipe
				glm::vec2 pipeDrawCircles;
				pipeDrawCircles.x = pipeCollision.x;
				pipeDrawCircles.y = -pipeCollision.y;


				// Top Pipe
				glm::vec2 pipeDrawCircles2;
				pipeDrawCircles2.x = pipeCollision.x;
				pipeDrawCircles2.y = -pipeCollision.y + gapSize + pipeHeight + pipeWidth;


				glm::vec2 actualBirdPos = birdPos;
				actualBirdPos.y += pipeWidth / 2.5;
				double hitboxSize = (birdWidth / 2) + (pipeWidth / 2);
				if (!debug && gameStart && (Distance(actualBirdPos, pipeDrawCircles) < hitboxSize || Distance(actualBirdPos, pipeDrawCircles2) < hitboxSize || abs(actualBirdPos.y) >= 4))
				{
					cout << "GG\n";
					gameStart = false;
					oneMoreSpace = true;
					birdVerticalVelocity = 10;
					birdVerticalFactor = -1;
					break;
				}

				// Draw the collision boxes if debug mode is enabled
				if (debug)
				{
					modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(pipeDrawCircles.x, pipeDrawCircles.y, pipeDepth / 2 + 0.01));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
					RenderMesh(meshes["point"], shaders["VertexColor"], modelMatrix);

					modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(pipeDrawCircles2.x, pipeDrawCircles2.y, pipeDepth / 2 + 0.01));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
					RenderMesh(meshes["point"], shaders["VertexColor"], modelMatrix);

					modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(actualBirdPos.x, actualBirdPos.y, 0.02));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
					RenderMesh(meshes["point"], shaders["VertexColor"], modelMatrix);
				}
			}

		}
		textureIndex = 0;
		// Bottom Pipe
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(pipePos[i].x, -pipePos[i].y, 0));
		modelMatrix = glm::rotate(modelMatrix, pipeRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(pipeWidth, pipeHeight, pipeDepth));
		RenderMesh(meshes["box"], shaders["Pipe"], modelMatrix);

		// Top Pipe
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(pipePos[i].x, pipeHeight + gapSize - pipePos[i].y, 0));
		modelMatrix = glm::rotate(modelMatrix, pipeRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(pipeWidth, pipeHeight, pipeDepth));
		RenderMesh(meshes["box"], shaders["Pipe"], modelMatrix);

	}


	// Bird
	textureIndex = 1;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(birdPos.x, birdPos.y, 0));
	modelMatrix = glm::rotate(modelMatrix, -rotation, glm::vec3(0, 0, 1));
	modelMatrix = glm::rotate(modelMatrix, AI_MATH_HALF_PI_F, glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.003, 0.003, 0.003));
	RenderMesh(meshes["bird"], shaders["Bird"], modelMatrix);

	// Wings
	float wingRotation = rotation;
	if (wingRotation < -MAX_WING_ROTATION)
		wingRotation = -MAX_WING_ROTATION;
	textureIndex = 3;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(birdPos.x - pipeWidth / 5, birdPos.y + pipeWidth / 2.5, 0.15));
	modelMatrix = glm::rotate(modelMatrix, -rotation, glm::vec3(0, 0, 1));
	modelMatrix = glm::rotate(modelMatrix, AI_MATH_HALF_PI_F, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, -wingRotation, glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6, 0.8, 0.1));
	RenderMesh(meshes["wings"], shaders["Bird"], modelMatrix);

	// Skybox
	skyboxRotation += skyboxFactor * deltaTimeSeconds / 10;
	if (abs(skyboxRotation) >= AI_MATH_HALF_PI)
		skyboxFactor *= -1;
	textureIndex = 2;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::rotate(modelMatrix, AI_MATH_PI_F, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, skyboxRotation, glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(50, 50, 50));
	RenderMesh(meshes["skybox"], shaders["Bird"], modelMatrix);

}

void Flappy::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Flappy::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render(textureIndex);
}


void Flappy::OnInputUpdate(float deltaTime, int mods)
{
	float cameraSpeed = 2.0f;

	if (debug)
	{
		if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
		{
			float distance = cameraSpeed * deltaTime;

			if (window->KeyHold(GLFW_KEY_W))
				camera->MoveForward(distance);

			if (window->KeyHold(GLFW_KEY_A))
				camera->TranslateRight(-distance);

			if (window->KeyHold(GLFW_KEY_S))
				camera->MoveForward(-distance);

			if (window->KeyHold(GLFW_KEY_D))
				camera->TranslateRight(distance);

			if (window->KeyHold(GLFW_KEY_Q))
				camera->TranslateUpword(-distance);

			if (window->KeyHold(GLFW_KEY_E))
				camera->TranslateUpword(distance);
		}

		if (window->KeyHold(GLFW_KEY_X) && matrix == "proj") {
			fov += deltaTime * cameraSpeed * 50.0f;
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
		}

		if (window->KeyHold(GLFW_KEY_Z) && matrix == "proj") {
			fov -= deltaTime * cameraSpeed * 50.0f;
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
		}
	}

	if (window->KeyHold(GLFW_KEY_2) && matrix == "proj") {
		projectionMatrix = glm::ortho(-width, width, -height, height, zNear, zFar);
		matrix = "orth";
	}

	if (window->KeyHold(GLFW_KEY_3) && matrix == "orth") {
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
		matrix = "proj";
	}

}

void Flappy::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE)
	{
		if (oneMoreSpace) {
			oneMoreSpace = false;
			resetWorld();
		}
		else if (!gameStart)
			gameStart = true;

		birdVelocity = -birdJumpHeight;
	}

	if (key == GLFW_KEY_1) {
		debug = !debug;
		cout << "DEBUG MODE: " << debug << endl;
	}
}

void Flappy::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

	if (debug && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
			camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
		}

	}
}
