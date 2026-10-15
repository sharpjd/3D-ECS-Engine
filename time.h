#pragma once 

#include <GLFW/glfw3.h>
#include <cassert>

class Time {
private:
	float startTime_ = 0.0f;
	float deltaTime_ = 0.0f;
	float timeOfLastFrame_ = 0.0f;

	bool startTimeRecorded = false;
public:

	void update() {
		assert(startTimeRecorded);
		float timeNow = glfwGetTime();
		deltaTime_ = timeNow - timeOfLastFrame_;
		timeOfLastFrame_ = timeNow;
	}

	void recordStart() {
		startTime_ = glfwGetTime();
		startTimeRecorded = true;
	}

	const float& startTime = startTime_;
	const float& deltaTime = deltaTime_;

};