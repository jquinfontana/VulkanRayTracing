
#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "raycommon.glsl"
#include "wavefront.glsl"
#include "random.glsl"

hitAttributeEXT vec2 attribs;

// clang-format off
layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;

layout(buffer_reference, scalar) buffer Vertices {Vertex v[]; }; // Positions of an object
layout(buffer_reference, scalar) buffer Indices {ivec3 i[]; }; // Triangle indices
layout(buffer_reference, scalar) buffer Materials {WaveFrontMaterial m[]; }; // Array of all materials on an object
layout(buffer_reference, scalar) buffer MatIndices {int i[]; }; // Material ID for each triangle
layout(set = 0, binding = eTlas) uniform accelerationStructureEXT topLevelAS;
layout(set = 1, binding = eObjDescs, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;
layout(set = 1, binding = eTextures) uniform sampler2D textureSamplers[];

layout(push_constant) uniform _PushConstantRay { PushConstantRay pcRay; };
// clang-format on


void main() {
  //Object data-------------------------------------------------------------------------------------------
  ObjDesc    objResource = objDesc.i[gl_InstanceCustomIndexEXT];
  MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
  Materials  materials   = Materials(objResource.materialAddress);
  Indices    indices     = Indices(objResource.indexAddress);
  Vertices   vertices    = Vertices(objResource.vertexAddress);
  // Indices of the triangle
  ivec3 ind = indices.i[gl_PrimitiveID];
  // Vertex of the triangle
  Vertex v0 = vertices.v[ind.x];
  Vertex v1 = vertices.v[ind.y];
  Vertex v2 = vertices.v[ind.z];
  const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
  // Computing the coordinates of the hit position
  const vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
  const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));  // Transforming the position to world space
  // Computing the normal at hit position
  const vec3 nrm      = v0.nrm * barycentrics.x + v1.nrm * barycentrics.y + v2.nrm * barycentrics.z;
  const vec3 worldNrm = normalize(vec3(nrm * gl_WorldToObjectEXT));  // Transforming the normal to world space
  // Vector toward the light
  vec3  L;
  float lightIntensity = pcRay.lightIntensity;
  float lightDistance  = 100000.0;
  // Point light
  if(pcRay.lightType == 0) {
    vec3 lDir      = pcRay.lightPosition - worldPos;
    lightDistance  = length(lDir);
    lightIntensity = pcRay.lightIntensity / (lightDistance * lightDistance);
    L              = normalize(lDir);
  }
  // Directional light
  else {
    L = normalize(pcRay.lightPosition);
  }
  // Material of the object
  int               matIdx = matIndices.i[gl_PrimitiveID];
  WaveFrontMaterial mat    = materials.m[matIdx];
  // Texture
  vec4 texColor = vec4(1);
  if(mat.textureId >= 0) {
	uint txtId    = mat.textureId + objDesc.i[gl_InstanceCustomIndexEXT].txtOffset;
	vec2 texCoord = v0.texCoord * barycentrics.x + v1.texCoord * barycentrics.y + v2.texCoord * barycentrics.z;
	texColor = texture(textureSamplers[nonuniformEXT(txtId)], texCoord);
  }
  //--------------------------------------------------------------------------------------------------------


  if(length(mat.emission) > 0) {
    prd.hitValue = 3*mat.emission;
    prd.done = 1;

  } else {
	  float reflectProb;
    switch (mat.illum) {
    case 5: //metal
        prd.hitValue = mat.specular * texColor.rgb;
        prd.done = 0;//isScattered ? 1 : 0;
        prd.rayDir = reflect(prd.rayDir, worldNrm) + 0.0*RandomInUnitSphere(prd.RandomSeed); //editar parametro para fuzzy material
        prd.rayOrigin = worldPos;
        
        break;
    case 7:	//dielectric
        const float dot = dot(prd.rayDir, worldNrm);
        const vec3 outwardNormal = dot > 0 ? -worldNrm : worldNrm;
        const float niOverNt = dot > 0 ? mat.ior : 1 / mat.ior;
        const float cosine = dot > 0 ? mat.ior * dot : -dot;
        const vec3 refracted = refract(prd.rayDir, outwardNormal, niOverNt);
        reflectProb = refracted != vec3(0) ? Schlick(cosine, mat.ior) : 1; //total internal refraction
        prd.hitValue = mat.specular * texColor.rgb;
        prd.done = 0;
        prd.rayDir = rnd(prd.RandomSeed) < reflectProb
          ? reflect(prd.rayDir, worldNrm)
          : refracted;
        prd.rayOrigin = worldPos;
        
        break;
    default: //lambetian and glossy (se modula con respecto al coeficiente specular)
        //const bool isScattered = dot(prd.rayDir, worldNrm) < 0;
        reflectProb = max(max(mat.specular.x, mat.specular.y),  mat.specular.z);
        prd.hitValue = mat.diffuse.rgb * texColor.rgb;
        prd.done = 0;//isScattered ? 1 : 0;
        prd.rayDir = rnd(prd.RandomSeed) < reflectProb
          ? reflect(prd.rayDir, worldNrm) + (350/mat.shininess)*RandomInUnitSphere(prd.RandomSeed)
          : worldNrm + RandomInUnitSphere(prd.RandomSeed);
        prd.rayOrigin = worldPos;
    }
  }

  //-------Luz direccional
  //   float tMin   = 0.001;
  //   float tMax   = 10000;
  //   vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
  //   vec3  rayDir = normalize(vec3(-1,3,-1));
  //   uint  flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
  //   isShadowed   = true;
  //   traceRayEXT(topLevelAS,  // acceleration structure
  //               flags,       // rayFlags
  //               0xFF,        // cullMask
  //               0,           // sbtRecordOffset
  //               0,           // sbtRecordStride
  //               1,           // missIndex
  //               origin,      // ray origin
  //               tMin,        // ray min range
  //               rayDir,      // ray direction
  //               tMax,        // ray max range
  //               1            // payload (location = 1)
  //   );
  // if(!isShadowed){
  //   vec4 texColor = vec4(1);
  //   if(mat.textureId >= 0) {
  //     uint txtId    = mat.textureId + objDesc.i[gl_InstanceCustomIndexEXT].txtOffset;
  //     vec2 texCoord = v0.texCoord * barycentrics.x + v1.texCoord * barycentrics.y + v2.texCoord * barycentrics.z;
  //     texColor = texture(textureSamplers[nonuniformEXT(txtId)], texCoord);
  //   }
  //   prd.hitValue = mat.diffuse.rgb * texColor.rgb;
  //   prd.done = 1;
  // }


}
