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

### Progress Updates
Oct. 30, 2026: Multi-light lighting and Uniform Array support. Added some CMake presets and preprocessor macros for convenience and benchmarking.
<img width="800" height="600" alt="Minecraft_GCzau2A5TF" src="https://github.com/user-attachments/assets/9909a7b9-7012-4cee-99e0-ef9b76b9cc97" />

Oct. 27, 2026: Specular map and lighting. Numerous abstractions for Shaders, Textures, and Materials now exist. The squares, light, geometry, material, and camera are implemented as entities and components in the engine.


