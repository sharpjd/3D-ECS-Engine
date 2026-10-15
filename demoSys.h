#pragma once

#include "system.h"
#include "registry.h"


class PLACEHOLDERSys : public ISystem {
private:
public:
	PLACEHOLDERSys(EngineContext& ec) : ISystem(ec)
	{
	}
	
	void start() override {

	}

	void update() override {

	}

	void onDestroy() override {

	}
};



class DumbComponent {
	int dumb = 67;
};

class HelloComponent {
public:
	std::string text = "Hello world";
	int counter = 0;
};

class DemoSys : public ISystem { //notice we need the base to be public
private:
	bool printStuff = false;
public:
	DemoSys(EngineContext& world, bool printStuff) 
		: printStuff(printStuff), //custom constructor stuff
		ISystem(world) //must pass reference to registry to base class
	{} 

	void start() override {
		/*EntityHandle e0 = reg.newEntity();
		reg.addComponent(e0, HelloComponent{ "hello earth", 0 });

		EntityHandle e1 = reg.newEntity();
		reg.addComponent(e1, HelloComponent{ "hello world", 0 });
		reg.addComponent(e1, DumbComponent());*/

		EntityHandle e = ec.reg.newEntity();
		reg.addComponent(e, HelloComponent{ "hello universe", 0 });
		reg.addComponent(e, DumbComponent());
	}

	void update() override {
		Query q = reg.query<HelloComponent, DumbComponent>();

		for (auto [entity, helloComponent, dumbComponent] : q) {

			if(printStuff)
				Debug::log(helloComponent.text + " " + std::to_string(helloComponent.counter));

			helloComponent.counter++;

			if (helloComponent.counter > 5) {
				reg.removeComponent<HelloComponent>(entity);
				reg.deleteEntity(entity);

				if(printStuff)
					Debug::log("Goodbye world");

				auto e1 = reg.newEntity();
				auto e2 = reg.newEntity();
				reg.addComponent(e1, HelloComponent{ "hello space", 0 });
				reg.addComponent(e1, DumbComponent());
				reg.addComponent(e2, HelloComponent{ "hello galaxy", 0 });
				reg.addComponent(e2, DumbComponent());
			}
		}
	}
};