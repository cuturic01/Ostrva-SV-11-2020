#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float rotationSpeed;
uniform vec3 rotationPoint;

void main()
{
    mat4 scaledModel = model * mat4(vec4(0.3, 0.0, 0.0, 0.0),
                                    vec4(0.0, 0.3, 0.0, 0.0),
                                    vec4(0.0, 0.0, 0.3, 0.0),
                                    vec4(0.0, 0.0, 0.0, 1.0));

    float angle = time * rotationSpeed;
    mat4 rotationMatrix = mat4(cos(angle), 0.0, sin(angle), 0.0,
                               0.0, 1.0, 0.0, 0.0,
                               -sin(angle), 0.0, cos(angle), 0.0,
                               -sin(angle), 0.0, cos(angle), 1.0);

     mat4 translationToOrigin = mat4(vec4(1.0, 0.0, 0.0, 0.0),
                                    vec4(0.0, 1.0, 0.0, 0.0),
                                    vec4(0.0, 0.0, 1.0, 0.0),
                                    vec4(-rotationPoint.x, -rotationPoint.y, -rotationPoint.z, 1.0));

    mat4 translationBack = mat4(vec4(1.0, 0.0, 0.0, 0),
                                vec4(0.0, 1.0, 0.0, 0),
                                vec4(0.0, 0.0, 1.0, 0),
                                vec4(rotationPoint.x, rotationPoint.y, rotationPoint.z, 1.0));

    mat4 finalModel = translationBack * rotationMatrix * translationToOrigin * scaledModel;



    FragPos = vec3(finalModel * vec4(inPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * inNormal;  
    TexCoords = inTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}