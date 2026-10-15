#pragma once 

#include "file.h"
#include <unordered_map>
#include <stdexcept>

class LoadableStringFile {
private:
	std::string str;
	std::string filename;
	bool loaded = false;
public:
	LoadableStringFile(std::string filename)
		: filename(filename)
	{
	}

	std::string& get() {
		//if (!loaded) {
		//	str = File::load_file(filename);
		//	loaded = true;
		//}
		if (!loaded) {
			throw std::runtime_error("load the string file before reading it");
		}
		return str;
	}

	void load() {
		str = File::load_file(filename);
		loaded = true;
	}

	void unload() {
		loaded = false;
		str.clear();
		str.shrink_to_fit();
	}
};

/*
* This is meant to map enums to a basic file loader
*/
template<typename IdEnum>
class StringFileLoader {
public:

	StringFileLoader() {
	}

	void loadAll() {
		for (auto& [key, value] : getLoadables()) {
			value.load();
		}
	}
	void unloadAll() {
		for (auto& [key, value] : getLoadables()) {
			value.unload();
		}
	}

	virtual std::unordered_map<IdEnum, LoadableStringFile>& getLoadables() = 0;

	LoadableStringFile& getLoadable(IdEnum id) {
		return getLoadables().at(id);
	}

	void load(IdEnum id) {
		getLoadable(id).load();
	}

	const char* getData(IdEnum id) {
		return getLoadable(id).get().data();
	}

	void unload(IdEnum id) {
		getLoadable(id).unload();
	}
};

enum class ShaderId {
	Undefined,
	Hello,
	Hello2,
	Hello3,
	Hello4,
	Hello5
};

class VertexShaderLoader : public StringFileLoader<ShaderId> {
private:
	std::unordered_map<ShaderId, LoadableStringFile> loadables = {
		{ShaderId::Hello, LoadableStringFile("assets/shaders/hello.vert")},
		{ShaderId::Hello2, LoadableStringFile("assets/shaders/hello2.vert")},
		{ShaderId::Hello3, LoadableStringFile("assets/shaders/hello3.vert")},
		{ShaderId::Hello4, LoadableStringFile("assets/shaders/hello4.vert")},
		{ShaderId::Hello5, LoadableStringFile("assets/shaders/hello5.vert")},
	};
public:
	std::unordered_map<ShaderId, LoadableStringFile>& getLoadables() override {
		return loadables;
	}
};

class FragShaderLoader : public StringFileLoader<ShaderId> {
private:
	std::unordered_map<ShaderId, LoadableStringFile> loadables = {
		{ShaderId::Hello, LoadableStringFile("assets/shaders/hello.frag")},
		{ShaderId::Hello2, LoadableStringFile("assets/shaders/hello2.frag")},
		{ShaderId::Hello3, LoadableStringFile("assets/shaders/hello3.frag")},
		{ShaderId::Hello4, LoadableStringFile("assets/shaders/hello4.frag")},
		{ShaderId::Hello5, LoadableStringFile("assets/shaders/hello5.frag")},
	};
public:
	std::unordered_map<ShaderId, LoadableStringFile>& getLoadables() override {
		return loadables;
	}
};

struct ShaderFileLoader {
	VertexShaderLoader& vert;
	FragShaderLoader& frag;
};