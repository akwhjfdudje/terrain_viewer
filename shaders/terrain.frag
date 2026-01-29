#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;

out vec4 FragColor;

uniform vec3 uLightDir = normalize(vec3(0.5, 1.0, 0.3));
uniform float uWaterLevel = 0.0;        // Y-coordinate of water surface
uniform vec3 uWaterColor = vec3(0.0, 0.3, 0.6); // blue water
uniform float uWaterOpacity = 0.9;      // 0 = fully transparent, 1 = opaque
uniform float uTime = 0.0; // for wave animation

vec3 getTerrainColor(float h) {
    if (h < -30) return vec3(0.0, 0.0, 0.6);      // deep water
    else if (h < 0) return vec3(0.0, 0.5, 1.0);   // shallow water
    else if (h < 30) return vec3(0.2, 0.8, 0.2);   // grass
    else if (h < 100) return vec3(0.5, 0.5, 0.5);   // rock
    else return vec3(1.0, 1.0, 1.0);               // snow
}

void main() {
    vec3 N = normalize(Normal);
    float diff = max(dot(N, normalize(uLightDir)), 0.0);

    // Terrain base color with lighting
    vec3 terrainColor = getTerrainColor(Height);
    vec3 litColor = terrainColor * diff;

    // Water effect
    if (Height < uWaterLevel) {
        // Simple wave distortion (offset color slightly by sin waves)
        float wave = 0.02 * sin(FragPos.x * 3.0 + uTime * 2.0) 
                   + 0.02 * cos(FragPos.z * 3.0 + uTime * 2.0);
        
        // Refraction: mix terrain color with water color
        float alpha = clamp(uWaterOpacity, 0.0, 1.0);
        vec3 distortedColor = mix(litColor, uWaterColor, alpha);

        // Slight wave tint
        distortedColor += wave * 0.1;

        litColor = distortedColor;
    }

    FragColor = vec4(litColor, 1.0);
}
