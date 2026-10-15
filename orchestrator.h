#pragma once

#include "registry.h"
#include "debug.h"
#include "system.h"

class Orchestrator {
private:
	Registry& reg;
	std::vector<std::shared_ptr<ISystem>> systems;
	uint64_t frame = 0;
	bool shouldEnd = false;
public:

	Orchestrator(
		Registry& reg
	)
		: reg(reg)
	{
		
	}

	void update() {
		frame++;
		
		//make a copy of the systems this frame
		std::vector<std::shared_ptr<ISystem>> systemsThisFrame = systems;
		
		systems.clear(); //we will be refilling it with systems that aren't marked for deletion

		for (int i = 0; i < systemsThisFrame.size(); ++i) {
			auto s = systemsThisFrame[i];

			if (!(s->shouldRemove)) { 
				systems.push_back(s); //re-add any systems not marked for deletion
			} else {
				s->onDestroy();
				continue;
			}

			if (s->disabled)
				continue;

			if (!(s->started)) {
				s->start();
				s->started = true;
			}

			s->update();
		}
		
		reg.flush(); //clear the registry of entities and comps marked as dead
	}

	void addSystem(std::shared_ptr<ISystem> system) {
		if (system->enforceSingle) {
			for (const auto& existing : systems) {
				if (typeid(*existing) == typeid(*system)) {
					throw std::runtime_error("Only one instance of this system type is allowed");
				}
			}
		}

		systems.push_back(system); //this should work regardless of where we're at
	}

	void end() {
		for (auto s : systems) {
			s->onDestroy();
		}
		Debug::log("end() called on frame " + std::to_string(frame));
		shouldEnd = true;
	}

	bool hasEnded() {
		return shouldEnd;
	}

};