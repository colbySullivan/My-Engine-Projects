Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specGlossEmitTexture : register(t2);
SamplerState diffuseSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState specGlossEmitSampler : register(s2);

//------------------------------------------------------------------------------------------------
cbuffer DebugConstants : register(b1)
{
    float   time;
    int     debugInt;
    float   debugFloat;
    float   padding;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 WorldToCameraTransform;
    float4x4 CameraToRenderTransform;
    float4x4 RenderToClipTransform;
    float3   cameraWorldPos;
    float    camera_padding;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
    float4x4 ModelToWorldTransform;
    float4   ModelColor;
};

//------------------------------------------------------------------------------------------------
cbuffer GameConstants : register(b8)
{
	int		effectsInt;
	int		padding2[3];
};

//------------------------------------------------------------------------------------------------
struct s_Light
{
    float4  color;
    float3  position;
    float   ambience;
    float   minRadius;
    float   maxRadius;
    float   innerConeDotThreshold;
    float   outerConeDotThreshold;
    float4  colorAndIntensity;
    float3  forwardNormal;
    float   paddingLight;
};

//------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 8
cbuffer LightConstants : register(b4)
{
    s_Light     lights[MAX_LIGHTS];
    float3      sunDir;
    int	        numLights;
    float4      sunColorConstant;
};

//------------------------------------------------------------------------------------------------
struct vs_input_t
{
    float3 modelSpacePosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 normal : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct v2p_t
{
    float4 clipSpacePosition : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;
    float3 worldBitangent : BITANGENT;
    float3 worldNormal : NORMAL;
    float3 worldPos : WORLD_POSITION;
};

//------------------------------------------------------------------------------------------------
float3 encodeXYZToRGB8( float3 xyzVec )
{
    return ( xyzVec + 1.0 ) * 0.5;
}

//------------------------------------------------------------------------------------------------
float3 decodeRGB8ToXYZ( float3 color )
{
    return ( color * 2.0 ) - 1.0;
}

//------------------------------------------------------------------------------------------------
float3 SmoothStart3( float3 v )
{
	return v * v * v;
}

//------------------------------------------------------------------------------------------------
float3 SmoothStop3( float3 v )
{
	float3 inv = 1.0 - v;
	return 1.0 - (inv * inv * inv);
}

//-----------------------------------------------------------------------------------------------
float InterpolateFloat(float start, float end, float fractionTowardEnd)
{
	return start + (fractionTowardEnd * (end - start));
}

//-----------------------------------------------------------------------------------------------
float3 Interpolate( float3 start, float3 end, float fractionTowardEnd )
{
	return float3(
		InterpolateFloat( start.x, end.x, fractionTowardEnd ),
		InterpolateFloat( start.y, end.y, fractionTowardEnd ),
		InterpolateFloat( start.z, end.z, fractionTowardEnd )
	);
}

//-----------------------------------------------------------------------------------------------
float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
    float range = rangeEnd - rangeStart;
    float fraction = 0.5f;
    if (range != 0.0f)
    {
        fraction = (value - rangeStart) / range;
    }
    return fraction;
}

//-----------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    return InterpolateFloat(outStart, outEnd, GetFractionWithinRange(inValue, inStart, inEnd));
}

//------------------------------------------------------------------------------
float SmoothStop2( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStart2( float t )
{
	return t * t;
}


//------------------------------------------------------------------------------
float SmoothStep3( float t )
{
	return InterpolateFloat( SmoothStart2(t), SmoothStop2(t), t );
}

//------------------------------------------------------------------------------------------------
v2p_t VertexMain(vs_input_t input)
{
    float4 modelSpacePosition  = float4(input.modelSpacePosition, 1);
    float4 worldSpacePosition  = mul( ModelToWorldTransform,  modelSpacePosition  );
    float4 cameraSpacePosition = mul( WorldToCameraTransform,  worldSpacePosition  );
    float4 renderSpacePosition = mul( CameraToRenderTransform, cameraSpacePosition );
    float4 clipSpacePosition   = mul( RenderToClipTransform,   renderSpacePosition );

    float4 modelNormal = float4(input.normal, 0);
    float4 worldNormal = mul( ModelToWorldTransform, modelNormal );

    float4 modelTangent = float4(input.tangent, 0);
    float4 worldTangent = mul( ModelToWorldTransform, modelTangent );

    float4 modelBitangent = float4(input.bitangent, 0);
    float4 worldBitangent = mul( ModelToWorldTransform, modelBitangent );
        
    v2p_t v2p;
    v2p.clipSpacePosition = clipSpacePosition;
    v2p.color = input.color * ModelColor;
    v2p.uv = input.uv;
    v2p.worldNormal = worldNormal.xyz;
    v2p.worldTangent = worldTangent.xyz;
    v2p.worldBitangent = worldBitangent.xyz;
    v2p.worldPos = worldSpacePosition.xyz;
    return v2p;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(v2p_t input) : SV_Target0
{
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
    float4 normalColor = normalTexture.Sample(normalSampler, input.uv);
    float4 vertexColor = input.color;
    float4 diffuseColor = textureColor * vertexColor;

    float4 specGlossEmitColor = specGlossEmitTexture.Sample( specGlossEmitSampler, input.uv );
    float specularity = specGlossEmitColor.r;
    float glossiness = specGlossEmitColor.g;
    float emissivity = specGlossEmitColor.b;

    // Decode tangent space normal map
    float3 normalColorToXYZ = normalize( decodeRGB8ToXYZ( normalColor.rgb ) );


    float3 totalDiffuseLight = float3( 0.f, 0.f, 0.f );
    float3 totalSpecularLight = float3( 0.f, 0.f, 0.f );

    //------------------------------------------------------------------------------------------------	
	// Sun
	//------------------------------------------------------------------------------------------------	
    // A single fixed directional white sunlight shines diagonally ESE and downward across the board e.g. {3,1,-2} normalized
    // float3 sunDirection = normalize( float3( 3.0, 1.0, -2.0 ) );
    float3 sunDirection = normalize( sunDir );
    float3 sunColor = sunColorConstant.rgb;
    float3 pixelToLightDir = -sunDirection;
        
     // Get normalized world-space normal (interpolated from vertex shader)
    // Remember to re-normalize the interpolated world-space normal inside the Pixel Shader, since interpolating two valid different normals can result in a non-normalized vector!
    float3 worldTangent = normalize( input.worldTangent );
    float3 worldBitangent = normalize( input.worldBitangent );
    float3 worldNormal = normalize( input.worldNormal );
        
    // TBN-To-World transformation matrix construction
    // HLSL matrices are row-major by default
    float3x3 TBNToWorld = float3x3( worldTangent, worldBitangent, worldNormal );
    float3 perPixelNormalToWorld = normalize( mul( normalColorToXYZ, TBNToWorld ) );

    // pixels are lit based on the dot product of the (interpolated) pixel world-space normal and the pixel-to-light direction vector
    float surfaceDot = dot( worldNormal, pixelToLightDir );
    float normalMapDot = dot( perPixelNormalToWorld, pixelToLightDir );
        
    // clamped to some minimum ambient value (e.g. 0.2f)
    float lightStrength = max( normalMapDot, 0.2 );
    totalDiffuseLight += lightStrength * sunColor;

    // get normal pixel to camera
    float3 pixelToCameraDir = normalize( cameraWorldPos - input.worldPos );

    // ideal sun reflection
    float3 halfVec = normalize( pixelToLightDir + pixelToCameraDir );

    // specular dot, raise to exponent derived from glossiness [0,1] -> power [1,32]
    float specDot = saturate( dot( halfVec, perPixelNormalToWorld ) );

    // higher range 1-32 for more control
    // 0 = chalky surface
    // 32 = mirror surface
    float specularRange = lerp( 1.0f, 32.0f, glossiness );

    // strength falls off when Specular angle increases multiplying glossiness scale intentensity down with rough surfaces
    float specStrength = glossiness * pow( specDot, specularRange );

    float3 specularLight = specStrength * specularity * sunColor;
    totalSpecularLight += specularLight;

    // emissivity = 1.0 looks glowing
    float3 emissiveLight = emissivity * diffuseColor.rgb;

     //------------------------------------------------------------------------------------------------	
	// Non-sun lights
	//------------------------------------------------------------------------------------------------	
	for( int lightIndex = 0; lightIndex < numLights; ++ lightIndex )
    {
        float   ambience = lights[lightIndex].ambience;
        float3  position = lights[lightIndex].position;
        float   minRadius = lights[lightIndex].minRadius;
        float   maxRadius = lights[lightIndex].maxRadius;
        float   innerConeDotThreshold = lights[lightIndex].innerConeDotThreshold;
        float   outerConeDotThreshold = lights[lightIndex].outerConeDotThreshold;
        float3  color = lights[lightIndex].colorAndIntensity.rgb;
		float   brightness = lights[lightIndex].colorAndIntensity.a;

        float3 pixelToLight = normalize( position - input.worldPos );
        float distanceToLight = length( position - input.worldPos );
        
        // saturate insures 0-1 
        // ease into smooth s-curve
        float pixelPosFallOf = saturate( RangeMap( distanceToLight, minRadius, maxRadius, 1.f, 0.f ) );
        pixelPosFallOf = SmoothStep3(pixelPosFallOf);

        // pixel alignment with light point direction
        float pixelAlignment = dot(lights[lightIndex].forwardNormal, -pixelToLight);
        float penumbra = saturate( RangeMap( pixelAlignment, outerConeDotThreshold, innerConeDotThreshold, 0.f, 1.f ) );
        penumbra = SmoothStep3( penumbra );
        
        // How closely the surface faces the light
        float surfaceDot = saturate( RangeMap( dot( pixelToLight, perPixelNormalToWorld), -ambience, 1.f, 0.f, 1.f ) );
        
        float lightStrength = penumbra * pixelPosFallOf * brightness * surfaceDot;
        
        // Additive light
		float3 diffuseLight = lightStrength * color;
		totalDiffuseLight += diffuseLight;
    }

    // multiplied by the light’s color and the surface (chess piece triangle) diffuse color
    // float3 litColor = (saturate(totalDiffuseLight) * diffuseColor.rgb) * lightStrength * sunColor + specularLight;
    // float3 litColor = diffuseColor.rgb * ( lightStrength * sunColor + totalDiffuseLight ) + specularLight;
    float3 litColor = ( saturate( totalDiffuseLight ) * diffuseColor.rgb ) + totalSpecularLight + emissiveLight;

    // -----------------------------------------------------------------------
    // Debug view modes
    // -----------------------------------------------------------------------

    // 1. Diffuse map texel only
    if (debugInt == 1)
    {
        litColor = textureColor.rgb;
    }

    // 2. Normal map texel only
    if (debugInt == 2)
    {
        litColor = normalColor.rgb;
    }

    // 3. UV as Red, Green (with Blue=0)
    if (debugInt == 3)
    {
        litColor = float3(input.uv.x, input.uv.y, 0.0);
    }

    // 4. World-space surface (vertex) Tangents
    if (debugInt == 4)
    {
        litColor = encodeXYZToRGB8( worldTangent );
    }
    
    // 5. World-space surface (vertex) Bitangents
    if (debugInt == 5)
    {
        litColor = encodeXYZToRGB8( worldBitangent );
    }
    
    // 6. World-space surface (vertex) Normals
    if (debugInt == 6)
    {
        litColor = encodeXYZToRGB8( worldNormal );
    }
    
    // 7. World-space normal map normals (TBN -> World converted)
    if (debugInt == 7)
    {
        litColor = encodeXYZToRGB8( perPixelNormalToWorld );
    }
    
    // 8. Surface-based lighting only (Ignore normal map, diffuse texture, and vertex colors)
    if (debugInt == 8)
    {
       float surfaceLight = max( surfaceDot, 0.2 );
       litColor = float3( surfaceLight, surfaceLight, surfaceLight );
    }
    
    // 9. Normal-mapped lighting only (Ignore diffuse texture and vertex colors)
    if (debugInt == 9)
    {
        float normalMapLight = max( normalMapDot, 0.2 );
        litColor = float3( normalMapLight, normalMapLight, normalMapLight );
    }


    //------------------------------------------------------------------------------------------------	
	// Final output color
	//------------------------------------------------------------------------------------------------		
    float4 outColor = float4( litColor, diffuseColor.a );
    clip(outColor.a - 0.01f);
    

    // -----------------------------------------------------------------------
    // Special effects
    // -----------------------------------------------------------------------
    if ( effectsInt == 1 )
	{
		float t = 0.5f + ( 0.5f * sin( 5.f * time ) );
		outColor.rgb = lerp( SmoothStart3( outColor.rgb), SmoothStop3( outColor.rgb ), t );
	}

    if ( effectsInt == 2 )
	{
		outColor.a = outColor.a * 0.5f;
	}


    return outColor;
}