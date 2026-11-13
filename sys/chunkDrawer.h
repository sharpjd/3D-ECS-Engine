#pragma once

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
#include "mesh.h"
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
#include "comps/lights.h"
#include "comps/demoCube.h"
#include <numbers>
#include "comps/staticModel.h"

struct RotateAround {
	glm::vec3 origin{ 0.0f, 0.0f, 0.0f };
	float rotationTime = 5.0f;
	float radius = 5.0f;
};

class ChunkDrawer : public ISystem {
private:
	unsigned int VBO1;
	unsigned int VAO_cubes;
	unsigned int VAO_light;
public:

	MultiLightShader multiLightShader{ ec.shaders };
	LightShader lightShader{ ec.shaders };
	MCTexture containerTex{ TextureId::Container, ec.textures.texLoader, GL_TEXTURE0 };
	MCTexture containerSpecularTex{ TextureId::Container_Specular, ec.textures.texLoader, GL_TEXTURE1 };

	ChunkDrawer(EngineContext& ec) : ISystem(ec)
	{
	}

	void start() override {
#pragma region add some lights

		std::mt19937 gen(67);

		float min_val = -3.0f;
		float max_val = 3.0f;
		std::uniform_real_distribution<float> dis(min_val, max_val);

		for (int i = 0; i < 4; i++) {
			EntityHandle lightE = ec.reg.newEntity();
			ec.reg.addComponent(
				lightE,
				PointLight{
					1.0f,
					0.00009f, //0.09f,
					0.00032f,
					glm::vec3(0.2f, 0.2f, 0.2f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(1.0f, 1.0f, 1.0f),
				}
				);
			ec.reg.addComponent(lightE, Geometry{ Geometries::cube_texcoords.data(), Geometries::cube_texcoords.size() });
			Position pos;
			if (i == 0) pos = Position{ glm::vec3(0.7f, 0.2f, 2.0f) };
			if (i == 1) pos = Position{ glm::vec3(1.3f, -1.3f, -1.0f) };
			if (i == 2) pos = Position{ glm::vec3(-4.0f, 2.0f, -12.0f) };
			if (i == 3) pos = Position{ glm::vec3(0.0f, 0.0f, -3.0f) };

			RotateAround rot;
			rot.origin = pos.position;
			if (i == 0) rot.rotationTime = 13.0f;
			if (i == 1) rot.rotationTime = 10.0f;
			if (i == 2) rot.rotationTime = 5.0f;
			if (i == 3) rot.rotationTime = 7.0f;

			ec.reg.addComponent(lightE, pos);
			ec.reg.addComponent(lightE, rot);
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
			8 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)0 //starting byte offset
		);
		glEnableVertexAttribArray(0);
		//in vec3 2
		glVertexAttribPointer(
			1, //attribute location
			3, //components per attribute (e.g. 3 for vec3)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			8 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(3 * sizeof(float)) //starting byte offset
		);
		glEnableVertexAttribArray(1);
		//in vec2 2
		glVertexAttribPointer(
			2, //attribute location
			2, //components per attribute (e.g. 2 for vec2)
			GL_FLOAT, //component type
			GL_FALSE, //normalize values or not
			8 * sizeof(float), //bytes per stride (0 can be used in some cases ???)
			(void*)(6 * sizeof(float)) //starting byte offset
		);
		glEnableVertexAttribArray(2);



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
#pragma endregion

		ec.world.generate();


	}

	void update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
		glEnable(GL_DEPTH_TEST);

		float width = ec.window.WIDTH;
		float height = ec.window.HEIGHT;
		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

		Query pointLights = ec.reg.query<Position, Geometry, PointLight, RotateAround>();

		Query q = ec.reg.query<Camera>();
		assert(q.count() == 1);

#pragma region draw some lights

		for (auto [eh, cam] : q) {
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);
			glm::mat4 model = glm::mat4(1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, VBO1);

			lightShader.use();
			{
				glBindVertexArray(VAO_light);

				lightShader.view.set(cam.view);
				lightShader.projection.set(projection);

				for (auto [eh, pos, geo, pointLight, rot] : pointLights) {

					float pi = std::numbers::pi_v<float>;
					float circleAnimPos = 2 * pi * ec.time.time / rot.rotationTime;
					pos.position = glm::vec3(rot.radius * sin(circleAnimPos), 0.0f, rot.radius * cos(circleAnimPos));
					pos.position += rot.origin;
					//pos.position += glm::vec3(3.0f, 3.0f, 5.0f);

					model = glm::mat4(1.0f);
					model = glm::translate(model, pos.position);
					model = glm::scale(model, glm::vec3(0.2f));

					lightShader.model.set(model);

					glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STREAM_DRAW);

					glDrawArrays(GL_TRIANGLES, 0, 36);

				}
			}
		}
#pragma endregion

		Geometry geo{
			Geometries::cube_normals_texcoords.data(),
			Geometries::cube_normals_texcoords.size()
		};

		int i = 0;

		multiLightShader.use(); {
			for (auto [eh, cam] : q) {
				glm::mat4 projection;
				projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);
				glm::mat4 model = glm::mat4(1.0f);

				for (int cy = 0; cy < ec.world.yChunksMax; cy++) {
					for (int cz = 0; cz < ec.world.zChunksMax; cz++) {
						for (int cx = 0; cx < ec.world.xChunksMax; cx++) {

							Chunk& chunk = ec.world.chunkAt_ChunkXYZ(cx, cy, cz);
							BlockList& blockList = chunk.blockMaps[BlockType::DebugBlock];

							for (int by = 0; by < BPC; by++) {
								for (int bz = 0; bz < BPC; bz++) {
									for (int bx = 0; bx < BPC; bx++) {
										
										int blockListIndex = bx + bz*BPC + by*BPCSQ;
										if (blockList.data()[blockListIndex] == false) {
											continue;
										}

										int bxWorld = cx * BPC + bx;
										int byWorld = cy * BPC + by;
										int bzWorld = cz * BPC + bz;

										model = glm::mat4(1.0f); //reset the model matrix

										glBindVertexArray(VAO_cubes); //just for the lights

										glm::vec3 blockPos{
											static_cast<float>(bxWorld),
											static_cast<float>(byWorld),
											static_cast<float>(bzWorld)
										};
										model = glm::translate(model, blockPos);
										multiLightShader.model.set(model);

										multiLightShader.view.set(cam.view);
										multiLightShader.projection.set(projection);

										multiLightShader.viewPos.set(cam.cameraPos);

										multiLightShader.materialShininess.set(32.0f);

										containerTex.use();
										containerSpecularTex.use();

										int i1 = 0;
										for (auto [eh1, pos1, geo1, pointLight1, rot] : pointLights) {
											assert(i1 < 4);
											multiLightShader.pointLightPosition[i1].set(pos1.position);

											multiLightShader.pointLightConstant[i1].set(pointLight1.constant);
											multiLightShader.pointLightLinear[i1].set(pointLight1.linear);
											multiLightShader.pointLightQuadratic[i1].set(pointLight1.quadratic);

											multiLightShader.pointLightAmbient[i1].set(pointLight1.ambient);
											multiLightShader.pointLightDiffuse[i1].set(pointLight1.diffuse);
											multiLightShader.pointLightSpecular[i1].set(pointLight1.specular);

											i1++;
										}

										//TODO: we need to batch things with the same geometry so we dont call this every frame
										glBufferData(GL_ARRAY_BUFFER, geo.arraySize * 4, geo.vertices, GL_STREAM_DRAW);

										glUniform1f(1, 1.0f);

										glDrawArrays(GL_TRIANGLES, 0, 36);

										i++; //to track how many times this is executing
									}
								}
							}
						}
					}
				}
			}
		}

		auto breakpoint = "breakpoint";
	}

	void onDestroy() override {

	}
};
