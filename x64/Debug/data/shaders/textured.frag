#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif

#if (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0)
#define LIGHTING_ENABLED
#endif

///////////////////////////////////////////////////////////
// UNIFORMS
///////////////////////////////////////////////////////////
uniform sampler2D 	u_diffuseTexture;
uniform float 		u_time;

///////////////////////////////////////////////////////////
// VARIABLES
///////////////////////////////////////////////////////////
vec4 _baseColor;

///////////////////////////////////////////////////////////
// VARYINGS
///////////////////////////////////////////////////////////
varying vec2 		v_texCoord;
varying vec4 		v_color;
///////////////////////////////////////////////////////////

#if defined(LIGHTING_ENABLED)
#include "lighting.frag"
#endif

void main()
{ 
	// Ambient component
	_baseColor = texture2D(u_diffuseTexture, v_texCoord);
	gl_FragColor.a = _baseColor.a;

	#if defined(LIGHTING_ENABLED)
		gl_FragColor.rgb = getLitPixel();
	#else
		gl_FragColor.rgb = _baseColor.rgb;
	#endif
}
