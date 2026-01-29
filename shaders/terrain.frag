#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;

out vec4 FragColor;

uniform vec3 uLightDir = normalize(vec3(0.5, 1.0, 0.3));

vec3 getColor(float h) {
    if (h < -0.3) return vec3(0.0, 0.0, 0.6);      // deep water
    else if (h < 0.0) return vec3(0.0, 0.5, 1.0);   // shallow water
    else if (h < 0.3) return vec3(0.2, 0.8, 0.2);   // grass
    else if (h < 0.6) return vec3(0.5, 0.5, 0.5);   // rock
    else return vec3(1.0, 1.0, 1.0);               // snow
}

void main() {
    vec3 N = normalize(Normal);
    float diff = max(dot(N, normalize(uLightDir)), 0.0);

    vec3 baseColor = getColor(Height);
    vec3 color = baseColor * diff; // simple diffuse lighting

    FragColor = vec4(color, 1.0);
}

