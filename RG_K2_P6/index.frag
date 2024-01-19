#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = texColor;
    
    // Use the alpha value for transparency
    if (texColor.a < 0.1) // You may need to adjust this threshold based on your image
        discard;
}
