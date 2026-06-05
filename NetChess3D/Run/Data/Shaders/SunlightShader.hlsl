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
};

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
    return v2p;
}

float4 PixelMain(v2p_t input) : SV_Target0
{
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
    float4 vertexColor = input.color;
    float4 diffuseColor = textureColor * vertexColor;
        
    // A single fixed directional white sunlight shines diagonally ESE and downward across the board e.g. {3,1,-2} normalized
    float3 sunDirection = normalize( float3( 3.0, 1.0, -2.0 ) );
    float3 sunColor = float3( 1.0, 1.0, 1.0 );
        
    // Get normalized world-space normal (interpolated from vertex shader)
    // Remember to re-normalize the interpolated world-space normal inside the Pixel Shader, since interpolating two valid different normals can result in a non-normalized vector!
    float3 worldTangent = normalize( input.worldTangent );
    float3 worldBitangent = normalize( input.worldBitangent );
    float3 worldNormal = normalize( input.worldNormal );
        
    // pixels are lit based on the dot product of the (interpolated) pixel world-space normal and the pixel-to-light direction vector
    float3 pixelToLightDir = -sunDirection;
    float diffuseDot = dot( worldNormal, pixelToLightDir );
       
    // clamped to some minimum ambient value (e.g. 0.2f)
    float lightStrength = max( diffuseDot, 0.2 );
        
    // multiplied by the light’s color and the surface (chess piece triangle) diffuse color
    float3 litColor = diffuseColor.rgb * lightStrength * sunColor;
        
    // Normal visual testing 
    // litColor = (worldNormal + 1.0) * 0.5;

    float4 color = float4( litColor, diffuseColor.a );
    clip(color.a - 0.01f);
    return color;
   
}