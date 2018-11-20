
float rand(vec2 col)
{
	return fract( sin( dot(col.xy, vec2(92.0, 80.0)) ) + cos( dot(col.xy, vec2(41.0, 62.0)) ) * 5.1 );
}

vec4 getFrostedPixel(sampler2D tSampler, vec2 vTexCoord)
{
	vec2 rnd = vec2( rand(vTexCoord.xy), rand(vTexCoord.xy) );
	return texture2D(tSampler, vTexCoord.xy + rnd * 0.05);
}