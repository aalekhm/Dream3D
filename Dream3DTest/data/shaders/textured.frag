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

uniform float u_time;

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
	vec2 ct = v_texCoord;	
	ct.y -= u_time * 0.05;
	vec4 clouds = texture2D(u_diffuseTexture, ct);
    
    _baseColor = texture2D(u_diffuseTexture, v_texCoord) * 0.5;
	_normalColor = texture2D(u_normalmapTexture, ct) * 0.5;
	
    gl_FragColor = (_baseColor + _normalColor);
}
