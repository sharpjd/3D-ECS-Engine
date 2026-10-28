#pragma once 

#include "file.h"
#include <unordered_map>
#include <stdexcept>
#include "stb_image.h"

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
		if (loaded) return;
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

	//TODO there may be no vert shader
	LoadableStringFile& getLoadable(IdEnum id) {
		return getLoadables().at(id);
	}
};

enum class ShaderId {
	Undefined,
	Hello,
	Hello2,
	Hello3,
	Hello4,
	Hello5,
	LightingDemo,
	Light,
	Phong,
	PhongLightMap,
};

class VertexShaderLoader : public StringFileLoader<ShaderId> {
private:
	std::unordered_map<ShaderId, LoadableStringFile> loadables = {
		{ShaderId::Hello, LoadableStringFile("assets/shaders/hello.vert")},
		{ShaderId::Hello2, LoadableStringFile("assets/shaders/hello2.vert")},
		{ShaderId::Hello3, LoadableStringFile("assets/shaders/hello3.vert")},
		{ShaderId::Hello4, LoadableStringFile("assets/shaders/hello4.vert")},
		{ShaderId::Hello5, LoadableStringFile("assets/shaders/hello5.vert")},
		{ShaderId::LightingDemo, LoadableStringFile("assets/shaders/lightingDemo.vert")},
		{ShaderId::Phong, LoadableStringFile("assets/shaders/phongDemo.vert")},
		{ShaderId::PhongLightMap, LoadableStringFile("assets/shaders/matLightDemo.vert")},
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
		{ShaderId::LightingDemo, LoadableStringFile("assets/shaders/lightingDemo.frag")},
		{ShaderId::Light, LoadableStringFile("assets/shaders/light.frag")},
		{ShaderId::Phong, LoadableStringFile("assets/shaders/phongDemo.frag")},
		{ShaderId::PhongLightMap, LoadableStringFile("assets/shaders/matLightDemo.frag")},
	};
public:
	std::unordered_map<ShaderId, LoadableStringFile>& getLoadables() override {
		return loadables;
	}
};

struct ShaderManager {
	VertexShaderLoader& vert;
	FragShaderLoader& frag;
};

class LoadableTextureFile {
private:
	unsigned char* data = nullptr;
	std::string filename;
	bool loaded = false;
	int width_ = 0, height_ = 0, nrChannels_ = 0;
public:
	LoadableTextureFile(std::string filename)
		: filename(filename)
	{
	}

	unsigned char* getTexture() {
		if (!loaded) {
			throw std::runtime_error("load the texture file before reading it");
		}
		return data;
	}

	void load() {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(filename.c_str(), &width_, &height_, &nrChannels_, 0);
		if (!data) {
			Debug::log("Failed to load texture: " + filename);
			assert(false);
		}
		loaded = true;
	}

	void unload() {
		loaded = false;
		stbi_image_free(data);
	}

	const int width() { return width_; }
	const int height() { return height_; }
	const int nrChannels() { return nrChannels_; }
};

template<typename IdEnum>
class TextureFileLoader {
public:

	TextureFileLoader() {
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

	virtual std::unordered_map<IdEnum, LoadableTextureFile>& getLoadables() = 0;

	LoadableTextureFile& getLoadable(IdEnum id) {
		return getLoadables().at(id);
	}
};

enum class TextureId {
	Jerma,
	Hank,
	Container,
	Container_Specular
};

class TextureLoader : public TextureFileLoader<TextureId> {
private:
	std::unordered_map<TextureId, LoadableTextureFile> loadables = {
		{TextureId::Jerma, LoadableTextureFile("assets/images/jerma.png")},
		{TextureId::Hank, LoadableTextureFile("assets/images/hank.png")},
		{TextureId::Container, LoadableTextureFile("assets/images/container.png")},
		{TextureId::Container_Specular, LoadableTextureFile("assets/images/container_specular.png")},
	};
public:
	std::unordered_map<TextureId, LoadableTextureFile>& getLoadables() override {
		return loadables;
	}
};

//TODO: in the future you could be able to specifiy and pre-load what you need
class TextureManager {
private:
public:
	TextureLoader& texLoader;

	LoadableTextureFile& getLoadable(TextureId id) {
		return texLoader.getLoadable(id);
	}

	void loadAll() {
		texLoader.loadAll();
	}
};

