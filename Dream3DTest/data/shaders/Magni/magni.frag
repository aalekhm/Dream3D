#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

#include "blackAndWhiteFilter.h"
#include "edgeDetectionIWithSobelFilter.h"

///////////////////////////////////////////////////////////
// Uniforms
uniform float 		u_time;

uniform sampler2D 	u_sampler0;
uniform sampler2D 	u_sampler1;
uniform sampler2D 	u_sampler2;
uniform sampler2D 	u_sampler3;
uniform sampler2D 	u_sampler4;
uniform sampler2D 	u_sampler5;

///////////////////////////////////////////////////////////
// Varying
varying vec2		v_texCoord;

void renderOriginalMagni()
{
	float t = u_time * 0.25;
	
	vec3 mask = texture2D(u_sampler1, v_texCoord).rgb;
	
	vec4 img = texture2D(u_sampler0, v_texCoord);
	img.rg *= 1.1;
	
	vec2 flow = texture2D(u_sampler3, v_texCoord).gr;
	flow.y += t*3.;
	
	vec4 plas = texture2D(u_sampler2, flow*1.2) * mask.r;
	plas *= 15.5;
	plas *= vec4(0.239, 0.224, 0.488,1.);
	vec2 ct = v_texCoord;
	ct.y -= t*0.5;

	vec4 clouds = texture2D(u_sampler4, ct*2.);
	float clouds_a = clouds.a;
	clouds *= 4.5;
	clouds *= vec4(0.275, 0.23, 0.161, 1.);
	clouds_a *= mask.b;
	img += clouds * mask.b;
	img += plas * mask.r ;
	img += (sin(t*15.)+1.) * mask.g * vec4(0.239, 0.224, 0.488,1.)*2.;
	ct.x += t * 0.5;
	vec4 clouds_overall = texture2D(u_sampler5, ct * 0.5);
	clouds_overall *= vec4(0.275, 0.23, 0.161, 1.);
	
	gl_FragColor = img +clouds_overall;
}

void renderMagniMy()
{
	float t = u_time * 0.25;

	//vec4 magniImg = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 magniImg = texture2D(u_sampler0, v_texCoord);
	magniImg.w = 1.0;
	
	float redMask = texture2D(u_sampler1, v_texCoord).r;
	float greenMask = texture2D(u_sampler1, v_texCoord).g;

	vec2 flow = texture2D(u_sampler3, v_texCoord).gr;
	flow.y += t*2.;

	vec4 plasma = texture2D(u_sampler2, flow);
	plasma *= 2.0;
	
	vec4 greenPlasmaTint = vec4(0.0, 1., 0.0, 1.) * 2;
	float fBlinkEffect = (sin(t*15.)+3.5);
	
	magniImg += fBlinkEffect * (plasma * redMask * greenMask) * greenPlasmaTint;
	
	magniImg = findEgde(u_sampler0, v_texCoord);
	//gl_FragColor = getBlackAndWhite(magniImg);
	gl_FragColor = magniImg;
} 

void main()
{
	//renderOriginalMagni();
	renderMagniMy();
}



