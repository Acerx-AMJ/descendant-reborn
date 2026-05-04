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
   float dx = pixelSize.x * (1.0 / viewport.x);
   float dy = pixelSize.y * (1.0 / viewport.y);

   vec2 coord = vec2(dx * floor(fragTexCoord.x / dx), dy * floor(fragTexCoord.y / dy));
   vec3 tc = texture(texture0, coord).rgb;
   finalColor = vec4(tc, fragColor.a);
}