#version 400

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float spec_ex;

uniform vec4 LightPosition; // Light position in world coords.

uniform vec3 La;
uniform vec3 Ld; 
uniform vec3 Ls; 

in vec3 direction_to_light_eye;
in vec4 position_eye;

void main()
{


	vec3 _direction_to_light_eye = normalize(direction_to_light_eye);
	vec4 _position_eye = normalize(position_eye);
 
    //normalised vector towards the light source
	vec3 normal_eye = normalize(vec3(LightPosition - _position_eye));
 /* 
	// The diffuse shading equation, dot product gives us the cosine of angle between the vectors
	vec3 Id = Ld * kd * max( dot( normal_eye, _direction_to_light_eye ), 0.0 );

	vec3 Ia = La * ka;




	vec3 reflection_eye = reflect (-_direction_to_light_eye, normal_eye);

	vec3 surface_to_viewer_eye = vec3(normalize (-_position_eye));

	float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);

	dot_prod_specular = max (dot_prod_specular, 0.0);

	float specular_factor = pow (dot_prod_specular, spec_ex);

	vec3 Is = Ls * ks * specular_factor; // final specular intensity
	
	vec3 LightIntensity = Ia + Id + Is;
	*/

	vec3 color;

//	float intensity = length(LightIntensity);
	float intensity = dot(_direction_to_light_eye, normal_eye);

	if (intensity > 0.99)
		color = ks*Ls;
	else if (intensity > 0.5)
		color = kd*Ld;
	else if (intensity > 0.25)
		color = kd*Ld*0.5;
	else
		color = ka*La;

	gl_FragColor = vec4(color, 1.0);
	
}