#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 vertexNormal;
in layout(location=3) vec3 vertexColor;

uniform mat4 modelTransformMatrix;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 theColor;
out vec2 UV;

uniform vec3 ambientLight;

out vec3 normalWorld;
out vec3 vertexPositionWorld;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 view_position = viewMatrix * modelTransformMatrix * v;
	vec4 projection_position = projectionMatrix*view_position;
	gl_Position = projection_position;

	vec4 normal_temp = modelTransformMatrix * vec4(vertexNormal,0);
	normalWorld = normal_temp.xyz;
	vertexPositionWorld = (modelTransformMatrix * v).xyz;

	UV = vertexUV;
	theColor = ambientLight;
	
}