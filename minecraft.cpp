#include <iostream>

#include "libs/glad/glad.h" //MUST be included before glfw (glfw checks for it)

#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32 //why. WHY DOES ORDER MATTER
//#include <GLFW/glfw3native.h>

#include "query.h"

#include "orchestrator.h"

#include "demoSys.h"
#include "system.h"
#include "window.h"
#include "input.h"
#include "time.h"

#include <thread>
#include <chrono>
#include "sys/cameraSys.h"

#include "demoCameraRenderingSys.h"

class Registry;

template <typename... Components>
class Query;

class Minecraft {

public:

	void run() {

		Debug::log("Start");

		initEngine();

		initGame();

		time.recordStart();

		while (!orch.hasEnded()) {
			if (window.shouldEnd) {
				orch.end();
			}
			mainLoop();
		}

		Debug::log("game loop ended, exiting");
	}

	Minecraft() 
		: orch(reg),
		ec(EngineContext{ reg, orch, input, shaderLoader, window, time }),
		window(&glfwWindow),
		input(&glfwWindow)
	{	
	}

private:

	GLFWwindow* glfwWindow;
	VertexShaderLoader vertexShaderLoader;
	FragShaderLoader fragShaderLoader;

	Time time;
	Window window;
	Input input;
	Registry reg;
	Orchestrator orch;
	ShaderFileLoader shaderLoader{ vertexShaderLoader, fragShaderLoader };
	EngineContext ec;

	void initEngine() {
		window.init();
	}

	void mainLoop() {

		input.update();
		window.update();
		orch.update();
		time.update(); //comes last, gives deltatime

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void initGame() { //for now, systems are run in the order they're added

		std::shared_ptr<CameraSys> cam(new CameraSys(ec));
		orch.addSystem(cam);

		std::shared_ptr<ISystem> s8(new DemoCameraRenderingSys(ec));
		orch.addSystem(s8);
	}

};

//yes, this needs to be at the end
int main() {
	Minecraft app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}