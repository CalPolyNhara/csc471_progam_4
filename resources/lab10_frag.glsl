#version 330 core

uniform sampler2D alphaTexture; // Texture containing alpha values

in vec3 partCol; // Particle color
out vec4 outColor;

void main()
{
    // Sample the alpha texture at the coordinate of the particle
    float alpha = texture(alphaTexture, gl_PointCoord).r;

    // Output color with the sampled alpha
    outColor = vec4(partCol, alpha);
}
