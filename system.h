#pragma once

#include "engineContext.h"
class Registry;
class Orchestrator;

class ISystem {
public:
	bool started = false;
	bool disabled = false;
	int priority = 0; //TODO dependencies? e.g. pointers to other systems, ranThisFrame...
	virtual void start() {};
	virtual void update() {};
	virtual void onDestroy() {};
	bool shouldRemove = false;
	EngineContext& ec;
	Registry& reg;
	Orchestrator& orch;
	Input& input;
	bool enforceSingle = false;
	ISystem(
		EngineContext& ec
	)
		: ec(ec),
		reg(ec.reg),
		orch(ec.orch),
		input(ec.input)
	{
	}

	virtual ~ISystem() = default;
};