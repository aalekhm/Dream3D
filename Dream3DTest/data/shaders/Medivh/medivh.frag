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

///////////////////////////////////////////////////////////
// Varying
varying vec2		v_texCoord;

void main()
{
	float t = u_time * 0.25;
	vec2 tc = v_texCoord;
	tc.x -= t*2.;
	
	vec4 mask = texture2D(u_sampler1, v_texCoord);
	
	float bump = texture2D(u_sampler4, tc*0.5).r; 
	
	vec4 img = texture2D(u_sampler0, vec2(v_texCoord.x, v_texCoord.y+bump * 0.055 * mask.g));
	tc = v_texCoord;
	tc.x-=0.05;
	tc.y+= 0.05;
	
	vec2 flow = texture2D(u_sampler3, tc).rg;
	flow.y+=t;
	flow.y*=2.;
	
	vec4 plasma = texture2D(u_sampler2, flow*1.5);
	plasma.rgb *= vec3(0.52, 0.26, 0.54);
	plasma *= 3.;
	
	flow = texture2D(u_sampler5, v_texCoord).rg;
	flow.g += t;
	
	vec4 plasma2 = texture2D(u_sampler2, flow);
	plasma2 *= 4.;
	plasma2.rgb *= vec3(0.52, 0.26, 0.54);
	
	gl_FragColor = img + plasma*mask.r + bump*mask.a*0.35 + plasma2*mask.b;
}



