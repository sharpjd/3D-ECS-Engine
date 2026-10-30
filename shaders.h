#pragma once

#include "shader.h"

/*
* Concrete shaders. Each declares its uniforms as members, so call sites read
*   shader.model.set(m);
* Only declare uniforms the GLSL actually has (see the UNIFORM_NOT_FOUND warning).
*/

struct Hello5Shader : MCShader {
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    Uniform<int> texture1{ *this, "texture1" };
    Uniform<int> texture2{ *this, "texture2" };
    Uniform<float> alpha{ *this, "alpha" };

    Hello5Shader(ShaderManager& sm) : MCShader(ShaderId::Hello5, sm) {
        //tell OpenGL which texture unit each sampler2D corresponds to (once; it's stored in the program)
        use();
        texture1.set(0);
        texture2.set(1);
    }
};

struct CubeLightingShader : MCShader {
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    Uniform<glm::vec3> objectColor{ *this, "objectColor" };
    Uniform<glm::vec3> lightColor{ *this, "lightColor" };

    CubeLightingShader(ShaderManager& sm) : MCShader(ShaderId::LightingDemo, sm) {}
};

struct LightShader : MCShader {
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    LightShader(ShaderManager& sm) : MCShader(ShaderId::LightingDemo, ShaderId::Light, sm) {}
};


struct PhongShader : MCShader {
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    Uniform<glm::vec3> viewPos{ *this, "viewPos" };

    Uniform<glm::vec3> materialAmbient{ *this, "material.ambient" };
    Uniform<glm::vec3> materialDiffuse{ *this, "material.diffuse" };
    Uniform<glm::vec3> materialSpecular{ *this, "material.specular" };
    Uniform<float> materialShininess{ *this, "material.shininess" };

    Uniform<glm::vec3> lightPosition{ *this, "light.position" };
    Uniform<glm::vec3> lightAmbient{ *this, "light.ambient" };
    Uniform<glm::vec3> lightDiffuse{ *this, "light.diffuse" };
    Uniform<glm::vec3> lightSpecular{ *this, "light.specular" };

    PhongShader(ShaderManager& sm) : MCShader(ShaderId::Phong, sm) {}
};

struct PhongLightMapShader : MCShader {
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    Uniform<glm::vec3> viewPos{ *this, "viewPos" };

    Uniform<int> glTexUnit_materialDiffuse{ *this, "material.diffuse" };
    Uniform<int> glTexUnit_materialSpecular{ *this, "material.specular" };
    Uniform<float> materialShininess{ *this, "material.shininess" };

    Uniform<glm::vec3> lightPosition{ *this, "light.position" };
    Uniform<glm::vec3> lightAmbient{ *this, "light.ambient" };
    Uniform<glm::vec3> lightDiffuse{ *this, "light.diffuse" };
    Uniform<glm::vec3> lightSpecular{ *this, "light.specular" };

    PhongLightMapShader(ShaderManager& sm) : MCShader(ShaderId::PhongLightMap, sm) {}
};

struct MultiLightShader : MCShader {
    //vert
    Uniform<glm::mat4> model{ *this, "model" };
    Uniform<glm::mat4> view{ *this, "view" };
    Uniform<glm::mat4> projection{ *this, "projection" };

    //frag
    Uniform<glm::vec3> viewPos{ *this, "viewPos" };

    Uniform<int> glTexUnit_materialDiffuse{ *this, "material.diffuse" };
    Uniform<int> glTexUnit_materialSpecular{ *this, "material.specular" };
    Uniform<float> materialShininess{ *this, "material.shininess" };

    Uniform<glm::vec3> dirLightDirection{ *this, "dirLight.direction" };
    Uniform<glm::vec3> dirLightAmbient{ *this, "dirLight.ambient" };
    Uniform<glm::vec3> dirLightDiffuse{ *this, "dirLight.diffuse" };
    Uniform<glm::vec3> dirLightSpecular{ *this, "dirLight.specular" };

    int pointLightsSize = 4;
    UniformArray<glm::vec3> pointLightPosition{ *this, "pointLights", pointLightsSize, ".position"};
    UniformArray<float> pointLightConstant{ *this, "pointLights", pointLightsSize, ".constant" };
    UniformArray<float> pointLightLinear{ *this, "pointLights", pointLightsSize, ".linear" };
    UniformArray<float> pointLightQuadratic{ *this, "pointLights", pointLightsSize, ".quadratic" };
    UniformArray<glm::vec3> pointLightAmbient{ *this, "pointLights", pointLightsSize, ".ambient" };
    UniformArray<glm::vec3> pointLightDiffuse{ *this, "pointLights", pointLightsSize, ".diffuse" };
    UniformArray<glm::vec3> pointLightSpecular{ *this, "pointLights", pointLightsSize, ".specular" };

    MultiLightShader(ShaderManager& sm) : MCShader(ShaderId::MultiLightDemo, sm) {}
};