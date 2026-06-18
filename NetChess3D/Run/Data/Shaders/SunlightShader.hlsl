Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specGlossEmitTexture : register(t2);
SamplerState diffuseSampler : register(s0);
SamplerState normalSampler : register(s1);
SamplerState specGlossEmitSampler : register(s2);

cbuffer DebugConstants : register(b1)
{
    float   time;
    int     debugInt;
    float   debugFloat;
    float   padding;
};

cbuffer CameraConstants : register(b2)
{
    float4x4 WorldToCameraTransform;
    float4x4 CameraToRenderTransform;
    float4x4 RenderToClipTransform;
    float3   cameraWorldPos;
    float    camera_padding;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 ModelToWorldTransform;
    float4   ModelColor;
};

cbuffer GameConstants : register(b8)
{
	int		effectsInt;
	int		padding2[3];
};

struct vs_input_t
{
    float3 modelSpacePosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 normal : NORMAL;
};

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

float3 encodeXYZToRGB8( float3 xyzVec )
{
    return ( xyzVec + 1.0 ) * 0.5;
}

float3 decodeRGB8ToXYZ( float3 color )
{
    return ( color * 2.0 ) - 1.0;
}

float3 SmoothStart3( float3 v )
{
	return v * v * v;
}

float3 SmoothStop3( float3 v )
{
	float3 inv = 1.0 - v;
	return 1.0 - (inv * inv * inv);
}

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

    // A single fixed directional white sunlight shines diagonally ESE and downward across the board e.g. {3,1,-2} normalized
    float3 sunDirection = normalize( float3( 3.0, 1.0, -2.0 ) );
    float3 sunColor = float3( 1.0, 1.0, 1.0 );
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
        
    // multiplied by the light’s color and the surface (chess piece triangle) diffuse color
    float3 litColor = diffuseColor.rgb * lightStrength * sunColor + specularLight;

    // emissivity = 1.0 looks glowing
    litColor += emissivity * diffuseColor.rgb;

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

    float4 outColor = float4( litColor, diffuseColor.a );
    clip(outColor.a - 0.01f);
    
    // Special effects -----------------------------------------------------------------------
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