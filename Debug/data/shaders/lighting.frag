
///////////////////////////////////////////////////////////
// UNIFORMS
///////////////////////////////////////////////////////////
uniform vec3 		u_ambientColor;
uniform float 		u_specularExponent;

uniform sampler2D 	u_normalmapTexture;

#if (POINT_LIGHT_COUNT > 0)
uniform vec3 		u_pointLightColour[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
uniform vec3 		u_spotLightDirection[SPOT_LIGHT_COUNT];
uniform vec3 		u_spotLightColour[SPOT_LIGHT_COUNT];
uniform float 		u_spotLightInnerAngleCos[SPOT_LIGHT_COUNT];
uniform float 		u_spotLightOuterAngleCos[SPOT_LIGHT_COUNT];
#endif

///////////////////////////////////////////////////////////
// VARYINGS
///////////////////////////////////////////////////////////
varying vec3 		v_normal;

#if (POINT_LIGHT_COUNT > 0)
varying vec3 		v_vertexToPointLightDirection[POINT_LIGHT_COUNT];
varying vec3 		v_pointLightColour[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
varying vec3 		v_vertexToSpotLightDirection[SPOT_LIGHT_COUNT];
varying vec3 		v_spotLightColour[SPOT_LIGHT_COUNT];
#endif

#if defined(SPECULAR)
varying vec3		v_cameraDirection;
#endif
///////////////////////////////////////////////////////////

vec3 computeLighting(vec3 vNormal, vec3 vLightDirection, vec3 vlightColour, float fAttenuation)
{
	vec3 vComputedColour = vec3(0.0f, 0.0f, 0.0f);
	
	// Diffuse component
	{
		float fDiffuseContribution = max( dot(vNormal, vLightDirection), 0.0 );
		vec3 vDiffuseColor = fDiffuseContribution * (_baseColor.rgb * vlightColour);
		
		vComputedColour += vDiffuseColor * fAttenuation;
	}
	
	// Specular component
	#if defined(SPECULAR)
	{
		// My - Does not take into account the camera.
		//float fSpecularAngle = clamp(dot(vNormal, vLightDirection), 0.0, 1.0);
		//float fSpecularContribution = pow(fSpecularAngle, u_specularExponent);
		//vec3 vSpecularColor = fSpecularContribution;
		
		// OR - Blinn-Phong shading (Gameplay)
		//vec3 vVertexToEye = normalize(v_cameraDirection);
		//vec3 vHalfVector = normalize(vLightDirection + vVertexToEye);
		//float fSpecularAngle = clamp(dot(vNormal, vHalfVector), 0.0, 1.0);
		//float fSpecularContribution = pow(fSpecularAngle, u_specularExponent);
		//vec3 vSpecularColor = fSpecularContribution;

		// Phong shading (Gameplay)
		//vec3 vertexToEye = normalize(v_cameraDirection);
		//vec3 fSpecularAngle = normalize(vNormal * fDiffuseContribution * 2.0 - vLightDirection);  
		//vec3 vSpecularColor = vec3(pow(clamp(dot(fSpecularAngle, vertexToEye), 0.0, 1.0), u_specularExponent)); 

		//OR - internet	
		vec3 incidenceVector = -vLightDirection; 					// a unit vector
		vec3 reflectionVector = reflect(incidenceVector, vNormal); 	// also a unit vector
		vec3 surfaceToCamera = normalize(v_cameraDirection); 		// also a unit vector
		float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
		float fSpecularContribution = pow(cosAngle, u_specularExponent);
		vec3 vSpecularColor = fSpecularContribution;
	
		// OR - cppgug
		//vec3 surf2light = normalize(vLightDirection);
		//vec3 surf2camera = normalize(v_cameraDirection);
		//vec3 reflection = -reflect(surf2camera, vNormal);
		//float specularContribution = pow(max(0.0,dot(reflection, surf2light)), 4.0);
		//vec3 vSpecularColor = specularContribution;

		vComputedColour += vSpecularColor * fAttenuation;
	}
	#endif
	
	return vComputedColour;
}

vec3 getLitPixel()
{
	vec3 vNormal = normalize(v_normal);
	vec3 vFinalLitRGB;
	
	// Ambient component
	{
		vec3 vAmbientColor = _baseColor.rgb * u_ambientColor;
		vFinalLitRGB = vAmbientColor;
	}
	
	// Point light contribution
	#if (POINT_LIGHT_COUNT > 0)
		for(int i = 0; i < POINT_LIGHT_COUNT; i++)
		{
			vec3 vVertexToPointLightDirection = normalize(v_vertexToPointLightDirection[i]);
			
			// Apply distance 'Attenuation' component
			float fDistance = length(vVertexToPointLightDirection);
			float fAttenuation = 1.0 / (1.0 + 0.1*fDistance + 0.01*fDistance*fDistance);
			fAttenuation *= fAttenuation;

			vFinalLitRGB += computeLighting(vNormal, vVertexToPointLightDirection, u_pointLightColour[i], fAttenuation);
		}
	#endif
	
	// Spot light contribution
	#if (SPOT_LIGHT_COUNT > 0)
		for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
		{
			vec3 vVertexToSpotLightDirection = normalize(v_vertexToSpotLightDirection[i]);
			vec3 vSpotLightDirection = normalize(u_spotLightDirection[i] * 2.0);

			// Apply distance 'Attenuation' component
			float fDistance = length(vVertexToSpotLightDirection);
			//float fAttenuation = 1.0 / (1.0 + 0.1*fDistance + .01*fDistance*fDistance);
			float fAttenuation = clamp(1.0 - fDistance/3.0f, 0.0, 1.0f);
			fAttenuation *= fAttenuation;

			// "-lightDirection" is used because light direction points in opposite direction to spot direction.
			float spotCurrentAngleCos = dot(vSpotLightDirection, -vVertexToSpotLightDirection);

			// Apply spot 'Attenuation'
			fAttenuation *= smoothstep(u_spotLightOuterAngleCos[i], u_spotLightInnerAngleCos[i], spotCurrentAngleCos);

			vFinalLitRGB += computeLighting(vNormal, vVertexToSpotLightDirection, u_spotLightColour[i], fAttenuation);
		}
	#endif

	return vFinalLitRGB;
}