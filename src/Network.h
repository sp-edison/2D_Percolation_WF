int Static_SF ( int **Network, int N, double mean_k, double alpha );
	// alpha = 0.65 -> gamma = 2.59
	// alpha = 0.38 -> gamma = 3.87
int Static_ER ( int **Network, int N, double mean_k );

int make_WS_Network (int** Network, int N, int int_k, double p);
	// Network[i][0] = the number of degree i
	// N : number of total node
	// int_k : number of degree(mean)
	// p : rewiring probability
int make_gD_Network( int** Network, int L, int D );
	// L : L^D lattice
	// D : Dimension

int make_Triangular( int** Network, int L );
int Full_Static_SF ( int **Network, int N, double mean_k, double alpha );
int Conf_SF ( int **Network, int N, double mean_k, double alpha );
int make_WS3_Network (int** Network, int N, int int_k, int *color);
