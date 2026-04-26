#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec3 primary;
uniform vec3 secondary;

out vec4 finalColor;

void main() {
   vec4 pixel = texture2D(texture0, fragTexCoord);
   if (pixel.a < 0.01) {
      discard;
   }

   float brightness = dot(pixel.rgb, vec3(0.333));
   vec3 finalSecondary = mix(secondary * 0.55, secondary, 1.0 - brightness);
   vec3 finalPrimary = mix(primary * 0.45, primary, brightness);
   vec3 finalMix = mix(finalSecondary, finalPrimary, brightness);

   finalColor = vec4(finalMix.rgb, fragColor.a);
}
