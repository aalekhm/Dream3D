
#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif

#ifndef SPOT_LIGHT_COUNT
#define SPOT_LIGHT_COUNT 0
#endif

#ifndef DIRECTIONAL_LIGHT_COUNT
#define DIRECTIONAL_LIGHT_COUNT 0
#endif

#if (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0) || (DIRECTIONAL_LIGHT_COUNT > 0)
#define LIGHTING_ENABLED
#endif

///////////////////////////////////////////////////////////
// ATTRIBUTES
///////////////////////////////////////////////////////////
attribute vec3 	a_position;
attribute vec4 	a_color;
attribute vec2 	a_texCoord;

///////////////////////////////////////////////////////////
// UNIFORMS
///////////////////////////////////////////////////////////
uniform mat4 	u_worldViewProjectionMatrix;

///////////////////////////////////////////////////////////
// VARYINGS
///////////////////////////////////////////////////////////
varying vec2 	v_texCoord;
varying vec4 	v_color;
///////////////////////////////////////////////////////////

vec4 getVertexPosition()
{
    return vec4(a_position, 1.0);
}

vec2 getVertexTexCoord()
{
	return a_texCoord;
}

vec4 getVertexColor()
{
	return a_color;
}

#if defined(LIGHTING_ENABLED)
#include "lighting.vert"
#endif

void main()
{
    vec4 vPosition = u_worldViewProjectionMatrix * getVertexPosition();
    gl_Position = vPosition;
	
	#if defined(LIGHTING_ENABLED)
	{
		applyLight();
	}
	#endif

    v_texCoord = getVertexTexCoord();
	v_color = getVertexColor();
}
