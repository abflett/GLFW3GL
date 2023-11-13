#version 330 core

in vec2 FragCoord; // Fragment coordinates
uniform float time; // Time variable for animation

out vec4 FragColor; // Output color

void main() {
    vec2 uv = FragCoord/iResolution.xy; // Normalized coordinates
    
    // Create a wave pattern based on time and coordinates
    float wave = sin(uv.x * 10.0 + time * 2.0) + sin(uv.y * 10.0 + time * 2.0);
    
    // Add color to the water based on the wave pattern
    vec3 color = vec3(0.0, 0.5, 1.0) + vec3(0.2, 0.3, 0.5) * wave;

    // Output the final color
    FragColor = vec4(color, 1.0);
}
