//reference1::https://learnopengl.com/
// reference2::https://www.cnblogs.com/hehao98/p/8709636.html

#version 440 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
const float R = 0.4;
const float c = 0.0001;
uniform sampler2D ourTexture;

uniform vec3 lightDir;
uniform vec3 lightColors;
uniform samplerCube skybox;

uniform vec3 camPos;


float Fog(float distance){
	
	return (1-exp(-c*distance));
	
}

void main()
{		
    vec3 N = normalize(Normal);
	vec3 L = normalize(lightDir);
    vec3 V = normalize(camPos - WorldPos);
	
	vec3 I = normalize(WorldPos - camPos);
	vec3 reflectVec = 2*dot(V,N)*N-V;
	float thetaI = acos(dot(V,N));
	float thetaR = acos(dot(reflectVec,N));
	float thetaT = asin(0.75*sin(thetaI));
	
	float r;
	if(abs(thetaI)>=0.000001){
		float t1 = sin(thetaT-thetaI);
		float t2 = sin(thetaT+thetaI);
		float t3 = tan(thetaT-thetaI);
		float t4 = tan(thetaT+thetaI);
		r = clamp(0.5*(t1*t1/(t2*t2)+t3*t3/(t4*t4)),0.0,1.0);
		
	}
	else{
		r=0;
	}
	
    vec4 bottom_color = vec4(0.02, 0.05, 0.10,1.0);
	vec4 fog_color = texture(skybox, vec3(-V.x, 0.0, -V.z));
	vec4 true_color = vec4(0.02, 0.05, 0.10,1.0);
	float dis = length(vec3(camPos.x,0.0,camPos.z)-WorldPos);
	float fd = Fog(dis);
	
	vec3 sky_c = reflect(I, N);
	vec3 sun_c = reflect(-L,N);
	float spec = pow(max(dot(V, sun_c), 0.0), 32);
	vec3 specular = lightColors*spec;
	
    true_color=(1-spec)*R*vec4(texture(skybox, sky_c).rgb, 1.0)+spec*vec4(specular,1.0)+(1-R)*bottom_color;
	//true_color = (1-r)*bottom_color+r*vec4(texture(skybox, sky_c).rgb, 1.0);
	FragColor =mix(true_color,fog_color,fd);
}