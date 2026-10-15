#pragma once	

#include "glad.h" //MUST be included before glfw (glfw checks for it)
#include <GLFW/glfw3.h>

#include "system.h"
#include "registry.h"
#include "engineContext.h"
#include "input.h"
#include "loader.h"
#include "shader.h"
#include "stb_image.h"
#include "window.h"
#include "time.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "comps/transform.h"
#include "comps/camera.h"
#include <random>

class DemoCameraRenderingSys : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	Shader hello5Shader{ ShaderId::Hello5, ec.shaderLoader };

	DemoCameraRenderingSys(EngineContext& ec) : ISystem(ec)
	{

	}


	std::vector<glm::vec3> cubePositions;

	void start() override {

		// 1. Obtain a seed from the hardware
		//std::random_device rd;

		// 2. Initialize the standard Mersenne Twister engine with the seed
		//std::mt19937 gen(rd());
		std::mt19937 gen(67);

		// 3. Define the distribution range [lower_bound, upper_bound)
		float min_val = 1.0f;
		float max_val = 6.0f;
		std::uniform_real_distribution<float> dis(min_val, max_val);

		for (int i = 0; i < 100; i++) {
			float rx = dis(gen);
			float ry = dis(gen);
			float rz = dis(gen);

			cubePositions.push_back(glm::vec3(rx, ry, rz));
		}

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		stbi_set_flip_vertically_on_load(true);

		glGenVertexArrays(1, &VAO1);
		glGenBuffers(1, &VBO1);

		glBindVertexArray(VAO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1); //does NOT immediately save to VAO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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



		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		unsigned char* data = stbi_load("assets/images/jerma.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width2, height2, nrChannels2;
		unsigned char* data2 = stbi_load("assets/images/hank.png", &width2, &height2, &nrChannels2, 0);
		if (data2)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data2);

		glUniform1i(glGetUniformLocation(hello5Shader.ID, "texture1"), 0);
		glUniform1i(glGetUniformLocation(hello5Shader.ID, "texture2"), 1);
		u1f_alpha = glGetUniformLocation(hello5Shader.ID, "alpha");
		u4fv_model = glGetUniformLocation(hello5Shader.ID, "model");
		u4fv_view = glGetUniformLocation(hello5Shader.ID, "view");
		u4fv_projection = glGetUniformLocation(hello5Shader.ID, "projection");

		/*
		* IMPORTANT
		* Tell OpenGL which texture unit each uniform (sampler2d) corresponds to
		*/
		glUseProgram(hello5Shader.ID);

		
	}

	unsigned int u4fv_model;
	unsigned int u4fv_view;
	unsigned int u4fv_projection;
	unsigned int u1f_alpha;
	unsigned int texture1;
	unsigned int texture2;
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

		Query q = ec.reg.query<Camera>();
		assert(q.count() == 1);
		for (auto [eh, cam] : q) {

			float width = ec.window.WIDTH;
			float height = ec.window.HEIGHT;
			glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(hello5Shader.ID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, texture2);

			glUniformMatrix4fv(u4fv_view, 1, GL_FALSE, glm::value_ptr(cam.view));
			glUniformMatrix4fv(u4fv_projection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniform1f(u1f_alpha, alpha);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
			glEnable(GL_DEPTH_TEST);

			glBindVertexArray(VAO1); //bind VAO1
			for (unsigned int i = 0; i < cubePositions.size(); i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				glUniformMatrix4fv(u4fv_model, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0); //unbind VAO1
		}
	}

};