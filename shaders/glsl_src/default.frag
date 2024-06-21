#version 460
#include "ubo.glsl"
#include "lighting.glsl"
#include "brdf.glsl"

struct SMaterial
{
    vec4 Albedo;
    float Roughness;
    float Metallic;
    float AO;
};

layout(push_constant) uniform constants
{
    layout(offset = 64) vec4 ColorMask;
    layout(offset = 80) float RoughnessMultiplier;
} 
PushConstants;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec4 WorldPosition;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;

layout(binding = 2) uniform sampler2D TransmittanceLUT;
layout(binding = 3) uniform sampler2D IrradianceLUT;
layout(binding = 4) uniform sampler3D InscatteringLUT;
layout(binding = 5) uniform sampler2D AlbedoMap;
layout(binding = 6) uniform sampler2D NormalMap;
layout(binding = 7) uniform sampler2D RoughnessMap;
layout(binding = 8) uniform sampler2D MetallnessMap;
layout(binding = 9) uniform sampler2D AOMap;

layout(location = 0) out vec4 outColor;

vec3 PointRadiance(vec3 Sun, vec3 Eye, vec3 Point)
{
    vec3 View = normalize(Point - Eye);
    const float Rp = length(Point);
    const float DotPL = dot(Point, Sun) / Rp;

    return GetTransmittanceWithShadow(TransmittanceLUT, Rp, DotPL) * MaxLightIntensity / PI;
}

// course-notes-moving-frostbite-to-pbr-v2
vec3 GetDiffuseTerm(vec3 Albedo, float NdotL, float NdotV, float LdotH, float Roughness)
{
    float EnergyBias = mix(0.0, 0.5, Roughness);
    float EnergyFactor = mix(1.0, 1.0 / 1.51, Roughness);
    vec3 F90 = vec3(EnergyBias + 2.0 * LdotH * LdotH * Roughness);
    vec3 F0 = vec3(1.0);

    float LightScatter = FresnelSchlick(NdotL, F0, F90).r;
    float ViewScatter = FresnelSchlick(NdotV, F0, F90).r;

    return Albedo * vec3(LightScatter * ViewScatter * EnergyFactor);
}

vec3 DirectSunlight(vec3 Eye, vec3 Point, vec3 V, vec3 L, vec3 N, in SMaterial Material)
{
    vec3 H = normalize(V + L);

    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));
    float LdotH = saturate(dot(L, H));
    float HdotV = saturate(dot(H, V));

    vec3 F0 = mix(vec3(0.04), Material.Albedo.rgb, Material.Metallic);
    vec3 F = FresnelSchlick(HdotV, F0);
    float G = GeometrySmith(NdotV, NdotL, Material.Roughness);
    float D = DistributionGGX(NdotH, Material.Roughness);

    vec3 kD = vec3(1.0 - Material.Metallic) * vec3(1.0 - F);
    vec3 specular = vec3(max((D * G * F) / (4.0 * NdotV * NdotL), 0.001));
    vec3 diffuse = kD * GetDiffuseTerm(Material.Albedo.rgb, NdotL, NdotV, LdotH, Material.Roughness);
    //vec3 diffuse = kD * Material.Albedo.rgb;
    vec3 ambient = Material.AO * vec3(0.03) * vec3(1.0 - Material.Metallic) * Material.Albedo.rgb;
    vec3 radiance = PointRadiance(L, Eye, Point);

    return (ambient + (specular + diffuse) * NdotL) * radiance;
}

void main()
{
    vec3 Point = WorldPosition.xyz + vec3(0.0, Rg, 0.0);
    vec3 Eye = View.CameraPosition.xyz + vec3(0.0, Rg, 0.0);
    vec3 NormalMap = normalize(texture(NormalMap, UV).xyz * 2.0 - 1.0);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    N = normalize(TBN * NormalMap);
    vec3 V = normalize(View.CameraPosition.xyz - WorldPosition.xyz);
    vec3 L = normalize(ubo.SunDirection.xyz);
    
    SMaterial Material;
    Material.Roughness = max(PushConstants.RoughnessMultiplier * texture(RoughnessMap, UV).r, 0.01);
    Material.Metallic = texture(MetallnessMap, UV).r;
    Material.AO = texture(AOMap, UV).r;
    Material.Albedo = texture(AlbedoMap, UV);
    Material.Albedo.rgb = pow(PushConstants.ColorMask.rgb * Material.Albedo.rgb, vec3(2.2));

    vec3 Lo = DirectSunlight(Eye, Point, V, L, N, Material);
    outColor = vec4(Lo, PushConstants.ColorMask.a * Material.Albedo.a);
}