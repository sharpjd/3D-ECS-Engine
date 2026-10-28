#pragma once

class Registry;
class Orchestrator;
class Input;
class ShaderManager;
class Window;
class Time;
class TextureManager;

struct EngineContext { //so we don't have to write a bunch of code just to pass references
	Registry& reg;
	Orchestrator& orch;
	Input& input;
	ShaderManager& shaders;
	TextureManager& textures;
	Window& window;
	Time& time;
};