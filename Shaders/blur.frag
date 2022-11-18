#version 330 core
uniform sampler2D sceneTex;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec4 scene = texture(sceneTex, IN.texCoord);
	fragColour = vec4(scene.rgb * weight[0], 1.0);
	if (horizontal) {
		for (int i = 1; i < 5; i++) {
			fragColour.rgb += textureOffset(sceneTex, IN.texCoord, ivec2( i, 0)).rgb * weight[i];
			fragColour.rgb += textureOffset(sceneTex, IN.texCoord, ivec2(-i, 0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			fragColour.rgb += textureOffset(sceneTex, IN.texCoord, ivec2(0,  i)).rgb * weight[i];
			fragColour.rgb += textureOffset(sceneTex, IN.texCoord, ivec2(0, -i)).rgb * weight[i];
		}
	}
}
