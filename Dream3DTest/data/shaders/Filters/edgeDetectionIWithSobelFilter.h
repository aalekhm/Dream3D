
//const float offset = 1.0f/3000.0f;
//const vec2	offsets_[9] = vec2[] (
//									vec2(-offset,	offset),
//									vec2(0,			offset),
//									vec2(offset,	offset),
//									vec2(-offset,	0),
//									vec2(0,			0),
//									vec2(offset,	0),
//									vec2(-offset,	-offset),
//									vec2(0,			-offset),
//									vec2(offset,	-offset)
//								);

const float xDirectionKernel[9] = float[] (
									-1.0,		0.0,		1.0,
									-2.0,		0.0,		2.0,
									-1.0,		0.0,		1.0
								);


const float yDirectionKernel[9] = float[] (
									-1.0,		-2.0,		-1.0,
									 0.0,		 0.0,		 0.0,
									 1.0,		 2.0,		 1.0
								);

vec4 findEgde(sampler2D tSampler, vec2 vTexCoord)
{
	vec3 vFinalEdge;

	// Edge detection is simple image processing, which aims at identifying points in a digital 
	// image at which the image brightness changes sharply, or more formally, has discontinuities.
	// Sobel Edge detection uses implementation of the Sobel operator as explained in 
	// http ://en.wikipedia.org/wiki/Sobel_operator.

	// Two kernels are applied to each pixel. One kernel finds the color change [gradient] in the X direction, 
	// and the other finds the color change [gradient] in the Y direction.

	vec2 tSize = textureSize(tSampler, 0);
	vec2 vPixelSize = (1 / tSize.x, 1 / tSize.y);

	vec2	offsets[9] = vec2[] (
									vec2(-vPixelSize.x,		vPixelSize.y),
									vec2(0,					vPixelSize.y),
									vec2(vPixelSize.x,		vPixelSize.y),
									vec2(-vPixelSize.x,		0),
									vec2(0,					0),
									vec2(vPixelSize.x,		0),
									vec2(-vPixelSize.x,		-vPixelSize.y),
									vec2(0,					-vPixelSize.y),
									vec2(vPixelSize.x,		-vPixelSize.y)
								);

	// Get the pixels around our current pixel.
	vec3 pixelSample[9];
	for (int i = 0; i < 9; i++)
	{
		pixelSample[i] = ( texture2D(tSampler, vTexCoord.st + offsets[i]) );
	}

	{
		// Run through our X-Directional Kernel
		vec3 vSumX = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 9; i++)
		{
			vSumX += (pixelSample[i] * xDirectionKernel[i]);
		}

		// Run through our Y-Directional Kernel
		vec3 vSumY = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 9; i++)
		{
			vSumY += (pixelSample[i] * yDirectionKernel[i]);
		}

		// Finally, the X and Y directional gradient values are combined with the following equation :
		vec3 vTempSum = (vSumX * vSumX) + (vSumY * vSumY);
		vFinalEdge = sqrt(vTempSum);
	}

	return vec4(vFinalEdge, 1.0);
}



