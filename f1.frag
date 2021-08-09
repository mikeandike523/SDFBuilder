#version 150
uniform float axisX;
uniform float axisY;
uniform float axisZ;
uniform sampler3D sdf;
uniform int loaded;
uniform float winW;
uniform float winH;
uniform float progress;

const float max_distance = 2048.0;
const float sdf_epsilon = 0.001;
const float normal_epsilon = 0.006;
const int max_steps = 512;
const float s_bias = 0.010;
const float s_strength = 0.005;
uniform mat3 matRotXZ;
uniform mat3 matRotZY;
uniform float zoom;
uniform float axs;
const float pi=3.141592;

//three point lighting
const int nl=32;

//funcdefs
float march(vec3 origin, vec3 ray);

float get_sdf(vec3 p){
	vec3 texCoord = vec3((p.z+axisZ)/(2.0*axisZ),(p.y+axisY)/(2.0*axisY),(p.x+axisX)/(2.0*axisX));
	return texture(sdf,texCoord).x;
}


float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdCappedCylinder( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float primary_sdf(vec3 position){
	float s = get_sdf(position);
	float disc_h = 0.1;
	return min(max(s,sdBox(position,vec3(axisX,axisY,axisZ))),sdCappedCylinder(position-(vec3(0.0,-axisY/axs,0.0)-vec3(0.0,disc_h/2.0,0.0)),2.0*max(axisX,axisZ),0.1));

}

//todo: change to circular ring of N lights (maybe 16, 32)
float light(vec3 position, vec3 n){

	float l=1.0;
	for(int i=0;i<nl;i++){
		float a=float(i)/float(nl)*2.0*pi;
		vec3 np = position+s_bias*n;
		vec3 lp=vec3(1.5*axisX*cos(a),2.5*axisY,1.5*axisZ*sin(a));
		vec3 ld = normalize(lp-np);
		
		float d = march(np,ld);
		if(d>sdf_epsilon){
			l-=s_strength;
		}
	}

	for(int i=0;i<nl;i++){
		float a=float(i)/float(nl)*2.0*pi;
		vec3 np = position+s_bias*n;
		vec3 lp=vec3(1.5*axisX*cos(a),0.5*axisY,1.5*axisZ*sin(a));
		vec3 ld = normalize(lp-np);
		
		float d = march(np,ld);
		if(d>sdf_epsilon){
			l-=s_strength;
		}
	}
	

	return l;
}


vec3 material(vec3 position, vec3 n){
	//return (n.yyy);
	return light(position,n)*vec3(1.0);
}


//code copied from previous project, not very clean
float march(vec3 origin, vec3 ray){

	

	float d = 0.0;
	for(int i=0;i<max_steps;i++){
		vec3 current = origin+ray*d;
		float step = primary_sdf(current);


		vec3 next = origin+ray*(d+step);
		d=d+step;
		
		if(d>max_distance){
			return -1.0;
		}

		if(step<sdf_epsilon){
			return d;
			break;
		}
		
	}
	
	return -1.0;

}

vec3 get_normal(vec3 intersection, float march_distance){

	float d = march_distance;

	vec3 axis1 = intersection+vec3(1.0,0.0,0.0)*normal_epsilon;
	vec3 axis2 = intersection+vec3(0.0,1.0,0.0)*normal_epsilon;
	vec3 axis3 = intersection+vec3(0.0,0.0,1.0)*normal_epsilon;

	float d1 = primary_sdf(axis1);
	float d2 = primary_sdf(axis2);
	float d3 = primary_sdf(axis3);

	float r1 = d1/d;
	float r2 = d2/d;
	float r3 = d3/d;

	vec3 n = normalize(vec3(r1,r2,r3));

	return n;



}



void main() {

	
	vec3 col = vec3(0.0);
	vec2 uv = gl_FragCoord.xy/vec2(winW,winH)/gl_FragCoord.w-vec2(0.5);
	

	if(loaded==1){

		vec3 corigin = vec3(0.0,0.0,-zoom);
		corigin=matRotXZ*matRotZY*corigin;
		vec2 uv_adj = vec2(uv.x,uv.y*winH/winW);
		
		if(gl_FragCoord.x/winW>progress)
		{
			col=vec3(1.0);
		}else{
			col=vec3(uv_adj.xy,0.0);
		}
		vec3 ray = normalize(vec3(uv_adj.xy,1.0));
		ray=matRotXZ*matRotZY*ray;
		float d = march(corigin,ray);
		if(d>0.0){
			vec3 pt= corigin+d*ray;
			vec3 n = get_normal(pt,d);
			col=material(pt,n);

		}


	}
	else{
		if(gl_FragCoord.x/winW>progress)
		{
			col=vec3(1.0,0.0,0.0);
		}else{
			col=vec3(0.0,0.0,1.0);
		}
	}

	gl_FragColor = vec4(col,1.0);
}