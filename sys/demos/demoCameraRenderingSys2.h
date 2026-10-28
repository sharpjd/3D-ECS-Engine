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

class DemoCameraRenderingSys2 : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	Hello5Shader hello5Shader{ ec.shaders };
	MCTexture jermaTexture{ TextureId::Jerma, ec.textures.texLoader };
	MCTexture hankTexture{ TextureId::Hank, ec.textures.texLoader };

	DemoCameraRenderingSys2(EngineContext& ec) : ISystem(ec)
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

		for (int i = 0; i < 10; i++) {
			float rx = dis(gen);
			float ry = dis(gen);
			float rz = dis(gen);

			auto entity = ec.reg.newEntity();

			auto texComp = Texture{ jermaTexture.gl_textureId };
			auto texComp2 = DemoSecondTexture{ hankTexture.gl_textureId };
			auto geometryComp = Geometry{ Geometries::cube.data(), Geometries::cube.size() };
			auto position = Position{ glm::vec3(rx, ry, rz) };
			ec.reg.addComponent(entity, texComp);
			ec.reg.addComponent(entity, texComp2);
			ec.reg.addComponent(entity, geometryComp);
			ec.reg.addComponent(entity, position);
		}

		glGenVertexArrays(1, &VAO1);
		glGenBuffers(1, &VBO1);


		glBindVertexArray(VAO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1); //does NOT immediately save to VAO

		//this call actually saves the VBO to the VAO
		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			5 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		glEnableVertexAttribArray(0); //enable location 0 in the vertex shader
		//in vec2 2
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			5 * sizeof(float),
			(void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);

	}

	unsigned int VBO1;
	unsigned int EBO1;
	unsigned int VAO1;

	float alpha = 0.0f;

	void update() override {

		//if (ec.input.w) {
		//	alpha += 0.0001;
		//}
		//if (ec.input.s) {
		//	alpha -= 0.0001;
		//}
		//if (alpha < 0.0f) alpha = 0.0f;
		//if (alpha > 1.0f) alpha = 1.0f;

		float width = ec.window.WIDTH;
		float height = ec.window.HEIGHT;
		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

		Query q = ec.reg.query<Camera>();
		//TODO we don't need to query every single frame. Gonna need some sorta query caching system
		Query q2 = ec.reg.query<Position, Geometry, Texture, DemoSecondTexture>();
		assert(q.count() == 1);
		for (auto [eh, cam] : q) {

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
			glEnable(GL_DEPTH_TEST);

			hello5Shader.use();

			glm::mat4 projection;
			projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);

			hello5Shader.view.set(cam.view);
			hello5Shader.projection.set(projection);
			hello5Shader.alpha.set(alpha);

			glBindBuffer(GL_ARRAY_BUFFER, VBO1); //this can go before or after VAO1 bind (you can figure out why)
			glBindVertexArray(VAO1); //bind VAO1

			/*
			* bind outside loop (only 1 time)
			*/
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, jermaTexture.gl_textureId);
			//glActiveTexture(GL_TEXTURE0 + 1);
			//glBindTexture(GL_TEXTURE_2D, hankTexture.gl_textureId);

			//glBufferData(GL_ARRAY_BUFFER, Geometries::cube.size() * 4, Geometries::cube.data(), GL_STATIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_DYNAMIC_DRAW); //dynamic is faster if geo changes a lot

			int i = 0;
			for (auto [eh, pos, geo, tex, tex2] : q2)
			{
				//bind inside loop (100,000 times)
				//TODO: we definitely don't need to bind every time
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex.gl_textureId);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, tex2.gl_textureId);

				glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STATIC_DRAW);
				//glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_DYNAMIC_DRAW); //dynamic is faster if geo changes a lot

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, pos.position);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				hello5Shader.model.set(model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
				i++;
			}
			glBindVertexArray(0); //unbind VAO1
		}
	}

};