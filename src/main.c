#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mt64.h"
#include "rand.h"
#include "Network.h"

#include "mt64.c"
#include "rand.c"
#include "Network.c"

int main ( int argc, char *argv[] ) {
	int L, N;
	int i, j, k;
	int count = 0, procount = 0;
	int stchk = 0;
	char tmpstr1[100], tmpstr2[100];
	int D = 2;
	double p;

	FILE *rh, *wh;

	int **Network;
	int **cluster;

	int *sequence;
	int *occupy, *cinfo;
	
	system("mkdir -p result");

	if ( strcmp(argv[1], "-i") == 0) {
		rh = fopen(argv[2], "r");
	} else if ( strcmp(argv[3], "-i") == 0) {
		rh = fopen(argv[4], "r");
	}
	while( fscanf(rh, "%s = %s", tmpstr1, tmpstr2) == 2 ) {
		//printf("first : [%s]\nsecond : [%s]\n", tmpstr1, tmpstr2);
		if ( strcmp(tmpstr1,"L") == 0 ) {
			L = atoi(tmpstr2);
		} else if ( strcmp(tmpstr1, "p") == 0 ) {
			p = atof(tmpstr2);
		}
		//tmpstr1[0] = 0;
		//tmpstr2[0] = 0;
	}
	fclose(rh);
	//printf("L, P => %d\t%f\n", L, p);
	wh = fopen("result/input.txt", "w");
	fprintf(wh, "L = %d\np = %.2f\n", L, p);
	fclose(wh);
	
	init_genrand64((unsigned)time(NULL) + getpid());

	N = 1;
	for(i=0;i<D;i++) N *= L;
	Network = (int**)malloc(sizeof(int*)*N);
	cluster = (int**)malloc(sizeof(int*)*N);
	cinfo = (int*)malloc(sizeof(int)*N);
	occupy = (int*)malloc(sizeof(int)*N);

	printf("...generating graph...");
	N = make_gD_1npc_Network(Network, L, 2);
	printf("  Done!\n");

	for(i=0;i<N;i++){
		cluster[i] = (int*)malloc(sizeof(int)*2);
		cluster[i][0] = 1;
		cluster[i][1] = i;
		cinfo[i] = i;
	}


	//for(i=0;i<N;i++){
	//	for(j=1;j<=Network[i][0];j++){
	//		printf("%d[%d]\t%d\n", i, Network[i][0], Network[i][j]);
	//	}
	//}

	printf("...occupying vertex...");
	for(i=0;i<N;i++){
		if ( genrand64_real2() < p ) {
			occupy[i] = 1;
			count++;
		} else {
			occupy[i] = 0;
		}
	}
	for(i=0;i<L;i++){
		if ( occupy[i] ) occupy[i] = 0x03;
	}
	for(i=N-L;i<N;i++){
		if ( occupy[i] ) occupy[i] = 0x05;
	}
	printf("  Done!\n");

	printf("...checking percolation state...\n");
	for(i=0;i<L;i++){
		if ( occupy[i] && cluster[cinfo[i]][0] == 1 ) {
			int stack[N];
			int stn = 1, dn = 0;
			stack[0] = i;
			procount++;

			while ( stn > dn ) {
				int chosen = stack[dn++];
				for(j=1;j<=Network[chosen][0];j++){
					int neighbor = Network[chosen][j];
					if ( occupy[neighbor] && cinfo[neighbor] != cinfo[chosen] ) {
						int mycl = cinfo[chosen];
						int necl = cinfo[neighbor];
						int mycln = cluster[mycl][0];
						int necln = cluster[necl][0];

						cluster[mycl] = (int*)realloc(cluster[mycl], sizeof(int)*(mycln+necln+1));
						for(k=1;k<=necln;k++){
							cluster[mycl][++cluster[mycl][0]] = cluster[necl][k];
							cinfo[cluster[necl][k]] = mycl;
						}
						occupy[mycl] = occupy[mycl] | occupy[necl];

						//printf("\t%d add to stack\n", neighbor);
						procount++;
						//printf("count = %d\tprocount = %d\n", count, procount);

						if ( ((procount * 100) / count) > stchk ) {
							printf("\t%3d %s\n", (procount * 100 / count), "% ...");
							while(stchk<procount*100/count)stchk += count/10;
						}
						stack[stn++] = neighbor;
						//printf("added %d ( %d-th std) \tseed : %d\n", neighbor, stn, i);
					}
				}
			}
		}
	}
	printf("  Done!\n");

	for(i=0;i<L;i++){
		for(j=0;j<L;j++){
			printf("%02d[%02d][%d] ", i*L+j, cinfo[i*L+j], occupy[cinfo[i*L+j]]);
		}
		printf("\n");
	}

	wh = fopen("result/lattice.txt", "w");
	for(i=0;i<L;i++){
		for(j=0;j<L;j++){
			fprintf(wh, "%d ", occupy[cinfo[i*L+j]]);
		}
		fprintf(wh,"\n");
	}
	fclose(wh);

	return 0;
}
