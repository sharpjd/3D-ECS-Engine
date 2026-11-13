#pragma once

/*
* don't forget to forward-declarea whenever we add something to EngineContext
*/
class Registry;
class Orchestrator;
class Input;
class ShaderManager;
class Window;
class Time;
class TextureManager;
class ModelManager;
class World;

struct EngineContext { //so we don't have to write a bunch of code just to pass references
	Registry& reg;
	Orchestrator& orch;
	Input& input;
	ShaderManager& shaders;
	TextureManager& textures;
	ModelManager& models;
	Window& window;
	Time& time;
	World& world;
};