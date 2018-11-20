
// Swirl effect parameters
vec4 swirl(sampler2D tex, vec2 uv, float radius, float fSwirlFactor, bool bIsClockwise)
{
	vec2 texSize = textureSize(tex, 0);
	vec2 center = vec2(texSize.x / 2.0, texSize.y/2.0);

	// Texture coordinate w.r.t the texSize.
	vec2 tc = uv * texSize;

	// Texture coordinate from the center of the texture.
	tc -= center;

	// Distance from the center.
	float dist = length(tc);
	if (dist < radius)
	{
		float fRotation = bIsClockwise ? 1 : -1;

		// Find % distance from the center.
		float percent = (radius - dist) / radius;

		// Higher the distance from the center, less is the angle to rotate,
		// hence the texture appears without swirl near the edge of the radius.
		// i.e, dist == radius, % = 0, hence theta = 0 (no rotation).
		// &    dist == 0	  , % = 100, theta = MAX(directly prop to the swirl factor).
		//float theta = percent * fSwirlFactor * fRotation;

		// Progressice swirl :
		// lower the percentage, lower is the theta.
		// theta increases by factor of (% ^ 3) as the distance from the center decreases.
		float theta = percent * percent * percent * fSwirlFactor * fRotation;

		float s = sin(theta);
		float c = cos(theta);

		// Multiply texture coordinate with the Rotation Matrix.
		tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
	}
	tc += center;
	vec3 color = texture2D(tex, tc / texSize).rgb;

	return vec4(color, 1.0);
}