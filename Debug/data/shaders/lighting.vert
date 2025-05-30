	///////////////////////////////////////////////////////////
	// ATTRIBUTES
	///////////////////////////////////////////////////////////
	attribute vec3 	a_normal;

#if defined(BUMPED)
	attribute vec3 a_tangent;	
	attribute vec3 a_binormal;
#endif

	///////////////////////////////////////////////////////////
	// UNIFORMS
	///////////////////////////////////////////////////////////
#if defined(SPECULAR) || (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0)
	uniform mat4 	u_worldMatrix;
	uniform mat4 	u_worldViewMatrix;
	uniform mat4 	u_inverseTransposeWorldMatrix;
	uniform mat4 	u_inverseTransposeWorldViewMatrix;
#endif

#if defined(SPECULAR)
	uniform vec3 	u_cameraPosition;
#endif

#if (POINT_LIGHT_COUNT > 0)
	uniform vec3 	u_pointLightPosition[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
	uniform vec3 	u_spotLightPosition[SPOT_LIGHT_COUNT];
#endif

#if !defined(BUMPED) && (DIRECTIONAL_LIGHT_COUNT > 0)
	uniform vec3	u_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif

	///////////////////////////////////////////////////////////
	// VARYINGS
	///////////////////////////////////////////////////////////
	varying vec3 	v_normal;

#if (POINT_LIGHT_COUNT > 0)
	varying vec3 	v_vertexToPointLightDirection[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
	varying vec3 	v_vertexToSpotLightDirection[SPOT_LIGHT_COUNT];
#endif

#if defined(SPECULAR)
	varying vec3	v_cameraDirection;
#endif
#if defined(BUMPED)
	varying mat3 	mat3_tbnMatrix;
#endif

#if defined(BUMPED) && (DIRECTIONAL_LIGHT_COUNT > 0)
	varying vec3	v_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif
	///////////////////////////////////////////////////////////
	
	vec3 getNormal()
	{
		return a_normal;
	}

#if defined(BUMPED)
	vec3 getTangent()
	{
		return a_tangent;
	}
#endif

void applyLight()
{
	vec3 vPosition = vec4(getVertexPosition() * u_worldMatrix).xyz;
	
	///////////// NORMALS /////////////
	mat3 inverseTransposeWorldMatrix = mat3(u_inverseTransposeWorldMatrix[0].xyz, u_inverseTransposeWorldMatrix[1].xyz, u_inverseTransposeWorldMatrix[2].xyz);
	v_normal = normalize(getNormal() * inverseTransposeWorldMatrix);
	////////////////////////////////////
		
	///////////// POINT LIGHT DIRECTIONS & COLOUR /////////////
	#if (POINT_LIGHT_COUNT > 0)
		for(int i = 0; i < POINT_LIGHT_COUNT; i++)
		{
			v_vertexToPointLightDirection[i] = (u_pointLightPosition[i] - vPosition);
		}
	#endif
	///////////////////////////////////////////////////////////
	
	///////////// SPOT LIGHT DIRECTIONS & COLOUR /////////////
	#if (SPOT_LIGHT_COUNT > 0)
		for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
		{
			v_vertexToSpotLightDirection[i] = (u_spotLightPosition[i] - vPosition);
		}
	#endif
	//////////////////////////////////////////////////////////
	
	///////////// DIRECTIONAL LIGHT DIRECTIONS ///////////////
	#if (DIRECTIONAL_LIGHT_COUNT > 0) && defined(BUMPED)
		for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++)
		{
			v_directionalLightDirection[i] = mat3_tbnMatrix * u_directionalLightDirection[i];
		}
	#endif
	//////////////////////////////////////////////////////////
	
	///////////// BUMPED /////////////
	#if defined(BUMPED)
		vec3 v_tangent = normalize(getTangent() * inverseTransposeWorldMatrix);
		
		// Improving the visual quality by re-orthoganalising it
		// by Gram-Schmitd process
		v_tangent = normalize(v_tangent - dot(v_tangent, v_normal) * v_normal);
		
		vec3 v_bitangent = cross(v_tangent, v_normal);
		
		mat3_tbnMatrix = mat3(v_tangent, v_bitangent, v_normal);
	#endif
	//////////////////////////////////

	///////////// CAMERA DIRECTION /////////////
	#if defined(SPECULAR)
	{
		v_cameraDirection = (u_cameraPosition - vPosition);
	}
	#endif
	////////////////////////////////////////////
}

