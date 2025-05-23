#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec3 aTexCoords;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
flat out ivec4 BoneIDs;
out vec4 Weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 inv;

#define MAX_BONES 1000
uniform mat4 Bones[MAX_BONES];


void main()
{
    // Bone skinning
    mat4 BoneTransform = Bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += Bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += Bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += Bones[aBoneIDs[3]] * aWeights[3];

    vec4 skinnedPos = BoneTransform * vec4(aPos, 1.0);

    // Final transforms
    vec4 worldPos = model * skinnedPos;
    FragPos = vec3(worldPos);
    gl_Position = proj * view * worldPos;

    // Outputs
    Normal = mat3(transpose(inv)) * aNormal;

    TexCoords = vec2(aTexCoords.x, aTexCoords.y);
    BoneIDs = aBoneIDs;
    Weights = aWeights;
}