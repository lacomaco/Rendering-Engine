#version 430 core

#define ThreadGroupX 16
#define ThreadGroupY 16
#define PI 3.14159265359

layout(std430, binding = 0) buffer DataBuffer {
	float data[];
};

layout(local_size_x = ThreadGroupX, local_size_y = ThreadGroupY, local_size_z = 1) in;

int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// see : https://3dvar.com/Green2003Spherical.pdf
float P(int l,int m,float x)
{
	float pmm = 1.0;

	if(m>0) {
		float somx2 = sqrt((1.0-x)*(1.0+x));
		float fact = 1.0;
		for(int i=1; i<=m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}

	if(l==m) {
		return pmm;
	}

	float pmmp1 = x * (2.0*m+1.0) * pmm;

	if(l==m+1) {
		return pmmp1;
	}

	float pll = 0.0;

	for(int ll=m+2; ll<=l; ++ll) {
		pll = ((2.0*ll-1.0) * x * pmmp1-(ll+m-1.0)*pmm) / (ll-m);
		pmm = pmmp1;
		pmmp1 = pll;
	}

	return pll;
}

float K(int l, int m)
{
	float temp = ((2.0*l+1.0)*factorial(l-m)) / (4.0*PI*factorial(l+m));
	return sqrt(temp);
}

float SH(int l, int m, float theta, float phi)
{
	const float sqrt2 = sqrt(2.0);

	if(m==0) {
		return K(l,0)*P(l,m,cos(theta));
	}
	else if(m>0) {
		return sqrt2*K(l,m)*cos(m*phi)*P(l,m,cos(theta));
	}
	else {
		return sqrt2*K(l,-m)*sin(-m*phi)*P(l,-m,cos(theta));
	}
}

void main () {
	uint localId = gl_LocalInvocationID.x + gl_LocalInvocationID.y * ThreadGroupX;
	data[localId] = float(localId);
}