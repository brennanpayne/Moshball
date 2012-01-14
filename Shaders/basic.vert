#version 400

uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec3 vertex_position;

out vec3 model_coordinates;

void main()
{
	model_coordinates = vertex_position;
	gl_Position = projection_matrix * modelview_matrix * vec4(vertex_position,1);
}
