#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
in vec3 EPos;
//position of the vertex in camera space

void main()
{
    // You will need to work with these for lighting
    vec3 normal = normalize(fragNor);
    vec3 light = normalize(lightDir);
    vec3 view_direction = normalize(-EPos);

    // Diffuse light
    float diffDot = max(dot(normal, light), 0.0);
    vec3 diff = diffDot * MatDif;

    // Specular light
    vec3 partial_direction = normalize(light + view_direction);
    float specDot = max(dot(partial_direction, normal), 0.0); // Using partial_direction
    float intensity = pow(specDot, MatShine); //matshine is now float

    vec3 specular = intensity * MatSpec;

    color = vec4(MatAmb + diff + specular, 1.0);
}

