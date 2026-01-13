
//float offset_GaussianBlur[3] = float[](0.0, 1.3846153846, 3.2307692308);
//float weight_GaussianBlur[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);
float offset_GaussianBlur[4] = float[](0.0, 1.3846153846, 2.2, 3.2307692308);
float weight_GaussianBlur[5] = float[](0.2, 0.05, 0.09, 0.11, 0.15);

vec4 applyGaussianBlur(sampler2D tSampler, vec2 vTexCoord)
{
	vec3 tc = vec3(0.0, 0.0, 0.0);
	{
		vec2 uv = vTexCoord.xy;
		vec2 tSize = textureSize(tSampler, 0);
		tc = texture2D(tSampler, uv).rgb * weight_GaussianBlur[0];

		// Apply Vertical
		for (int i = 1; i < 5; i++)
		{
			//tc += texture2D(tSampler, uv + vec2(0.0, offset_GaussianBlur[i]) / tSize.y).rgb * weight_GaussianBlur[i];
			//tc += texture2D(tSampler, uv - vec2(0.0, offset_GaussianBlur[i]) / tSize.y).rgb * weight_GaussianBlur[i];

			tc += texture2D(tSampler, uv + vec2(0.0, i / tSize.y)).rgb * weight_GaussianBlur[i];
			tc += texture2D(tSampler, uv - vec2(0.0, i / tSize.y)).rgb * weight_GaussianBlur[i];
		}

		// Apply Horizontal
		for (int i = 1; i < 3; i++)
		{
			//tc += texture2D(tSampler, uv + vec2(offset_GaussianBlur[i]) / tSize.x, 0.0).rgb * weight_GaussianBlur[i];
			//tc += texture2D(tSampler, uv - vec2(offset_GaussianBlur[i]) / tSize.x, 0.0).rgb * weight_GaussianBlur[i];

			tc += texture2D(tSampler, uv + vec2(i / tSize.x, 0.0)).rgb * weight_GaussianBlur[i];
			tc += texture2D(tSampler, uv - vec2(i / tSize.x, 0.0)).rgb * weight_GaussianBlur[i];
		}
	}

	return vec4(tc, 1.0);	
}