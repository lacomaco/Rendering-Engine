#define PI 3.14159265359

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

void L2(float theta,float phi,out float Y[9]){
	Y[0] = SH(0,0,theta,phi);
	Y[1] = SH(1,-1,theta,phi);
	Y[2] = SH(1,0,theta,phi);
	Y[3] = SH(1,1,theta,phi);
	Y[4] = SH(2,-2,theta,phi);
	Y[5] = SH(2,-1,theta,phi);
	Y[6] = SH(2,0,theta,phi);
	Y[7] = SH(2,1,theta,phi);
	Y[8] = SH(2,2,theta,phi);
}

// m이 0인 Y값 모음
void M0(float theta,out float Y[9]) {
	Y[0] = SH(0,0,theta,0);
	Y[1] = SH(1,0,theta,0);
	Y[2] = SH(2,0,theta,0);
}

vec3 computeF(vec3 v,float Y[27]){
	float theta = atan(v.y,v.x);
	float phi = atan(sqrt(v.x * v.x + v.y * v.y), v.z);

	vec3 result = vec3(0.0);

	float y[9];
	L2(theta,phi,y);

	for(int i = 0; i < 9; i++){
		result.r += Y[i * 3] * y[i];
		result.g += Y[i * 3 + 1] * y[i];
		result.b += Y[i * 3 + 2] * y[i];
	}

	return result;
}
