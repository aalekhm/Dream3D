
vec4 getBlackAndWhite(vec4 vInputColor)
{
	return vec4((vInputColor.x * 0.3333 + vInputColor.y * 0.3333, vInputColor.z * 0.3333));
}
