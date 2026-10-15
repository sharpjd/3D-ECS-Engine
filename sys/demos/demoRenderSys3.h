#pragma once	

#include "libs/glad/glad.h" //MUST be included before glfw (glfw checks for it)
#include <GLFW/glfw3.h>

#include "system.h"
#include "registry.h"
#include "engineContext.h"
#include "loader.h"
#include "shader.h"
#include <cmath>

class DemoRenderSys3 : public ISystem {
private:
	const char* vertexShaderSource = nullptr; //const char* is still assignable for some reason. Also, GL only accepts const char*
	const char* fragShaderSource = nullptr;
public:

	Shader hello2Shader{ ShaderId::Hello, ec.shaderLoader };

	DemoRenderSys3(EngineContext& ec) : ISystem(ec)
	{

	}

	void start() override {

		

		float vertices[] = {
			// positions         // colors
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 2
		};

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
			6 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		glEnableVertexAttribArray(0); //enable location 0 in the vertex shader
		//in vec3 2
		glVertexAttribPointer(
			1, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			6 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(3 * sizeof(float)) //starting byte offset
		);
		glEnableVertexAttribArray(1); //enable location 1 in the vertex shader
	}

	unsigned int VBO1;
	unsigned int EBO1;
	unsigned int VAO1;
	void update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//uniform variables: https://learnopengl.com/Getting-started/Shaders
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(hello2Shader.ID, "ourColor");
		glUseProgram(hello2Shader.ID);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); //called after glUseProgram

		glBindVertexArray(VAO1); //bind VAO1
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL). //(already default) draw in fill mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //draw in wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //draw in point mode
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); //unbind VAO1


	}

};