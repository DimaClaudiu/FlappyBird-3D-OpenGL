#pragma once
#include <include/glm.h>
#include <include/math.h>

namespace MyFlappy
{
	class Camera
	{
	public:
		Camera()
		{
			position = glm::vec3(0, 2, 5);
			forward = glm::vec3(0, 0, -1);
			up = glm::vec3(0, 1, 0);
			right = glm::vec3(1, 0, 0);
			distanceToTarget = 2;
		}

		Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
		{
			Set(position, center, up);
		}

		~Camera()
		{ }

		// Update camera
		void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
		{
			this->position = position;
			forward = glm::normalize(center - position);
			right = glm::cross(forward, up);
			this->up = glm::cross(right, forward);
		}

		void MoveForward(float distance)
		{
			glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
			position = position + dir * distance;
			// movement will keep the camera at the same height always
			// Example: If you rotate up/down your head and walk forward you will still keep the same relative distance (height) to the ground!
			// Translate the camera using the DIR vector computed from forward
		}

		void TranslateForward(float distance)
		{
			// Translate the camera using the "forward" vector
			this->position += forward * distance;
		}

		void TranslateUpword(float distance)
		{
			// Translate the camera using the up vector
			this->position += glm::vec3(0, 1, 0) * distance;
		}

		void TranslateRight(float distance)
		{
			// Translate the camera using the "right" vector
			// Usually translation using camera "right' is not very useful because if the camera is rotated around the "forward" vector 
			// translation over the right direction will have an undesired effect; the camera will get closer or farther from the ground
			// Using the projected right vector (onto the ground plane) makes more sense because we will keep the same distance from the ground plane
			this->position += right * distance;
		}

		void RotateFirstPerson_OX(float angle)
		{
			// Compute the new "forward" and "up" vectors
			glm::vec4 newForward = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
			forward = glm::normalize(glm::vec3(newForward));
			glm::vec4 newUp = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
			up = glm::normalize(glm::vec3(newUp));
		}

		void RotateFirstPerson_OY(float angle)
		{
			// Compute the new "forward", "up" and "right" vectors
			glm::vec4 newForward = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
			forward = glm::normalize(glm::vec3(newForward));
			glm::vec4 newUp = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(up, 1);
			up = glm::normalize(glm::vec3(newUp));
			glm::vec4 newRight = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
			right = glm::normalize(glm::vec3(newRight));
		}

		void RotateFirstPerson_OZ(float angle)
		{
			// Compute the new Right and Up, Forward stays the same
			glm::vec4 newUp = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(up, 1);
			up = glm::normalize(glm::vec3(newUp));
			glm::vec4 newRight = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(right, 1);
			right = glm::normalize(glm::vec3(newRight));
		}

		void RotateThirdPerson_OX(float angle)
		{
			// Rotate the camera in Third Person mode - OX axis
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OX(angle);
			TranslateForward(-distanceToTarget);

		}

		void RotateThirdPerson_OY(float angle)
		{
			// Rotate the camera in Third Person mode - OY axis
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OY(angle);
			TranslateForward(-distanceToTarget);
		}

		void RotateThirdPerson_OZ(float angle)
		{
			// Rotate the camera in Third Person mode - OZ axis
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OZ(angle);
			TranslateForward(-distanceToTarget);
		}

		glm::mat4 GetViewMatrix()
		{
			// Returns the View Matrix
			return glm::lookAt(position, position + forward, up);
		}

		glm::vec3 GetTargetPosition()
		{
			return position + forward * distanceToTarget;
		}

	public:
		float distanceToTarget;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	};
}