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
#include "comps/material.h"
#include <random>
#include "comps/light.h"
#include "comps/demoCube.h"
#include <numbers>

class DemoLightingRenderSys2 : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	//static constexpr glm::vec3 lightPos = glm::vec3{ 1.2f, 1.0f, -2.0f };
	//static constexpr glm::vec3 lightPos = glm::vec3{ 0.0f, 0.0f, 0.0f };

	PhongShader phongShader{ ec.shaders };
	LightShader lightShader{ ec.shaders };

	DemoLightingRenderSys2(EngineContext& ec) : ISystem(ec)
	{

	}

	void start() override {

		auto& vertices = Geometries::cube_texcoords;

		// 1. Obtain a seed from the hardware
		//std::random_device rd;

		// 2. Initialize the standard Mersenne Twister engine with the seed
		//std::mt19937 gen(rd());
		std::mt19937 gen(67);

		// 3. Define the distribution range [lower_bound, upper_bound)
		float min_val = -3.0f;
		float max_val = 3.0f;
		std::uniform_real_distribution<float> dis(min_val, max_val);

		auto lightE = ec.reg.newEntity();
		ec.reg.addComponent(
			lightE,
			Light{
				glm::vec3(0.2f, 0.2f, 0.2f),
				glm::vec3(0.5f, 0.5f, 0.5f),
				glm::vec3(1.0f, 1.0f, 1.0f),
			}
		);
		ec.reg.addComponent(lightE, Geometry{ Geometries::cube_texcoords.data(), Geometries::cube_texcoords.size() });
		ec.reg.addComponent(lightE, Position{});

		for (int i = 0; i < 10; i++) {
			float rx = dis(gen);
			float ry = dis(gen);
			float rz = dis(gen);

			auto cubeE = ec.reg.newEntity();

			auto geometryComp = Geometry{ Geometries::cube.data(), Geometries::cube.size() };
			auto position = Position{ glm::vec3(rx, ry, rz) };
			auto material = PhongMaterial{
				glm::vec3(1.0f, 0.5f, 0.31f),
				glm::vec3(1.0f, 0.5f, 0.31f),
				glm::vec3(0.5f, 0.5f, 0.5f),
				32.0f
			};
			auto demoCube = DemoCube{};
			ec.reg.addComponent(cubeE, geometryComp);
			ec.reg.addComponent(cubeE, position);
			ec.reg.addComponent(cubeE, material);
			ec.reg.addComponent(cubeE, demoCube);
		}

		glGenBuffers(1, &VBO1); //we will bind this to TWO VAOs

		glGenVertexArrays(1, &VAO_cubes);
		glBindVertexArray(VAO_cubes);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);

		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			6 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		glEnableVertexAttribArray(0);
		//in vec3 2
		glVertexAttribPointer(
			1, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			6 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(3 * sizeof(float)) //starting byte offset
		);
		glEnableVertexAttribArray(1);



		glGenVertexArrays(1, &VAO_light);
		glBindVertexArray(VAO_light);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			5 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(0) //starting byte offset
		);
		glEnableVertexAttribArray(0);

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
		assert(q.count() == 1);
		//TODO we don't need to query every single frame. Gonna need some sorta query caching system
		Query q2 = ec.reg.query<Position, Geometry, PhongMaterial,DemoCube>();

		Query lights = ec.reg.query<Position, Geometry, Light>();
		assert(lights.count() == 1);

		for (auto [eh, cam] : q) {

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
			glEnable(GL_DEPTH_TEST);

			glm::mat4 projection;
			projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);
			glm::mat4 model = glm::mat4(1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, VBO1);

			Light light_copy;
			Position light_pos_copy;

			lightShader.use();
			{
				glBindVertexArray(VAO_light);

				lightShader.view.set(cam.view);
				lightShader.projection.set(projection);

				for (auto [eh, pos, geo, light] : lights) {

					light_copy = light;
					light_pos_copy = pos;

					float pi = std::numbers::pi_v<float>;
					float circleAnimPos = 2 * pi * 0.125f * ec.time.time;
					pos.position = glm::vec3(5.0f*sin(circleAnimPos), 0.0f, 5.0f * cos(circleAnimPos));
					//pos.position += glm::vec3(3.0f, 3.0f, 5.0f);

					glm::vec3 lightColor;
					lightColor.x = sin(glfwGetTime() * 2.0f);
					lightColor.y = sin(glfwGetTime() * 0.7f);
					lightColor.z = sin(glfwGetTime() * 1.3f);
					glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
					glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
					light.diffuse = diffuseColor;
					light.ambient = ambientColor;

					model = glm::mat4(1.0f);
					model = glm::translate(model, pos.position);
					model = glm::scale(model, glm::vec3(0.2f));

					lightShader.model.set(model);
					
					glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STREAM_DRAW);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}

			phongShader.use();
			{
				glBindVertexArray(VAO_cubes);

				phongShader.view.set(cam.view);
				phongShader.projection.set(projection);

				phongShader.viewPos.set(cam.cameraPos);

				int i = 0;
				for (auto [eh, pos, geo, mat, demoCube] : q2)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, pos.position);
					float angle = 20.0f * i;
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

					phongShader.model.set(model);

					phongShader.materialAmbient.set(mat.ambient);
					phongShader.materialDiffuse.set(mat.diffuse);
					phongShader.materialSpecular.set(mat.specular);
					phongShader.materialShininess.set(mat.shininess);

					phongShader.lightPosition.set(light_pos_copy.position);
					phongShader.lightAmbient.set(light_copy.ambient);
					phongShader.lightDiffuse.set(light_copy.diffuse);
					phongShader.lightSpecular.set(light_copy.specular);

					//TODO: we need to batch things with the same geometry so we dont call this every frame
					glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STREAM_DRAW);

					glDrawArrays(GL_TRIANGLES, 0, 36);
					i++;
				}
			}
			
		}
	}

};