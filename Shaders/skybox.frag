#version 330 core

uniform vec3 sunDir;

uniform float time;
uniform float cirrus;
uniform float cumulus;

const int p[512] = int[](
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234,
    75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237,
    149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48,
    27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105,
    92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73,
    209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215,
    61, 156, 180,
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234,
    75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237,
    149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48,
    27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105,
    92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73,
    209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215,
    61, 156, 180
);

const float Br = 0.0025;
const float Bm = 0.0003;
const float g  = 0.98;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

float grad(int hash, vec3 pos) {
	int h = hash & 15;
	float u = h < 8 ? pos.x : pos.y;
	float v = h < 4 ? pos.y : ((h == 12 || h == 14) ? pos.x : pos.z);
	return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -v : v);
}

vec4 taylorInvSqrt(vec4 r) {
	return 1.79294291400159 - 0.85373472095314 * r;
}

ivec4 perm(ivec4 index) {
	return ivec4(p[index.x], p[index.y], p[index.z], p[index.w]);
}

// Adapted from https://stegu.github.io/webgl-noise/webdemo/
float simplex(vec3 v) {
	const float n_ = 0.142857142857;
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	ivec3 i = ivec3(floor(v + dot(v, C.yyy)));
	vec3 x0 = v - i + dot(i, C.xxx);

	ivec3 g = ivec3(step(x0.yzx, x0.xyz));
	ivec3 l = 1 - g;
	ivec3 i1 = min(g.xyz, l.zxy);
	ivec3 i2 = max(g.xyz, l.zxy);

	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy;
	vec3 x3 = x0 - D.yyy;

	i &= 255; 
	vec4 p = perm(perm(perm(i.z + ivec4(0.0, i1.z, i2.z, 1.0)) + i.y + ivec4(0.0, i1.y, i2.y, 1.0)) + i.x + ivec4(0.0, i1.x, i2.x, 1.0));

	vec3  ns = n_ * D.wyz - D.xzx;

	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );

	vec4 x = x_ * ns.x + ns.yyyy;
	vec4 y = y_ * ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);

	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	vec3 p0 = vec3(a0.xy, h.x);
	vec3 p1 = vec3(a0.zw, h.y);
	vec3 p2 = vec3(a1.xy, h.z);
	vec3 p3 = vec3(a1.zw, h.w);

	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	vec4 m = max(0.5 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	m = m * m;
	return 105.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3))) * 0.5 + 0.5;
}

float fBm(vec3 pos) {
	float f = simplex(pos) * 0.5
			+ simplex(pos * 2.0) * 0.25
			+ simplex(pos * 4.0) * (1.0 / 6.0)
			+ simplex(pos * 8.0) * (1.0 / 12.0)
			+ simplex(pos * 16.0) * (1.0 / 24.0);
	return f;
}

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragColour;

// Adapted from https://github.com/shff/opengl_sky
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
	
	if (IN.viewDir.y > 0.0) {
		float cirrusDensity = smoothstep(1.0 - cirrus, 1.0, fBm(viewDir / viewDir.y * 2.0 + time * 0.05)) * 0.3;
		fragColour.rgb = mix(fragColour.rgb, extinction * 4.0, cirrusDensity * viewDir.y);

		for (int i = 0; i < 10; i++) {
			float cumulusDensity = smoothstep(1.0 - cumulus, 1.0, fBm((0.7 + float(i) * 0.01) * viewDir / viewDir.y + time * 0.3));
			fragColour.rgb = mix(fragColour.rgb, extinction * cumulusDensity * 5.0, min(cumulusDensity, 1.0) * viewDir.y);
		}
	}

	fragColour.rgb += simplex(viewDir * 1000) * 0.01;
}
