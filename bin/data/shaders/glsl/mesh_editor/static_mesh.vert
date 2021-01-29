#version 450

layout(location=0) in vec3 position;

layout(set=0, binding=0) uniform Camera {
  mat4 Projection;
  mat4 View;
};

void main()
{
  gl_Position = Projection * View * vec4(position, 1.0f);
}