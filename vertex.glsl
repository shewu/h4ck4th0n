varying vec3 normalv, posv;

void main() {
	normalv = gl_NormalMatrix * gl_Normal;
	posv = vec3(gl_ModelViewMatrix * gl_Vertex);
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
