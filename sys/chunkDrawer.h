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
#include <glm/gtc/matrix_inverse.hpp>
#include <cmath>

#include "comps/transform.h"
#include "comps/camera.h"
#include "comps/material.h"
#include <random>
#include "comps/lights.h"
#include "comps/demoCube.h"
#include <numbers>
#include "comps/staticModel.h"

#include "chunkMesher.h"

class ChunkDrawer : public ISystem {
private:
public:

	BlockShader blockShader{ ec.shaders };

	std::vector<ChunkMesher> chunkMeshers;

	ChunkDrawer(EngineContext& ec) : ISystem(ec)
	{
	}

	void start() override {

		ec.world.generate();

		for (auto chunk : ec.world.chunks) {
			chunkMeshers.push_back(
				ChunkMesher{*chunk, ec.world}
			);
		}

		for (auto& chunkMesher : chunkMeshers) {
			chunkMesher.init();
		}

	}

	void update() override {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //must be cleared per frame
		glEnable(GL_DEPTH_TEST);

		float width = ec.window.WIDTH;
		float height = ec.window.HEIGHT;
		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

		Query q = ec.reg.query<Camera>();
		assert(q.count() == 1);

		int i = 0;

		blockShader.use(); {

			for (auto [eh, cam] : q) {
				glm::mat4 projection;
				projection = glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 10000.0f);
				glm::mat4 model = glm::mat4(1.0f);

				blockShader.view.set(cam.view);
				blockShader.projection.set(projection);
				blockShader.direction.set(glm::vec3(-0.2f, -1.0f, -0.3f));

				for (auto chunkMesher : chunkMeshers) {
					auto chunkCornerPos = ec.world.chunkBaseCorner(&(chunkMesher.chunk()));
					blockShader.chunkWorldPos.set(chunkCornerPos);

					blockShader.faceCounts[0].set(chunkMesher.rightFaces());
					blockShader.faceCounts[1].set(chunkMesher.leftFaces());
					blockShader.faceCounts[2].set(chunkMesher.topFaces());
					blockShader.faceCounts[3].set(chunkMesher.bottomFaces());
					blockShader.faceCounts[4].set(chunkMesher.frontFaces());
					blockShader.faceCounts[5].set(chunkMesher.backFaces());

					glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunkMesher.SSBO());
					glBufferData(GL_SHADER_STORAGE_BUFFER, 
						chunkMesher.ssboVertices().size() * sizeof(uint32_t), 
						chunkMesher.ssboVertices().data(), GL_DYNAMIC_DRAW);
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunkMesher.SSBO());

					blockShader.model.set(model);
					blockShader.transposeInverseModel.set(glm::inverseTranspose(model));

					glBindVertexArray(chunkMesher.VAO());
					glDrawArrays(GL_TRIANGLES, 0, chunkMesher.ssboVertices().size() * 6);
				}
			}
		}

		auto breakpoint = "breakpoint";
	}

	void onDestroy() override {

	}
};
