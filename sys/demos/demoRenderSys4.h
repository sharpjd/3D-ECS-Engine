#pragma once	

#include "glad.h" //MUST be included before glfw (glfw checks for it)
#include <GLFW/glfw3.h>

#include "system.h"
#include "registry.h"
#include "engineContext.h"
#include "loader.h"
#include "shader.h"
#include "stb_image.h"
#include <cmath>

class DemoRenderSys4 : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	Shader hello3Shader{ ShaderId::Hello3, ec.shaderLoader };

	DemoRenderSys4(EngineContext& ec) : ISystem(ec)
	{

	}

	void start() override {

		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
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



		glGenVertexArrays(1, &VAO1);
		glBindVertexArray(VAO1);

		// EBO binding is stored directly in the VAO.
		// VBO is associated with the VAO when glVertexAttribPointer is called.

		glGenBuffers(1, &EBO1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1); //immediately saved to VAO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glGenBuffers(1, &VBO1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1); //does NOT immediately save to VAO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		//this call actually saves the VBO to the VAO
		//in vec3 1
		glVertexAttribPointer(
			0, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			8 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		glEnableVertexAttribArray(0); //enable location 0 in the vertex shader
		//in vec3 2
		glVertexAttribPointer(
			1, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			8 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(3 * sizeof(float)) //starting byte offset
		);
		glEnableVertexAttribArray(1); //enable location 1 in the vertex shader
		//in vec2 2
		glVertexAttribPointer(
			2, 
			2, 
			GL_FLOAT, 
			GL_FALSE, 
			8 * sizeof(float), 
			(void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	unsigned int texture;
	unsigned int VBO1;
	unsigned int EBO1;
	unsigned int VAO1;
	void update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//uniform variables: https://learnopengl.com/Getting-started/Shaders
		int uniformSampler2DLoc = glGetUniformLocation(hello3Shader.ID, "ourTexture"); //not actually required
		glUseProgram(hello3Shader.ID);
		glActiveTexture(GL_TEXTURE0); //this is not actually required, it's active by default
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO1); //bind VAO1
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); //unbind VAO1


	}

};