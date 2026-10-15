#pragma once

class Registry;
class Orchestrator;
class Input;
class ShaderFileLoader;
class Window;
class Time;

struct EngineContext { //so we don't have to write a bunch of code just to pass references
	Registry& reg;
	Orchestrator& orch;
	Input& input;
	ShaderFileLoader& shaderLoader;
	Window& window;
	Time& time;
};