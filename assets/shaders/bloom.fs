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

const float samples = 5.0;
const float quality = 2.5;

void main() {
    vec4 sum = vec4(0);
    vec2 sizeFactor = vec2(1)/viewport*quality;

    // Texel color fetching from texture sampler
    vec4 source = texture(texture0, fragTexCoord);

    const int range = 2;// (samples - 1) / 2;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            sum += texture(texture0, fragTexCoord + vec2(x, y)*sizeFactor);
        }
    }

    // Calculate final fragment color
    finalColor = ((sum/(samples*samples)) * fade + source);
}