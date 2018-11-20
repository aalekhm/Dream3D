
vec4 getBlackAndWhite(vec4 vInputColor)
{
	return vec4((vInputColor.x * 0.5 + vInputColor.y * 0.5, vInputColor.z * 0.5));
}
