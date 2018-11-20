
vec4 getHeatMap(sampler2D tSampler, vec2 vTexCoord)
{
	vec4 pixColor = texture2D(tSampler, vTexCoord);

	vec4 colors[3];
	colors[0] = vec4(0.0, 0.0, 1.0, 1.0);
	colors[1] = vec4(1.0, 1.0, 0.0, 1.0);
	colors[2] = vec4(1.0, 0.0, 0.0, 1.0);

	float luminosity = (pixColor.r + pixColor.g + pixColor.b) / 3;
	int idx = (luminosity < 0.5) ? 0 : 1;

	vec4 thermal = mix(colors[idx], colors[idx + 1], (luminosity - float(idx)*0.5) / 0.5);

	return thermal;
}