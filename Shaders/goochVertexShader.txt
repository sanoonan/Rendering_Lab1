#version 400

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;






uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

out vec3 direction_to_light_eye;
out vec4 position_eye;


void main(){



	mat4 ModelViewMatrix = view * model;
	mat3 NormalMatrix =  mat3(ModelViewMatrix);
	// Convert normal and position to eye coords
	// Normal in view space
	direction_to_light_eye = normalize( NormalMatrix * vertex_normal);
	// Position in view space
	position_eye = ModelViewMatrix * vec4(vertex_position,1.0);



	// Convert position to clip coordinates and pass along
	gl_Position = proj * view * model * vec4(vertex_position, 1.0);


}


  