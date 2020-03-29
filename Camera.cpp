//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//
#include "stdafx.h"
#include "Camera.hpp"

namespace gps {

	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
	{
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	glm::mat4 Camera::getViewMatrix()
	{
		return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return cameraPosition;
	}

	void Camera::setPosition(float x, float y, float z) {
		this->cameraPosition = glm::vec3(x,y,z);
	}

	void Camera::setTarget(float x, float y, float z) {
		this->cameraTarget = glm::vec3(x, y, z);
	}

	void Camera::move(MOVE_DIRECTION direction, float speed)
	{
		switch (direction) {
		case MOVE_FORWARD:
			cameraPosition += cameraDirection * speed;
			break;

		case MOVE_BACKWARD:
			cameraPosition -= cameraDirection * speed;
			break;

		case MOVE_RIGHT:
			cameraPosition += cameraRightDirection * speed;
			break;

		case MOVE_LEFT:
			cameraPosition -= cameraRightDirection * speed;
			break;
		}
	}

	void Camera::rotate(float pitch, float yaw)
	{
		glm::vec3 cameraDirectionAux;
		cameraDirectionAux.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirectionAux.y = sin(glm::radians(pitch));
		cameraDirectionAux.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(cameraDirectionAux);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

}
