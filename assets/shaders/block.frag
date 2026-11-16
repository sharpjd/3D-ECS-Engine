#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct GlobalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform GlobalLight globalLight;

const float shininess = 1.0f;

void main() {

    vec3 baseColor = vec3(0.7f, 1.0f, 0.6f);

    vec3 ambient = vec3(0.3f, 0.3f, 0.3f);
    vec3 diffuse_ = vec3(0.3f);
    vec3 specular = vec3(0.3f);

    vec3 lightDir = normalize(-globalLight.direction);

    float diff = max(dot(Normal, lightDir), 0.0);

    //vec3 reflectDir = reflect(-lightDir, Normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 diffuse = diffuse_ * diff * vec3(1.0f);

    vec3 result = (diffuse + ambient) * baseColor;

    FragColor = vec4(result, 1.0f);
}