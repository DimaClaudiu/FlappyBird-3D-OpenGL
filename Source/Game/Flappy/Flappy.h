#pragma once
#include <Component/SimpleScene.h>
#include "Camera.h"

class Flappy : public SimpleScene
{
public:
	Flappy();
	~Flappy();

	void Init() override;
	int currentTexture = 0;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	double Distance(glm::vec2 p1, glm::vec2 p2);
	void resetWorld();

protected:
	MyFlappy::Camera* camera;
	glm::mat4 projectionMatrix;
	float fov = 60;
	float height = 3.0f, width = 5.0f;
	std::string matrix = "proj";
	float zNear = 0.01f, zFar = 200.0f;

	bool debug = false;
	float rotation;
	double birdVelocity;
	double acceleration;
	double pipeSpeed;
	double pipeAcceleration;
	int score;

	bool gameStart = false;
	bool oneMoreSpace = false;
	int lastPipe = -1;

	double pipeWidth = 0.6;
	double pipeHeight = 8 * pipeWidth;
	double pipeDepth = pipeWidth / 2;
	double pipeDistance = 5 * pipeWidth;
	float pipeRotation = 0;
	double gapSize = 3.5 * pipeWidth;

	double pipeVerticalVelocity = 0;
	int pipeVerticalFactor = 1;
	double pipeHorizontalVelocity = 0;
	int pipeHorizontalFactor = 1;

	int nOfPipes = 6;
	glm::vec2 pipePos[6];
	double initialYOffset = 6 * pipeWidth;
	double initialXOffset = 5 * pipeWidth;
	double middleX = (initialXOffset - pipeWidth) * pipeDistance;

	float skyboxRotation = 0;
	int skyboxFactor = 1;

	double birdWidth = pipeWidth;
	double birdJumpHeight = 8 * pipeWidth;
	double birdVerticalVelocity = 1;
	int birdVerticalFactor = 1;
	glm::vec2 birdPos;

	int textureIndex = 0;



};
