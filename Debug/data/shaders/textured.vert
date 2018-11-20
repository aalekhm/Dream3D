///////////////////////////////////////////////////////////
// Atributes
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;
///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_worldViewProjectionMatrix;
///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texCoord;
varying vec4 v_color;
///////////////////////////////////////////////////////////

vec4 getVertexPosition()
{
    return a_position;    
}

vec2 getVertexTexCoord()
{
	return a_texCoord;
}

vec4 getVertexColor()
{
	return a_color;
}

void main()
{
    vec4 position = getVertexPosition();
    gl_Position = u_worldViewProjectionMatrix * position;
    
    v_texCoord = getVertexTexCoord();
	v_color = getVertexColor();
}
