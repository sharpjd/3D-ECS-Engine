## Entity Component System
This is an ECS game engine written in C++20 with no external ECS library. Components are stored in **sparse sets**, with one densely packed array per component type. Iteration is cache-friendly, and adding, removing or looking up a component is O(1). Component types are assigned IDs at compile time through templates. 

Queries are done using variadic templates and Iterators, for example `reg.query<Transform, Mesh>()`. They return an `Iterator` you can use in a range-based for-loop. A query iterates the *smallest* of the requested component pools and checks membership in the others, so its cost scales with the rarest component rather than the total entity count. When Entities/Components are "removed", deletion is deferred. Anything removed mid-frame is marked "dead", skipped by the iterator, and removed in a single `flush()` at the end of the frame. This makes it safe to destroy entities while iterating.

Entities are generational handles. Each entity index carries a generation counter, so a stale handle to a destroyed entity is detected instead of silently aliasing whatever reuses its slot. 

Logic lives in **systems**: classes with `start()`, `update()` and `onDestroy()` hooks. Systems like camera control and rendering are all implemented as systems that read plain-data components.

### Future Plans
I am planning to turn this project into a Minecraft clone as it's something I'm familiar with and offers me with a sandbox-y environment to explore programming ideas such as more advanced lighting and shader effects. I aim to responsibly use AI mainly as a learning tool as game engines and graphics programming necessitate deep understanding of underlying systems and hardware.
