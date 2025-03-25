#version 330 core
in vec2 uv;
out vec4 FragColor;

void main() {
    vec3 sky = mix(vec3(0.4,0.6,1.0), vec3(0.7,0.9,1.0), uv.y);
    vec2 sunPos = vec2(0.5,0.7);
    float sun = smoothstep(0.05,0.03,distance(uv,sunPos));
    sun += smoothstep(0.1,0.0,distance(uv,sunPos))*0.5;
    sky += sun*vec3(1.0,0.9,0.5);
    FragColor = vec4(sky,1.0);
}