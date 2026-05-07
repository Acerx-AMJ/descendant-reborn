// from Raylib's examples
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 viewport;
uniform float fade;

out vec4 finalColor;

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
    // Texel color fetching from texture sampler
    vec3 texelColor = texture(texture0, fragTexCoord).rgb*weight[0];

    for (int i = 1; i < 3; i++)
    {
        texelColor += texture(texture0, fragTexCoord + vec2(offset[i])*1.5/viewport, 0.0).rgb*weight[i];
        texelColor += texture(texture0, fragTexCoord - vec2(offset[i])*1.5/viewport, 0.0).rgb*weight[i];
    }

    vec4 source = texture(texture0, fragTexCoord);
    finalColor = vec4(mix(source.rgb, texelColor, fade), 1.0);
}