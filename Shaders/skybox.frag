#version 330 core

uniform vec3 sunDir;

const float Br = 0.0025;
const float Bm = 0.0003;
const float g  = 0.98;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragColour;

// Based on https://github.com/shff/opengl_sky
void main() {
	vec3 viewDir = IN.viewDir;
	viewDir.y = abs(viewDir.y);
	float mu = dot(normalize(viewDir), normalize(sunDir));
	float rayleigh = 3.0 / (8.0 * 3.1415926535) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
	
	vec3 day_extinction =
		exp(-exp(-((viewDir.y + sunDir.y * 4.0) * (exp(-viewDir.y * 16.0) * 0.1) / 80.0) / Br) * (exp(-viewDir.y * 16.0) + 0.1) * Kr / Br) *
		exp(-viewDir.y * exp(-viewDir.y * 8.0) * 4.0) *
		exp(-viewDir.y * 2.0) * 4.0;
	vec3 night_extinction = vec3(1.0 - exp(sunDir.y)) * 0.2;
	vec3 extinction = mix(day_extinction, night_extinction, -sunDir.y * 0.2 + 0.5);

	fragColour = vec4(rayleigh * mie * extinction, 1.0);
}
