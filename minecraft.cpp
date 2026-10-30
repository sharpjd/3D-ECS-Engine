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

#include "demoMultiLight.h"

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
		ec(EngineContext{ reg, orch, input, shaders, textures, window, time }),
		window(&glfwWindow),
		input(&glfwWindow)
	{	
	}

private:

	GLFWwindow* glfwWindow;

	VertexShaderLoader vertexShaderLoader;
	FragShaderLoader fragShaderLoader;
	ShaderManager shaders{ vertexShaderLoader, fragShaderLoader };

	TextureLoader texLoader;
	TextureManager textures{ texLoader };

	Time time;
	Window window;
	Input input;
	Registry reg;
	Orchestrator orch;

	EngineContext ec;

	void initEngine() {
		window.init();

		textures.loadAll(); //for now we just load all textures
	}

#if _DEBUG
	float startDelaySecs = 5.0f;
	int sampleMax = 10000;
	int samples = 0;
	uint64_t sum = 0; //Eighteen quintillion, four hundred forty-six quadrillion, seven hundred forty-four trillion, seventy-three billion, seven hundred nine million, five hundred fifty-one thousand, six hundred fifteen.
	std::vector<int> points;
	void mainLoop() {

		input.update();
		auto start = std::chrono::steady_clock::now();
		orch.update();
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		sum += duration;
		points.push_back(duration);
		window.update();
		time.update(); //comes last, gives deltatime

		if (time.time - time.startTime < startDelaySecs) {
			samples = 0;
			sum = 0;
			points.clear();
		}

		samples++;
		if (samples == sampleMax) {
			long double avg = static_cast<long double>(sum) / static_cast<long double>(sampleMax);

			long double standardDeviation = 0;
			long double numerator = 0;
			for (int num : points) {
				numerator += std::pow((static_cast<double>(num) - avg), 2);
			}
			long double denominator = static_cast<double>(sampleMax);

			standardDeviation = std::sqrt(numerator / denominator);

			auto _ = 1;
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
#else
	void mainLoop() {
		input.update();
		orch.update();
		window.update();
		time.update(); 
	}
#endif

	void initGame() { //for now, systems are run in the order they're added

		std::shared_ptr<CameraSys> cam(new CameraSys(ec));
		orch.addSystem(cam);

		std::shared_ptr<ISystem> s(new DemoMultiLight(ec));
		orch.addSystem(s);
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