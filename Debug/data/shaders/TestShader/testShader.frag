#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform float 		u_time;

uniform sampler2D 	u_sampler0;
uniform sampler2D 	u_sampler1;
uniform sampler2D 	u_sampler2;
uniform sampler2D 	u_sampler3;
uniform sampler2D 	u_sampler4;
uniform sampler2D 	u_sampler5;
uniform sampler2D 	u_sampler6;
uniform sampler2D 	u_sampler7;
uniform sampler2D 	u_sampler8;

///////////////////////////////////////////////////////////
// Varying
varying vec2		v_texCoord;

void renderMagniMy()
{
	float t = u_time * 0.25;

	vec4 magniImg = vec4(0.0, 0.0, 0.0, 1.0);
	//vec4 magniImg = texture2D(u_sampler0, v_texCoord) * .5;
	magniImg.w = 1.0;
	
	float redMask = texture2D(u_sampler1, v_texCoord).r;
	float greenMask = texture2D(u_sampler1, v_texCoord).g;

	vec2 flow = texture2D(u_sampler3, v_texCoord).gr;
	float redTint = flow.x;
	float greenTint = flow.y;
	flow.y += t * 1.5;

	vec4 plasma = texture2D(u_sampler2, flow * 1.2);
	plasma *= 2.0;
	
	vec4 greenPlasmaTint = vec4(1.0, 0., 0.0, 1.) * 2;
	float fBlinkEffect = (sin(t*15.)+3.5);
	
	magniImg += fBlinkEffect * (plasma * redMask * greenMask) * greenPlasmaTint;
	
	gl_FragColor = magniImg;
} 

void main()
{
	renderMagniMy();
}



