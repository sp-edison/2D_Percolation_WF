double gaussian ( double mean, double scale ) {
	double x, y, z;
	x = genrand64_real2();
	y = genrand64_real2();
	z = sqrt(-2.*log(x))*sin(2.*M_PI*y);
	return mean + z * scale;
}
double cauchy ( double mean, double scale ) {
	double x, z;
	x = genrand64_real1();
	z = mean + scale * tan(M_PI*x);
	return z;
}
double power_law ( double mode, double alpha ) {
	double x, y;
	x = genrand64_real1();
	y = mode * pow(1.-x,-1./(alpha - 1.));
	return y;
}
