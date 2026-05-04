// from Raylib's examples
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 viewport;
uniform vec2 pixelSize;
uniform float fade;
uniform float time;

out vec4 finalColor;

void main() {
    float x = 1.5 / viewport.x;
    float y = 1.5 / viewport.y;

    vec2 uv = fragTexCoord;
    uv.x += sin(uv.y * 20.0 + time * 0.5) * (0.003 * fade);
    uv.y += cos(uv.x * 20.0 + time * 0.5) * (0.003 * fade);

    vec4 horizEdge = vec4(0.0);
    horizEdge -= texture(texture0, vec2(uv.x - x, uv.y - y))*1.0;
    horizEdge -= texture(texture0, vec2(uv.x - x, uv.y   ))*2.0;
    horizEdge -= texture(texture0, vec2(uv.x - x, uv.y + y))*1.0;
    horizEdge += texture(texture0, vec2(uv.x + x, uv.y - y))*1.0;
    horizEdge += texture(texture0, vec2(uv.x + x, uv.y   ))*2.0;
    horizEdge += texture(texture0, vec2(uv.x + x, uv.y + y))*1.0;

    vec4 vertEdge = vec4(0.0);
    vertEdge -= texture(texture0, vec2(uv.x - x, uv.y - y))*1.0;
    vertEdge -= texture(texture0, vec2(uv.x    , uv.y - y))*2.0;
    vertEdge -= texture(texture0, vec2(uv.x + x, uv.y - y))*1.0;
    vertEdge += texture(texture0, vec2(uv.x - x, uv.y + y))*1.0;
    vertEdge += texture(texture0, vec2(uv.x    , uv.y + y))*2.0;
    vertEdge += texture(texture0, vec2(uv.x + x, uv.y + y))*1.0;

    vec3 edge = sqrt((horizEdge.rgb*horizEdge.rgb) + (vertEdge.rgb*vertEdge.rgb));
    vec4 source = texture(texture0, uv);

    finalColor = vec4(mix(source.rgb, edge, fade), source.a);
}