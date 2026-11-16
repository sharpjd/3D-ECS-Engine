## Progess Update (Nov. 12, 2026):
### - Chunk Generation
 - The world is split into 16x16x16-block chunks. In the sample scene, the world was set to be 3x5x3 chunks (xyz). The Chunk Generator used simple noise to create "terrain variations".
 - **A naive approach (which I did not use):** Let's say we store every single block in the world as 4 integers each (one for each xyz, one for the block type) inside a vector. Let's say we have a render radius of 16 chunks with each chunk being 256 blocks tall like we're playing Minecraft. This would result in up to 67,108,864 items. That's already 256GB of memory used. Obviously, we need to get creative.
 - **My approach:** Chunks contain flattened 3D array bitmaps (std::vector<bool>) to represent each block type — just 4096 bits or 0.5KB per block type per chunk. Substituting this into our previous scenario, we only need 8MB of memory for each block type within the entire render distance. I realize this is colloquially a "Flyweight" design pattern.
  - There is of course plenty of room for further optimization. For example, for each chunk, if a block type is not present, we don't need to store the bitmap for it at all. Also, what if we had 1000 block types? We'd need up to 8GB of memory for that; there are plenty of voxel game data optimization we could search up.
 - (example GIF) (warning, might be slow to load):
<img width="800" height="600" alt="Minecraft_tyEbbhbhaT-ezgif com-optimize" src="https://github.com/user-attachments/assets/6bc9425c-8028-4da2-8830-d64209f50dd8" />

### - Model loading
 - I followed the Assimp loading tutorial from LearnOpenGL and made it compatible with my engine.
  - I substituted the file loading system with my own. There were numerous loaded data formats that had to be converted to be compatible with  my ECS.
 - (example GIF) (warning, might be slow to load):
<img width="800" height="600" alt="Minecraft_r8Qq4NSQlW-ezgif com-optimize" src="https://github.com/user-attachments/assets/03f48a46-152f-43c1-abf7-45d4d38c9f84" />

## Entity Component System
This is an ECS game engine written in C++20 with no external ECS library. Components are stored in **sparse sets**, with one densely packed array per component type. Iteration is cache-friendly, and adding, removing or looking up a component is O(1). Component types are assigned IDs at compile time through templates.

Queries are done using variadic templates and Iterators, for example `reg.query<Transform, Mesh>()`. They return an `Iterator` you can use in a range-based for-loop. A query iterates the *smallest* of the requested component pools and checks membership in the others, so its cost scales with the rarest component rather than the total entity count. When Entities/Components are "removed", deletion is deferred. Anything removed mid-frame is marked "dead", skipped by the iterator, and removed in a single `flush()` at the end of the frame. This makes it safe to destroy entities while iterating.

Entities are generational handles. Each entity index carries a generation counter, so a stale handle to a destroyed entity is detected instead of silently aliasing whatever reuses its slot. 

Logic lives in **systems**: classes with `start()`, `update()` and `onDestroy()` hooks. Systems like camera control and rendering are all implemented as systems that read plain-data components.

Example psuedocode:
```cpp
//Component: Camera
struct Camera {
  float fov = 45.0f;
  vec3 cameraDirection;
  vec3 cameraPos;
}

//System: RenderSystem
class RenderSystem : public ISystem{

  void start() override {

    Camera cameraComp{...};
    EntityHandle handle = ecs.newEntity();
    ecs.addComponent(handle, cameraComp);

  }

  void update() override {

    Query q = ecs.query<Camera>();
    for (auto [entityHandle, camera] : q) {
      glm::mat4 projection;
      projection = glm::perspective(camera.fov, camera.cameraDirection, ...);
      glUseProgram(myShader);
      ...
    }

  }

}
```

### Future Plans / Disclaimers
I am planning to turn this project into a Minecraft clone as it's something I'm familiar with and offers me with a sandbox-y environment to explore programming ideas such as more advanced lighting and shader effects. I am currently sticking to OpenGL as it allows me to focus on concepts without the deep plumbing requirements of Vulkan.

The ECS data structures / logic were written without any AI-generated code. I aim to responsibly use AI mainly as a learning tool as game engines and graphics programming necessitate independent thinking / deep understanding of underlying systems and hardware. 

### Older Progress Updates

Oct. 30, 2026: Multi-light lighting and Uniform Array support. Added some CMake presets and preprocessor macros for convenience and benchmarking.
<img width="800" height="600" alt="Minecraft_GCzau2A5TF" src="https://github.com/user-attachments/assets/9909a7b9-7012-4cee-99e0-ef9b76b9cc97" />

Oct. 27, 2026: Specular map and lighting. Numerous abstractions for Shaders, Textures, and Materials now exist. The squares, light, geometry, material, and camera are implemented as entities and components in the engine.


