#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform vec3 u_ambientColor;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_normalmapTexture;

uniform float u_Time;

///////////////////////////////////////////////////////////
// Variables
vec4 _baseColor;
vec4 _normalColor;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texCoord;
varying vec4 v_color;

void main()
{ 
    _baseColor = texture2D(u_diffuseTexture, v_texCoord) * u_Time;
	_normalColor = texture2D(u_normalmapTexture, v_texCoord) * u_Time;
    gl_FragColor = (_baseColor + _normalColor) * v_color;
}
