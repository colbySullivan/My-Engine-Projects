#pragma once
const char* sunlightShaderSource = R"(
    Texture2D diffuseTexture : register(t0);
    SamplerState diffuseSampler : register(s0);

    cbuffer CameraConstants : register(b2)
    {
        float4x4 WorldToCameraTransform;
        float4x4 CameraToRenderTransform;
        float4x4 RenderToClipTransform;
    };

    cbuffer ModelConstants : register(b3)
    {
        float4x4 ModelToWorldTransform;
        float4   ModelColor;
    };

    struct vs_input_t
    {
        float3 modelSpacePosition : POSITION;
        float4 color : COLOR;
        float2 uv : TEXCOORD;
        float3 normal : NORMAL;
    };

    struct v2p_t
    {
        float4 clipSpacePosition : SV_Position;
        float4 color : COLOR;
        float2 uv : TEXCOORD;
        float3 worldNormal : NORMAL;
    };

    v2p_t VertexMain(vs_input_t input)
    {
        float4 modelSpacePosition  = float4(input.modelSpacePosition, 1);
        float4 worldSpacePosition  = mul( ModelToWorldTransform,  modelSpacePosition  );
        float4 cameraSpacePosition = mul( WorldToCameraTransform,  worldSpacePosition  );
        float4 renderSpacePosition = mul( CameraToRenderTransform, cameraSpacePosition );
        float4 clipSpacePosition   = mul( RenderToClipTransform,   renderSpacePosition );
        
        // Transform normal to world space (using w=0 for direction vectors)
        float4 modelNormal = float4(input.normal, 0);
        float4 worldNormal = mul( ModelToWorldTransform, modelNormal );
        
        v2p_t v2p;
        v2p.clipSpacePosition = clipSpacePosition;
        v2p.color = input.color * ModelColor;
        v2p.uv = input.uv;
        v2p.worldNormal = worldNormal.xyz;
        return v2p;
    }

    float4 PixelMain(v2p_t input) : SV_Target0
    {
        float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
        float4 vertexColor = input.color;
        float4 diffuseColor = textureColor * vertexColor;
        
        // Fixed directional sunlight shining diagonally ESE and downward: {3, 1, -2} normalized
        float3 sunDirection = normalize( float3( 3.0, 1.0, -2.0 ) );
        float3 sunColor = float3( 1.0, 1.0, 1.0 ); // White sunlight
        float ambientStrength = 0.2; // Minimum ambient lighting
        
        // Get normalized world-space normal (interpolated from vertex shader)
        float3 worldNormal = normalize( input.worldNormal );
        
        // Calculate diffuse lighting: dot product of surface normal and direction TO light
        // Sun direction {3,1,-2} points FROM the sun, so we negate it to get pixel-to-light direction
        float3 pixelToLightDir = -sunDirection;
        float diffuseDot = dot( worldNormal, pixelToLightDir );
        
        // Clamp to minimum ambient value (0.2) to avoid completely dark surfaces
        float lightStrength = max( diffuseDot, ambientStrength );
        
        // Apply lighting: multiply diffuse color by light strength and sun color
        float3 litColor = diffuseColor.rgb * lightStrength * sunColor;
        
        // Construct final color with original alpha
        float4 color = float4( litColor, diffuseColor.a );
        
        clip(color.a - 0.01f);
        return color;
    }
)";