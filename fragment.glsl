uniform vec3 lightv;
varying vec3 normalv, posv;

void main() {
	vec3 realnormal = normalize(normalv);
	vec3 reallight = normalize(lightv-posv);
	float dotp = dot(reallight, realnormal);
	float col = .2;
	if (dotp > 0) {
		float specDot = dot(-normalize(posv), 2*dotp*realnormal-reallight);
		specDot = max(specDot, 0);
		col += .3*dotp+.3*pow(specDot, 10)*pow(dotp, .25);
	}
	gl_FragColor = vec4(col*gl_Color.r, col*gl_Color.g, col*gl_Color.b, 1.0);
}
