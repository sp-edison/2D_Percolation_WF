int Static_SF ( int **Network, int N, double mean_k, double alpha ) {
	int **tmp_network;
	int i = 0, j = 0, k = 0, l = 0;
	double normalize;
	double *rnds, *rnd2;
	double now_k = 0.;
	//double alpha = 2. / (ga

	int Actives, *Active, tmpact;
	int *rich, *poor;

	tmp_network = (int**)malloc(sizeof(int*)*N);
	rich = (int*)malloc(sizeof(int)*N);
	poor = (int*)malloc(sizeof(int)*N);
	Active = (int*)malloc(sizeof(int)*N);
	rnds = (double*)malloc(sizeof(double)*N);
	rnd2 = (double*)malloc(sizeof(double)*N);

	normalize = 0.;

	for(i=0;i<N;i++){
		tmp_network[i] = (int*)malloc(sizeof(int)*1);
		tmp_network[i][0] = 0;
		rich[i] = i;
		poor[i] = i;
		rnds[i] = pow((double)(i+1),-alpha);
		normalize += rnds[i];
		Active[i] = i;
	}
	Actives = N;
	for(i=0;i<N;i++){
		rnds[i] /= normalize/(double)N;
		rnd2[i] = 0.;
	}

	for(i=0;i<Actives;i++){
		if ( Active[i] == -1 ) continue;
		j = Actives-1;
		while ( rnds[i] + rnds[j] > 2. && rnds[j] < 1. ) {
			if ( Active[j] == -1 ) {
				j--;
				continue;
			}
			rnds[i] -= 1. - rnds[j];
			rnd2[j] += 1. - rnds[j];
			poor[j] = i;
			Active[j] = -1;
			Actives--;
			j--;
		}
	}

/*	for(i=Actives-1;i>=0;i--){
		if ( rnds[i] > 1. ) break;
		for(j=0;j<i;j++){
			if ( rnds[i] + rnds[j] > 2. ) {
				rnds[j] -= 1. - rnds[i];
				rnd2[i] += 1. - rnds[i];
				poor[i] = j;
				Actives--;
				Active[i] = Active[Actives];
				break;
			}
		}
	}*/


	//for(i=0;i<N;i++){
	//	printf("%d\t%d\t%e\t%d\t%e\t%e\n", poor[i], i, rnds[i], Active[i], rnd2[i], rnds[i] + rnd2[i]);
	//}


	i = 0;
	tmpact = Actives;
	Active[0] = Active[--Actives];
	//printf("%d actives\n", Actives);
	while ( Actives > 0 ) {
		j = i;
		while ( (j == i) || (rnds[i] + rnd2[i] - 1. + rnds[j] < 0.999999999) || (rnds[i]+rnd2[i]-1.+rnds[j]>2.&&rnds[j]>1. )) {
			k = (int)(genrand64_real2()*(double)Actives);
			j = Active[k];
			if ( Actives == 1 ) break;
			//printf("%d\t%e + %e = %e\n", j, rnds[i] + rnd2[i] - 1., rnds[j], rnds[i]+rnd2[i]-1.+rnds[j]);
		}
		//printf("%d selected\n", j);


		if ( rnds[i] + rnd2[i] - 1. + rnds[j] > 2. ) {
			rnd2[j] += 1. - rnds[j];
			rnds[i] -= 1. - rnds[j];
			poor[j] = i;
			Actives--;
			Active[k] = Active[Actives];
		}
		else {
			rnd2[j] += rnds[i] + rnd2[i] - 1.;
			rnds[i] -= rnds[i] + rnd2[i] - 1.;
			poor[j] = i;
			Actives--;
			Active[k] = Active[Actives];
			i = j;
		}

		//for(l=0;l<20;l++){
		//	printf("%d\t%d\t%e\t%d\t%e\t%e\n", poor[l], rich[l], rnds[l], Active[l], rnd2[l], rnds[l] + rnd2[l]);
		//}
		//getchar();
	}

	//printf("finish worker-algorithm\n");
	
	while ( now_k < mean_k ) {
		int lg_n = 0, sm_n = 0, check = 0;

		while ( lg_n == sm_n || check ) {
			check = 0;
			i = (int)(genrand64_real2()*N);
			if ( genrand64_real2() < rnds[i] ) {
				lg_n = i;
			}
			else {
				lg_n = poor[i];
			}
			
			i = (int)(genrand64_real2()*N);
			if ( genrand64_real2() < rnds[i] ) {
				sm_n = i;
			}
			else {
				sm_n = poor[i];
			}
			for(i=1;i<=tmp_network[lg_n][0];i++){
				if ( sm_n == tmp_network[lg_n][i]) {
					check = 1;
					//printf("%d\t%d - reselect\n", lg_n, sm_n);
					break;
				}
			}
			//printf("%d\t%d - selected\n", lg_n, sm_n);
		}
		tmp_network[lg_n][0]++;
		tmp_network[sm_n][0]++;
		tmp_network[lg_n] = (int*)realloc(tmp_network[lg_n], sizeof(int)*(tmp_network[lg_n][0]+1));
		tmp_network[sm_n] = (int*)realloc(tmp_network[sm_n], sizeof(int)*(tmp_network[sm_n][0]+1));
		tmp_network[lg_n][tmp_network[lg_n][0]] = sm_n;
		tmp_network[sm_n][tmp_network[sm_n][0]] = lg_n;
		now_k += 2. / (double)N;
	}

	//printf("clear : make links\n");

	free(rnds);
	free(rnd2);

	l = 0;
	while ( 1 ) {
		Actives = 1;
		tmpact = 0;
		Active[0] = (int)(genrand64_real2()*(double)N);
		rich[Active[0]] = 0;
		poor[Active[0]] = -1;
		while ( Actives > tmpact ) {
			j = Active[tmpact];
			for(i=1;i<=tmp_network[j][0];i++){
				if ( poor[tmp_network[j][i]] != -1 ) {
					Active[Actives] = tmp_network[j][i];
					rich[tmp_network[j][i]] = Actives;
					Actives++;
					poor[tmp_network[j][i]] = -1;
				}
			}
			tmpact++;
		}
		if ( tmpact > N/2 ) break;
	}

	//printf("before moving \t %d \n", Active[3449]);
	for(i=0;i<N;i++) poor[i] = 0;

	for (i=0;i<tmpact;i++) {
		//printf("moving %d:%d node...(total %d)\n", i, Active[i], tmpact);
		j = Active[i];
		//printf("%d = Active[%d]\n", j, i);
		Network[i] = (int*)malloc(sizeof(int)*(tmp_network[j][0]+1));
		//if ( i == 3448 ) {
		//	printf("%d\t%d\t%d\n", Active[3449], tmp_network[Active[3449]][0], tmp_network[Active[3449]][1]);
		//}
		//printf("malloc process\n");
		Network[i][0] = tmp_network[j][0];
		for(k=1;k<=Network[i][0];k++){
			Network[i][k] = rich[tmp_network[j][k]];
			poor[j] = 1;
			//printf("\t%d:%d", Network[i][k], tmp_network[j][k]);
			//if ( k % 5 == 0 ) printf("\n");
		}
		free(tmp_network[j]);
		//printf("\n");
		//printf("f-ree\n");
	}
	
	//printf("after moving\n");

	for(i=0;i<N;i++){
		if ( poor[i] == 0 ) free(tmp_network[i]);
	}	
	free(tmp_network);
	free(poor);
	free(rich);
	free(Active);

	return tmpact;
}


int Static_ER ( int **Network, int N, double mean_k ) {
	int **tmp_network;
	int i, j, k, l;
	double normalize;
	double now_k = 0.;
	//double alpha = 2. / (ga

	int Actives, *Active, tmpact;
	int *rich, *poor;

	tmp_network = (int**)malloc(sizeof(int*)*N);
	rich = (int*)malloc(sizeof(int)*N);
	poor = (int*)malloc(sizeof(int)*N);
	Active = (int*)malloc(sizeof(int)*N);

	normalize = 0.;

	for(i=0;i<N;i++){
		tmp_network[i] = (int*)malloc(sizeof(int)*1);
		tmp_network[i][0] = 0;
		rich[i] = i;
		poor[i] = i;
		Active[i] = i;
	}

	while ( now_k < mean_k ) {
		int lg_n = 0, sm_n = 0, check = 0;

		while ( lg_n == sm_n || check ) {
			check = 0;
			lg_n = (int)(genrand64_real2()*N);
			sm_n = (int)(genrand64_real2()*N);
			for(i=1;i<=tmp_network[lg_n][0];i++){
				if ( sm_n == tmp_network[lg_n][i]) {
					check = 1;
					//printf("%d\t%d - reselect\n", lg_n, sm_n);
					break;
				}
			}
			//printf("%d\t%d - selected\n", lg_n, sm_n);
		}
		tmp_network[lg_n][0]++;
		tmp_network[sm_n][0]++;
		tmp_network[lg_n] = (int*)realloc(tmp_network[lg_n], sizeof(int)*(tmp_network[lg_n][0]+1));
		tmp_network[sm_n] = (int*)realloc(tmp_network[sm_n], sizeof(int)*(tmp_network[sm_n][0]+1));
		tmp_network[lg_n][tmp_network[lg_n][0]] = sm_n;
		tmp_network[sm_n][tmp_network[sm_n][0]] = lg_n;
		now_k += 2. / (double)N;
	}

	//printf("clear : make links\n");
	l = 0;
	while ( 1 ) {
		Actives = 1;
		tmpact = 0;
		Active[0] = (int)(genrand64_real2()*(double)N);
		rich[Active[0]] = 0;
		poor[Active[0]] = -1;
		while ( Actives > tmpact ) {
			j = Active[tmpact];
			for(i=1;i<=tmp_network[j][0];i++){
				if ( poor[tmp_network[j][i]] != -1 ) {
					Active[Actives] = tmp_network[j][i];
					rich[tmp_network[j][i]] = Actives;
					Actives++;
					poor[tmp_network[j][i]] = -1;
				}
			}
			tmpact++;
		}
		if ( tmpact > N/2 ) break;
	}

	//printf("before moving \t %d \n", Active[3449]);
	for(i=0;i<N;i++) poor[i] = 0;

	for (i=0;i<tmpact;i++) {
		//printf("moving %d:%d node...(total %d)\n", i, Active[i], tmpact);
		j = Active[i];
		//printf("%d = Active[%d]\n", j, i);
		Network[i] = (int*)malloc(sizeof(int)*(tmp_network[j][0]+1));
		//if ( i == 3448 ) {
		//	printf("%d\t%d\t%d\n", Active[3449], tmp_network[Active[3449]][0], tmp_network[Active[3449]][1]);
		//}
		//printf("malloc process\n");
		Network[i][0] = tmp_network[j][0];
		for(k=1;k<=Network[i][0];k++){
			Network[i][k] = rich[tmp_network[j][k]];
			poor[j] = 1;
			//printf("\t%d:%d", Network[i][k], tmp_network[j][k]);
			//if ( k % 5 == 0 ) printf("\n");
		}
		free(tmp_network[j]);
		//printf("\n");
		//printf("f-ree\n");
	}
	//printf("after moving\n");
	for(i=0;i<N;i++){
		if ( poor[i] == 0 ) free(tmp_network[i]);
	}
	free(tmp_network);
	free(poor);
	free(rich);
	free(Active);

	return tmpact;
}

int make_WS_Network (int** Network, int N, int int_k, double p) {
	int i, j, k, l, n, m;
	int check;
	double x, y, z;

	Network = (int**)realloc(Network, sizeof(int*)*N);
	for(i=0;i<N;i++){
		Network[i] = (int*)malloc(sizeof(int)*1);
		Network[i][0] = 0;
	}
	for(i=0;i<N;i++){
		for(j=0;j<(int_k/2);j++){
			if ( genrand64_real2() < p ) {
				//rewiring
				check = 1;
				l = i;
				while ( l == i || check != 0 ) {
					l = (int)(genrand64_real2() * N);
					if ( l == i ) continue;
					check = 0;
					for(k=1;k<=Network[i][0];k++){
						if ( Network[i][k] == l ) {
							check = 1;
							break;
						}
					}
				}
			}
			else {
				check = 0; // 연결할 위치에 링크가 이미 있는 경우,
							  // rewiring 할지 안 할지 체크.
				l = (i + j + 1) % N;
				for(k=1;k<=Network[i][0];k++){
					if ( Network[i][k] == l ) {
						check = 1;
						break;
					}
				}
				if ( check != 0 ) { // rewiring
					check = 1;
					l = i;
					while ( l == i || check != 0 ) {
						l = (int)(genrand64_real2() * N);
						if ( l == i ) continue;
						check = 0;
						for(k=1;k<=Network[i][0];k++){
							if ( Network[i][k] == l ) {
								check = 1;
								break;
							}
						}
					}
				}
			}
			//printf("%d link to %d\n", i, l);
			Network[i] = (int*)realloc(Network[i], sizeof(int)*(++Network[i][0]+1));
			Network[l] = (int*)realloc(Network[l], sizeof(int)*(++Network[l][0]+1));
			Network[i][Network[i][0]] = l;
			Network[l][Network[l][0]] = i;
		}
	}
	return 0;
}

int make_gD_Network( int** Network, int L, int D ) {
	int i, j, k;
	double x, y, z;
	int N, Nn;
	int step[D+1];
	int count = 0;
	for(i=0;i<D;i++) step[i] = 0;
	N = 1;
	Nn = 2*D;
	//printf("%d\n", Nn);
	for(i=0;i<D;i++) {
		step[i] = N;
		N *= L;
	}
	step[D] = N;
	//for(i=0;i<=D;i++) printf("%d\t%d\n", i, step[i]);
	//Network = (int**)realloc(Network, sizeof(int*)*N);

	for(i=0;i<N;i++) {
		Network[i] = (int*)malloc(sizeof(int)*(Nn+1));
		Network[i][0] = Nn;
	}

	for(k=0;k<N;k++){
		count = 1;
		for(j=0;j<D;j++){
			//printf("%d\t%d\t%d\n", j, step[j], step[j+1]);
			//printf("join %d\t%d\n", k, (k / step[j+1])*step[j+1] + (k + step[j]) % step[j+1]);
			//printf("join %d\t%d\n", k, (k / step[j+1])*step[j+1] + (step[j+1] + k - step[j]) % step[j+1]);
			Network[k][count++] = (k / step[j+1])*step[j+1] + (k + step[j]) % step[j+1];
			//printf("join %d\t%d\n", k, Network[k][count-1]);
			Network[k][count++] = (k / step[j+1])*step[j+1] + (step[j+1] + k - step[j]) % step[j+1];
			//printf("join %d\t%d\n", k, Network[k][count-1]);
		}
	}
	//printf("%d\n", Network[0][0]);
	//printf("%0x\n", Network);
	return N;
}

int Full_Static_SF ( int **Network, int N, double mean_k, double alpha ) {
	int **tmp_network;
	int i = 0, j = 0, k = 0, l = 0;
	double normalize;
	double *rnds, *rnd2;
	double now_k = 0.;
	//double alpha = 2. / (ga

	int Actives, *Active, tmpact;
	int *rich, *poor;

	tmp_network = (int**)malloc(sizeof(int*)*N);
	rich = (int*)malloc(sizeof(int)*N);
	poor = (int*)malloc(sizeof(int)*N);
	Active = (int*)malloc(sizeof(int)*N);
	rnds = (double*)malloc(sizeof(double)*N);
	rnd2 = (double*)malloc(sizeof(double)*N);

	normalize = 0.;

	for(i=0;i<N;i++){
		tmp_network[i] = (int*)malloc(sizeof(int)*1);
		tmp_network[i][0] = 0;
		rich[i] = i;
		poor[i] = i;
		rnds[i] = pow((double)(i+1),-alpha);
		normalize += rnds[i];
		Active[i] = i;
	}
	Actives = N;
	for(i=0;i<N;i++){
		rnds[i] /= normalize/(double)N;
		rnd2[i] = 0.;
	}

	for(i=0;i<Actives;i++){
		if ( Active[i] == -1 ) continue;
		j = Actives-1;
		while ( rnds[i] + rnds[j] > 2. && rnds[j] < 1. ) {
			if ( Active[j] == -1 ) {
				j--;
				continue;
			}
			rnds[i] -= 1. - rnds[j];
			rnd2[j] += 1. - rnds[j];
			poor[j] = i;
			Active[j] = -1;
			Actives--;
			j--;
		}
	}

/*	for(i=Actives-1;i>=0;i--){
		if ( rnds[i] > 1. ) break;
		for(j=0;j<i;j++){
			if ( rnds[i] + rnds[j] > 2. ) {
				rnds[j] -= 1. - rnds[i];
				rnd2[i] += 1. - rnds[i];
				poor[i] = j;
				Actives--;
				Active[i] = Active[Actives];
				break;
			}
		}
	}*/


	//for(i=0;i<N;i++){
	//	printf("%d\t%d\t%e\t%d\t%e\t%e\n", poor[i], i, rnds[i], Active[i], rnd2[i], rnds[i] + rnd2[i]);
	//}


	i = 0;
	tmpact = Actives;
	Active[0] = Active[--Actives];
	//printf("%d actives\n", Actives);
	while ( Actives > 0 ) {
		j = i;
		while ( (j == i) || (rnds[i] + rnd2[i] - 1. + rnds[j] < 0.999999999) || (rnds[i]+rnd2[i]-1.+rnds[j]>2.&&rnds[j]>1. )) {
			k = (int)(genrand64_real2()*(double)Actives);
			j = Active[k];
			if ( Actives == 1 ) break;
			//printf("%d\t%e + %e = %e\n", j, rnds[i] + rnd2[i] - 1., rnds[j], rnds[i]+rnd2[i]-1.+rnds[j]);
		}
		//printf("%d selected\n", j);


		if ( rnds[i] + rnd2[i] - 1. + rnds[j] > 2. ) {
			rnd2[j] += 1. - rnds[j];
			rnds[i] -= 1. - rnds[j];
			poor[j] = i;
			Actives--;
			Active[k] = Active[Actives];
		}
		else {
			rnd2[j] += rnds[i] + rnd2[i] - 1.;
			rnds[i] -= rnds[i] + rnd2[i] - 1.;
			poor[j] = i;
			Actives--;
			Active[k] = Active[Actives];
			i = j;
		}

		//for(l=0;l<20;l++){
		//	printf("%d\t%d\t%e\t%d\t%e\t%e\n", poor[l], rich[l], rnds[l], Active[l], rnd2[l], rnds[l] + rnd2[l]);
		//}
		//getchar();
	}

	//printf("finish worker-algorithm\n");
	
	while ( now_k < mean_k ) {
		int lg_n = 0, sm_n = 0, check = 0;

		while ( lg_n == sm_n || check ) {
			check = 0;
			i = (int)(genrand64_real2()*N);
			if ( genrand64_real2() < rnds[i] ) {
				lg_n = i;
			}
			else {
				lg_n = poor[i];
			}
			
			i = (int)(genrand64_real2()*N);
			if ( genrand64_real2() < rnds[i] ) {
				sm_n = i;
			}
			else {
				sm_n = poor[i];
			}
			for(i=1;i<=tmp_network[lg_n][0];i++){
				if ( sm_n == tmp_network[lg_n][i]) {
					check = 1;
					//printf("%d\t%d - reselect\n", lg_n, sm_n);
					break;
				}
			}
			//printf("%d\t%d - selected\n", lg_n, sm_n);
		}
		tmp_network[lg_n][0]++;
		tmp_network[sm_n][0]++;
		tmp_network[lg_n] = (int*)realloc(tmp_network[lg_n], sizeof(int)*(tmp_network[lg_n][0]+1));
		tmp_network[sm_n] = (int*)realloc(tmp_network[sm_n], sizeof(int)*(tmp_network[sm_n][0]+1));
		tmp_network[lg_n][tmp_network[lg_n][0]] = sm_n;
		tmp_network[sm_n][tmp_network[sm_n][0]] = lg_n;
		now_k += 2. / (double)N;
	}

	//printf("clear : make links\n");

	free(rnds);
	free(rnd2);

	l = 0;
	for(i=0;i<N;i++){
		if ( tmp_network[i][0] > 0 ) {
			Active[Actives] = i;
			rich[i] = Actives;
			Actives++;
		}
		tmpact++;
	}
	//printf("before moving \t %d \n", Active[3449]);
	for(i=0;i<N;i++) poor[i] = 0;

	for (i=0;i<tmpact;i++) {
		//printf("moving %d:%d node...(total %d)\n", i, Active[i], tmpact);
		j = Active[i];
		//printf("%d = Active[%d]\n", j, i);
		Network[i] = (int*)malloc(sizeof(int)*(tmp_network[j][0]+1));
		Network[i][0] = tmp_network[j][0];
		for(k=1;k<=Network[i][0];k++){
			Network[i][k] = rich[tmp_network[j][k]];
			poor[j] = 1;
			//printf("\t%d:%d", Network[i][k], tmp_network[j][k]);
			//if ( k % 5 == 0 ) printf("\n");
		}
		free(tmp_network[j]);
		//printf("\n");
		//printf("f-ree\n");
	}
	//printf("after moving\n");
	for(i=0;i<N;i++){
		if ( poor[i] == 0 ) free(tmp_network[i]);
	}
	free(tmp_network);
	free(poor);
	free(rich);
	free(Active);

	return tmpact;
}
