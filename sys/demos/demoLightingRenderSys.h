#pragma once	

#include "glad.h" //MUST be included before glfw (glfw checks for it)
#include <GLFW/glfw3.h>

#include "system.h"
#include "registry.h"
#include "engineContext.h"
#include "input.h"
#include "loader.h"
#include "shaders.h"
#include "stb_image.h"
#include "window.h"
#include "time.h"
#include "geometries.h"
#include "comps/texture.h"
#include "comps/geometry.h"
#include "comps/transform.h"
#include "mcTexture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "comps/transform.h"
#include "comps/camera.h"
#include <random>
#include "comps/light.h"
#include "comps/demoCube.h"

class DemoLightingRenderSys : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	CubeLightingShader cubeLightingShader{ ec.shaders };
	LightShader lightShader{ ec.shaders };

	DemoLightingRenderSys(EngineContext& ec) : ISystem(ec)
	{

	}

	void start() override {

		auto& vertices = Geometries::cube;

		// 1. Obtain a seed from the hardware
		//std::random_device rd;

		// 2. Initialize the standard Mersenne Twister engine with the seed
		//std::mt19937 gen(rd());
		std::mt19937 gen(67);

		// 3. Define the distribution range [lower_bound, upper_bound)
		float min_val = 1.0f;
		float max_val = 6.0f;
		std::uniform_real_distribution<float> dis(min_val, max_val);

		auto lightE = ec.reg.newEntity();
		ec.reg.addComponent(lightE, Light{});
		ec.reg.addComponent(lightE, Geometry{ Geometries::cube.data(), Geometries::cube.size() });
		ec.reg.addComponent(lightE, Position{ glm::vec3{1.2f, 1.0f, -2.0f} });

		for (int i = 0; i < 2000; i++) {
			float rx = dis(gen);
			float ry = dis(gen);
			float rz = dis(gen);

			auto cubeE = ec.reg.newEntity();

			auto geometryComp = Geometry{ Geometries::cube.data(), Geometries::cube.size() };
			auto position = Position{ glm::vec3(rx, ry, rz) };
			auto demoCube = DemoCube{};
			ec.reg.addComponent(cubeE, geometryComp);
			ec.reg.addComponent(cubeE, position);
			ec.reg.addComponent(cubeE, demoCube);
		}

		glGenBuffers(1, &VBO1); //we will bind this to TWO VAOs

		glGenVertexArrays(1, &VAO_cubes);
		glBindVertexArray(VAO_cubes);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1); //does NOT immediately save to VAO

		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			5 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		//this call actually saves the VBO to the VAO
		glEnableVertexAttribArray(0); //enable location 0 in the vertex shader



		glGenVertexArrays(1, &VAO_light);
		glBindVertexArray(VAO_light);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1); //does NOT immediately save to VAO
		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			5 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		//this call actually saves the VBO to the VAO
		glEnableVertexAttribArray(0); //enable location 0 in the vertex shader

	}

	unsigned int VBO1;

	//you can bind one VBO to multiple VAOs
	unsigned int VAO_cubes;
	unsigned int VAO_light;

	float alpha = 0.0f;

	void update() override {

		float width = ec.window.WIDTH;
		float height = ec.window.HEIGHT;
		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

		Query q = ec.reg.query<Camera>();
		//TODO we don't need to query every single frame. Gonna need some sorta query caching system
		Query q2 = ec.reg.query<Position, Geometry, DemoCube>();
		Query lights = ec.reg.query<Position, Geometry, Light>();
		assert(q.count() == 1);
		for (auto [eh, cam] : q) {

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
			glEnable(GL_DEPTH_TEST);

			glm::mat4 projection;
			projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);
			glm::mat4 model = glm::mat4(1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, VBO1);

			lightShader.use();
			{
				glBindVertexArray(VAO_light);

				lightShader.view.set(cam.view);
				lightShader.projection.set(projection);

				for (auto [eh, pos, geo, light] : lights) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, pos.position);
					model = glm::scale(model, glm::vec3(0.2f));

					glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STATIC_DRAW);

					lightShader.model.set(model);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			
			cubeLightingShader.use();
			{
				glBindVertexArray(VAO_cubes);

				cubeLightingShader.view.set(cam.view);
				cubeLightingShader.projection.set(projection);
				int i = 0;
				for (auto [eh, pos, geo, demoCube] : q2)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, pos.position);
					float angle = 20.0f * i;
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

					glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STATIC_DRAW);

					cubeLightingShader.model.set(model);

					auto objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
					auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
					cubeLightingShader.objectColor.set(objectColor);
					cubeLightingShader.lightColor.set(lightColor);

					glDrawArrays(GL_TRIANGLES, 0, 36);
					i++;
				}
			}
			
		}
	}

};