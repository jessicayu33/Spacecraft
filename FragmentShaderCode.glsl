#version 430
out vec4 daColor;

in vec3 theColor;
in vec2 UV;

uniform sampler2D myTextureSampler;

in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

//user control
uniform float brightness_diffuse;
uniform float brightness_specular;
void main()
{
	vec3 color = texture(myTextureSampler,UV).rgb;

	//difuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness,brightness,brightness,1.0) * vec4(brightness_diffuse,brightness_diffuse,brightness_diffuse,1.0);

	//specular
	// cal reflect light direction
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld,normalWorld);
	// cal direction from eye to object
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	// cal light brightness regrads to angle b/tw eye and reflect light
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld),0,1);
	// control lobe of specular light
	s = pow(s,50);
	vec4 specularLight = s * brightness_specular * vec4(1.0,1.0,1.0,1.0);

	//final color
	daColor = vec4(color,1.0)*theColor  +  vec4(color,1.0)*clamp(diffuseLight,0,1) + clamp(specularLight,0,1) ;

}