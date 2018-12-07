// change version!!!
#version 450
out vec4 daColor;

in vec3 theColor;
in vec2 UV;

uniform sampler2D myTextureSampler;
uniform sampler2D myTextureSampler2;

in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPositionWorld;
uniform vec3 lightPositionWorld2;
uniform vec3 eyePositionWorld;

//user control
uniform float brightness_diffuse;
uniform float brightness_specular;
uniform float brightness_specular2;

uniform bool normalMapping_flag;

void main()
{
	// for normal mapping
	vec3 normal = normalize(normalWorld);
	if (normalMapping_flag) {
		// obtain normal from normal map in range [0, 1]
		normal = texture(myTextureSampler2, UV).rgb;
		// transform normal vector to range [-1, 1]
		normal = normalize(normal * 2.0 - 1.0);
	}

	// texture color
	vec3 color = texture(myTextureSampler,UV).rgb;

	// diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normal);
	vec4 diffuseLight = vec4(brightness,brightness,brightness,1.0) * vec4(brightness_diffuse,brightness_diffuse,brightness_diffuse,1.0);

	// specular
	// cal reflect light direction
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normal);
	// cal direction from eye to object
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	// cal light brightness regrads to angle b/tw eye and reflect light
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld),0,1);
	// control lobe of specular light
	s = pow(s, 20);
	vec4 specularLight = s * brightness_specular * vec4(1.0,1.0,1.0,1.0);

	vec4 MaterialAmbientColor = vec4(color * theColor, 1.0);
	vec4 MaterialDiffuseColor = vec4(color, 1.0) * clamp(diffuseLight, 0, 1);
	vec4 MaterialSpecularColor = clamp(specularLight, 0, 1);

	vec3 lightVectorWorld2 = normalize(lightPositionWorld2 - vertexPositionWorld);
	vec3 reflectedLightVectorWorld2 = reflect(-lightVectorWorld2, normal);
	float s2 = clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld),0,1);
	s2 = pow(s2, 5);
	vec4 specularLight2 = s2 * brightness_specular2 * vec4(1.0, 0.0, 0.0, 1.0);
	vec4 MaterialSpecularColor2 = clamp(specularLight2, 0, 1);

	//final color
	//daColor = vec4(color,1.0)*theColor  +  vec4(color,1.0)*clamp(diffuseLight,0,1) + clamp(specularLight,0,1) ;

	daColor = MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor + MaterialSpecularColor2;
}
