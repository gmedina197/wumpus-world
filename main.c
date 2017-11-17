#include "wumpus-world.h"

int main(int argc, char *argv[]) {
	int realWorld[NOCOLS][NOROWS];	
	int agentWorld[NOCOLS][NOROWS];
	int x, y;						
	struct coord currentCoord;		
	struct coord start = {0, 0};	

	srand((unsigned int)time(0));

	for(y=0; y<NOROWS; y++){
		for(x=0; x<NOCOLS; x++){
			realWorld[x][y] = 0;
			agentWorld[x][y] = 0;
		}
	}

	generateRealWorld(realWorld);
	setFlag(agentWorld, start, CURRENT);	

	currentCoord = start;

	printf("Mundo com todos os objetos \n");
	displayWorldAll(realWorld);

	int count = 0;
	while(1){

		count++;
		currentCoord = getCurrentCoord(agentWorld);

		if(testFlag(agentWorld, currentCoord, VISITED) == 0){
			copyFlags(realWorld,agentWorld,currentCoord);
			if(testFlag(agentWorld, currentCoord, CAVE)){
				printf("\nAgente do mundo (Passo %d)\n",count);
				printf("Posição Atual (%d,%d):\n", currentCoord.x, currentCoord.y);
				displayWorldAll(agentWorld);

				printf("\nCaiu no buraco. GAME OVER!");
				break;
			} else if(testFlag(agentWorld,currentCoord,WUMPUS)){
				printf("\nAgente do mundo (Passo %d)\n",count);
				printf("Posição Atual (%d,%d):\n", currentCoord.x, currentCoord.y);
				displayWorldAll(agentWorld);

				printf("\nWumpus te devorou. GAME OVER!");
				break;
			} else if(testFlag(agentWorld,currentCoord,GLOW)){
				// Display agent world
				printf("\nAgente do mundo (Passo %d)\n",count);
				printf("Posição Atual (%d,%d):\n", currentCoord.x, currentCoord.y);
				displayWorldAll(agentWorld);

				printf("\nParabens! Voce achou ouro! :D");
				break;
			} else{

				setFlag(agentWorld, currentCoord, VISITED);

				evaluateNeighbors(agentWorld,currentCoord);
			}
		}

		// Display agent world
		printf("\nAgente do mundo(Passo %d)\n",count);
		printf("Posição Atual (%d,%d):\n", currentCoord.x, currentCoord.y);
		displayWorldAll(agentWorld);

		if(takeGlowAction(agentWorld, currentCoord)){
			printf("Esse movimento foi recomendado por takeGlowAction!\n");
		} else if(takeSafeAction(agentWorld, currentCoord)){
			printf("Esse movimento foi recomendado por takeSafeAction!\n");
		} else if(takeRollTheDiceAction(agentWorld, currentCoord)){
			printf("Esse movimento foi recomendado por takeRollTheDiceAction!\n");
		} else if(takeSuicideAction(agentWorld, currentCoord)){
			printf("Esse movimento foi recomendado por takeSuicideAction, não existiam mais opções!\n");
		}

		myPause();
	}

	myPause();
	return EXIT_SUCCESS;
}

struct coord getCurrentCoord(int world[NOCOLS][NOROWS]){
	int x,y;
	struct coord currentCoord = { -1, -1 };

	for(x=0; x<NOROWS; x++){
		for(y=0; y<NOCOLS; y++){
			struct coord testCoord = { x, y };
			if(testFlag(world, testCoord, CURRENT)){
				currentCoord = testCoord;
			}
		}
	}

	if(currentCoord.x == -1 || currentCoord.y == -1){
		fprintf(stderr, "Agente não foi devidamente colocado!\n");
		exit(EXIT_FAILURE);
	}else{
		return currentCoord;
	}
}

void displayWorldDecimal(int world[NOROWS][NOCOLS]){
	int x, y;

	for(y=NOROWS-1; y>=0; y--){
		for(x=0; x<NOCOLS; x++){
		  printf("%d",world[x][y]);
		  printf("\t");

		}
		printf("\n");
	}
}



void displayWorldAll(int world[NOROWS][NOCOLS]){
	displayWorldDecimal(world);
	fflush(stdout);
}

void neighborFieldsCoords(struct coord field, struct coord results[4]){
	// cima 
	results[0].x = field.x;
	results[0].y = field.y + 1;

	// direita
	results[1].x = field.x + 1;
	results[1].y = field.y;

	// baixo
	results[2].x = field.x;
	results[2].y = field.y - 1;

	// esquerda
	results[3].x = field.x - 1;
	results[3].y = field.y;
}

void delFlag(int world[NOCOLS][NOROWS], struct coord field, int action){
	if(field.x <0 || field.y<0){
		printf("Del flag: Numeros negativos!\n");
		exit(EXIT_FAILURE);
	}
	world[field.x][field.y] = world[field.x][field.y] & ~action;
}

void copyFlags(int fromWorld[NOCOLS][NOROWS], int toWorld[NOCOLS][NOROWS], struct coord field){
	if(field.x <0 || field.y<0){
		printf("Copy flag: Numeros negativos!\n");
		exit(EXIT_FAILURE);
	}
	toWorld[field.x][field.y] |= fromWorld[field.x][field.y];
}

int testFlag(int world[NOCOLS][NOROWS], struct coord field, int action){
	if(((world[field.x][field.y] & action) == action)){
		return 1;
	} else {
		return 0;
	}
}

void setFlag(int world[NOCOLS][NOROWS], struct coord field, int action){
	if(field.x <0 || field.y<0){
		printf("setFlag: Numeros negativos!\n");
		exit(EXIT_FAILURE);
	}
	if(action == VISITED){
		delFlag(world, field, SAFE);
		delFlag(world, field, CAVESUS);
		delFlag(world, field, GLOWSUS);
		delFlag(world, field, WUMPUSSUS);
	}
	world[field.x][field.y] = world[field.x][field.y] | action;
}

int randInt(int min, int max, int *except, int noExcept){
	int result, i;
	result = rand() % (max - min + 1) + min;
	if(except != NULL){
		for(i=0;i<noExcept;i++){
			if(result == except[i]){
				result = randInt(min,max,except,noExcept);
				break;
			}
		}
	}
	return result;
}

struct coord randPair(struct coord min, struct coord max, struct coord *except, int noExcept){
	int i;
	struct coord result;
	result.x = randInt(min.x,max.x, NULL,0);
	result.y = randInt(min.y,max.y, NULL,0);

	if(except != NULL){
		for(i=0;i<noExcept;i++){
			if(result.x == except[i].x && result.y == except[i].y){
				result = randPair(min,max,except,noExcept);
				break;
			}
		}
	}
	return result;
}

void generateRealWorld(int realWorld[NOCOLS][NOROWS]){
	int k, z, w;			// Counters.
	struct coord xy;
	struct coord cavePos[NOCAVES];	
	struct coord wumpusPos[NOWUMPUS];
	struct coord glowPos[NOGLOW];		
	struct coord neighbors[4];
	struct coord min = {1,1};					
	struct coord max = {NOROWS-1, NOCOLS-1};	
	struct coord default_except = {0,0};	// para wumpus, ouro, e caverna nao ficar na coordenada 1

	int except_i;	
	int except_size;	

	struct coord *except = NULL;		
	struct coord *except_temp = NULL;

	except_i = 0;
	except_size = 1;
	if ((except = (struct coord*)calloc(except_size, sizeof(struct coord))) == NULL) {
		fprintf(stderr, "ERRO.\n");
		exit(EXIT_FAILURE);
	}

	except[except_i] = default_except;

	k = 0;
	while(k<NOCAVES){
		except_i++;		
		if(except_i == except_size){
			except_size += 1;	
			if ((except_temp = (struct coord*)realloc(except, except_size * sizeof(struct coord))) == NULL) {
				fprintf(stderr, "ERRO.\n");
				free(except);
				exit(EXIT_FAILURE);
			}
			except = except_temp;	
		}
		cavePos[k] = randPair(min,max,except,except_i);
		except[except_i] = cavePos[k];

		k++;
	}

	// coord wumpus.
	k = 0;
	while(k<NOWUMPUS){
		except_i++;
		if(except_i == except_size){
			except_size += 1;
			if ((except_temp = (struct coord*)realloc(except, except_size * sizeof(struct coord))) == NULL) {
				fprintf(stderr, "ERRO.\n");
				free(except);
				exit(EXIT_FAILURE);
			}
			except = except_temp;
		}

		wumpusPos[k] = randPair(min,max,except,except_i);

		except[except_i] = wumpusPos[k];

		k++;
	}

	// coord ouro
	k = 0;
	while(k<NOGLOW){
		except_i++;
		if(except_i == except_size){
			except_size += 1;
			if ((except_temp = (struct coord*)realloc(except, except_size * sizeof(struct coord))) == NULL) {
				fprintf(stderr, "ERRO.\n");
				free(except);
				exit(EXIT_FAILURE);
			}
			except = except_temp;
		}

		glowPos[k] = randPair(min,max,except,except_i);

		except[except_i] = glowPos[k];

		k++;
	}

	free(except);

	for(xy.x=0; xy.x<NOROWS; xy.x++){
		for(xy.y=0; xy.y<NOCOLS; xy.y++){

			// checar se carvena pode ser colocado aqui
			for(k=0; k< NOCAVES; k++){
				if(xy.x == cavePos[k].x && xy.y == cavePos[k].y){
					setFlag(realWorld,cavePos[k], CAVE);

					neighborFieldsCoords(xy, neighbors);

					for(z=0; z<4; z++){
						if	(
								((neighbors[z].y < NOROWS) && (z == 0))
								||
								((neighbors[z].x < NOCOLS) && (z == 1))
								||
								((neighbors[z].y >= 0) && (z == 2))
								||
								((neighbors[z].x >= 0) && (z == 3))
							){
							setFlag(realWorld, neighbors[z], BREEZE);
						}
					}
				}
			}
			// checar se wumpus pode ser colocado aqui
			for(k=0; k< NOWUMPUS; k++){
				if(xy.x == wumpusPos[k].x && xy.y == wumpusPos[k].y){
					setFlag(realWorld, wumpusPos[k], WUMPUS);

					neighborFieldsCoords(xy, neighbors);

					for(z=0; z<4; z++){
						if	(
								((neighbors[z].y < NOROWS) && (z == 0))
								||
								((neighbors[z].x < NOCOLS) && (z == 1))
								||
								((neighbors[z].y >= 0) && (z == 2))
								||
								((neighbors[z].x >= 0) && (z == 3))
							){
							setFlag(realWorld, neighbors[z], STENCH);
						}
					}
				}
			}
			// checar se ouro pode ser colocado aqui
			for(k=0; k< NOGLOW; k++){
				if(xy.x == glowPos[k].x && xy.y == glowPos[k].y){
					setFlag(realWorld, glowPos[k], GLOW);

					neighborFieldsCoords(xy, neighbors);

					for(z=0; z<4; z++){
						if	(
								((neighbors[z].y < NOROWS) && (z == 0))
								||
								((neighbors[z].x < NOCOLS) && (z == 1))
								||
								((neighbors[z].y >= 0) && (z == 2))
								||
								((neighbors[z].x >= 0) && (z == 3))
							){
							setFlag(realWorld, neighbors[z], GLOWING);
						}
					}
				}
			}
		}
	}

	for(w=0; w<NOCAVES; w++){
			printf("%d. buraco coords: (%d, %d)\n",w,cavePos[w].x,cavePos[w].y);
	}

	for(w=0; w<NOWUMPUS; w++){
			printf("%d. wumpus coords: (%d, %d)\n",w,wumpusPos[w].x,wumpusPos[w].y);
	}
	for(w=0; w<NOGLOW; w++){
				printf("%d. ouro coords: (%d, %d)\n",w,glowPos[w].x,glowPos[w].y);
	}
	printf("\n");
}

void evaluateNeighbors(int agentWorld[NOCOLS][NOROWS], struct coord field){
	int z;
	struct coord neighbors[4];
	neighborFieldsCoords(field, neighbors);

	// descobrir suspeitas sobre algum perigo
	for(z=0; z<4; z++){
		if	(	(
					((neighbors[z].y < NOROWS) && (z == 0))
					||
					((neighbors[z].x < NOCOLS) && (z == 1))
					||
					((neighbors[z].y >= 0) && (z == 2))
					||
					((neighbors[z].x >= 0) && (z == 3))
				)
				&&
				(testFlag(agentWorld, neighbors[z], SAFE) == 0)
				&&
				(testFlag(agentWorld, neighbors[z], VISITED) == 0)
			){
			//marcar a brisa
			if(testFlag(agentWorld, field, BREEZE)){
				if(testFlag(agentWorld, neighbors[z], CAVESUS)){
					setFlag(agentWorld, neighbors[z], CAVE);
				}else{
					setFlag(agentWorld, neighbors[z], CAVESUS);
				}
			}
			//marcar como fedor
			if(testFlag(agentWorld, field, STENCH)){
				if(testFlag(agentWorld, neighbors[z], WUMPUSSUS)){
					setFlag(agentWorld, neighbors[z], WUMPUS);
				}else{
					setFlag(agentWorld, neighbors[z], WUMPUSSUS);
				}
			}
			//marcar como ouro
			if(testFlag(agentWorld, field, GLOWING)){
				if(testFlag(agentWorld, neighbors[z], GLOWSUS)){
					setFlag(agentWorld, neighbors[z], GLOW);
				}else{
					setFlag(agentWorld, neighbors[z], GLOWSUS);
				}
			}

			//marcar como seguro
			if((testFlag(agentWorld, field, BREEZE) == 0) &&
				(testFlag(agentWorld, field, STENCH) == 0)){
				setFlag(agentWorld, neighbors[z], SAFE);

				delFlag(agentWorld, neighbors[z], CAVESUS);
				delFlag(agentWorld, neighbors[z], WUMPUSSUS);
				delFlag(agentWorld, neighbors[z], CAVE);
				delFlag(agentWorld, neighbors[z], WUMPUS);
			}

		}
	}
}

void moveOneField(int agentWorld[NOCOLS][NOROWS], int direction, struct coord srcField ){
	struct coord temp;
	agentWorld[srcField.x][srcField.y] = agentWorld[srcField.x][srcField.y] & ~CURRENT;
	switch(direction){
		// cima
		case 0:
			temp.x = srcField.x;
			temp.y = srcField.y+1;
			setFlag(agentWorld, temp, CURRENT);
			printf("Para Cima!\n");
			break;
		// direita
		case 1:
			temp.x = srcField.x+1;
			temp.y = srcField.y;
			setFlag(agentWorld, temp, CURRENT);
			printf("Para Direita!\n");
			break;
		// baixo
		case 2:
			temp.x = srcField.x;
			temp.y = srcField.y-1;
			setFlag(agentWorld, temp, CURRENT);
			printf("Para Baixo!\n");
			break;
		// esquerda
		case 3:
			temp.x = srcField.x-1;
			temp.y = srcField.y;
			setFlag(agentWorld, temp, CURRENT);
			printf("Para Esquerda!\n");
			break;
	}
}

double calcDistance(int agentWorld[NOCOLS][NOROWS], struct coord srcField, struct coord destField){
	double distance;
	distance = sqrt((double)pow((double)destField.x-srcField.x,2)+(double)pow((double)destField.y-srcField.y,2));
	return distance;
}

// Calculate direction to go from srcField to destField
int calcDirrection(int agentWorld[NOCOLS][NOROWS], struct coord srcField, struct coord destField){
	int z, i, shortestGoodNeighbor;
	double shortestDist;
	struct coord neighbors[4];
	int goodNeighbors_i;				
	int goodNeighbors_size;			

	int *goodNeighbors = NULL;			
	int *goodNeighbors_temp = NULL;

	double *distances = NULL;			
	double *distances_temp = NULL;	
	neighborFieldsCoords(srcField, neighbors);

	goodNeighbors_i = 0;
	goodNeighbors_size = 0;

	for(z=0; z<4; z++){
		if	(	(
				((neighbors[z].y < NOROWS) && (z == 0))
				||
				((neighbors[z].x < NOCOLS) && (z == 1))
				||
				((neighbors[z].y >= 0) && (z == 2))
				||
				((neighbors[z].x >= 0) && (z == 3))
				)
				&&
				(testFlag(agentWorld, neighbors[z], VISITED))
			){
			if(goodNeighbors_i == 0){
				goodNeighbors_size++;
				if ((goodNeighbors = (int*)calloc(goodNeighbors_size, sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					exit(EXIT_FAILURE);
				}
				if ((distances = (double*)calloc(goodNeighbors_size, sizeof(double))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					exit(EXIT_FAILURE);
				}
			}else if(goodNeighbors_i == goodNeighbors_size){
				goodNeighbors_size++;
				if ((goodNeighbors_temp = (int*)realloc(goodNeighbors, goodNeighbors_size * sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					free(goodNeighbors);
					exit(EXIT_FAILURE);
				}
				if ((distances_temp = (double*)realloc(distances, goodNeighbors_size * sizeof(double))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					free(distances);
					exit(EXIT_FAILURE);
				}
				goodNeighbors = goodNeighbors_temp;
				distances = distances_temp;
			}
			goodNeighbors[goodNeighbors_i] = z;

			distances[goodNeighbors_i] = calcDistance(agentWorld,neighbors[z], destField);
			goodNeighbors_i++;
		}
	}

	if(goodNeighbors != 0){
		shortestGoodNeighbor = goodNeighbors[0];
		shortestDist = distances[0];
		for(i=1;i<goodNeighbors_i;i++){
			if(distances[i] < shortestDist){
				shortestGoodNeighbor = goodNeighbors[i];
				shortestDist = distances[i];
			}
		}
	}
	free(goodNeighbors);
	free(distances);
	return shortestGoodNeighbor;
}

int takeGlowAction(int agentWorld[NOCOLS][NOROWS], struct coord field){
	int z;
	struct coord neighbors[4];	
	neighborFieldsCoords(field, neighbors);

	for(z=0; z<4; z++){
		if	(	(
					((neighbors[z].y < NOROWS) && (z == 0))
					||
					((neighbors[z].x < NOCOLS) && (z == 1))
					||
					((neighbors[z].y >= 0) && (z == 2))
					||
					((neighbors[z].x >= 0) && (z == 3))
				)
				&&
				(
					testFlag(agentWorld, neighbors[z], GLOW)
				)
			){
				printf("Ouro pode estar por perto, vamso checar!\n");
				moveOneField(agentWorld, z, field);
				return(1);
		}
	}
	return(0);
}


int takeSafeAction(int agentWorld[NOCOLS][NOROWS], struct coord field){
	int random,z;
	struct coord neighbors[4];		
	neighborFieldsCoords(field, neighbors);

	int except_i;				
	int except_size;			

	int *exceptSF = NULL;			
	int *exceptSF_temp = NULL;

	except_i = 0;
	except_size = 0;

	for(z=0; z<4; z++){
		if	(	(
					((neighbors[z].y > NOROWS-1) && (z == 0))
					||
					((neighbors[z].x > NOCOLS-1) && (z == 1))
					||
					((neighbors[z].y < 0) && (z == 2))
					||
					((neighbors[z].x < 0) && (z == 3))
				)
				||
				(testFlag(agentWorld, neighbors[z], VISITED))
				||
				(testFlag(agentWorld, neighbors[z], SAFE) == 0)
				){
			if(except_i == 0){
				except_size++;
				if ((exceptSF = (int*)calloc(except_size, sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					exit(EXIT_FAILURE);
				}
			}else if(except_i == except_size){
				except_size++;
				if ((exceptSF_temp = (int*)realloc(exceptSF, except_size * sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					free(exceptSF);
					exit(EXIT_FAILURE);
				}
				exceptSF = exceptSF_temp;
			}
			exceptSF[except_i] = z;
			except_i++;
		}
	}

	if (except_i<4){
		random = randInt(0,3,exceptSF,except_i);

		moveOneField(agentWorld, random, field);
		free(exceptSF);
		return(1);
	}else{
		free(exceptSF);
		int x,y,direction;
		struct coord nearestSafeFieldCoord = { -1, -1 };

		
		for(x=0; x<NOROWS; x++){
			for(y=0; y<NOCOLS; y++){
				struct coord testSafeFieldCoord = { x, y };
				if(testFlag(agentWorld, testSafeFieldCoord, SAFE)){
					if((nearestSafeFieldCoord.x == -1) && (nearestSafeFieldCoord.y == -1)){
						nearestSafeFieldCoord = testSafeFieldCoord;
					}else{
						if(calcDistance(agentWorld,field,testSafeFieldCoord) < calcDistance(agentWorld, field,nearestSafeFieldCoord)){
							nearestSafeFieldCoord = testSafeFieldCoord;
						}
					}
				}
			}
		}

		if((nearestSafeFieldCoord.x == -1) && (nearestSafeFieldCoord.y == -1)){
			return(0);
		} else{
			direction = calcDirrection(agentWorld,field,nearestSafeFieldCoord);
			moveOneField(agentWorld,direction,field);
			return(1);
		}

	}
}

int takeRollTheDiceAction(int agentWorld[NOCOLS][NOROWS], struct coord field){
	int random,z;

	struct coord neighbors[4];
	neighborFieldsCoords(field, neighbors);

	int except_i;
	int except_size;

	int *exceptRND = NULL;
	int *exceptRND_temp = NULL;


	except_i = 0;
	except_size = 0;

	for(z=0; z<4; z++){
		if	(	(
					((neighbors[z].y > NOROWS-1) && (z == 0))
					||
					((neighbors[z].x > NOCOLS-1) && (z == 1))
					||
					((neighbors[z].y < 0) && (z == 2))
					||
					((neighbors[z].x < 0) && (z == 3))
				)
				||
				(testFlag(agentWorld, neighbors[z], VISITED))
				||
				(testFlag(agentWorld, neighbors[z], CAVE))
				||
				(testFlag(agentWorld, neighbors[z], WUMPUS))
				){
			if(except_i == 0){
				except_size++;
				if ((exceptRND = (int*)calloc(except_size, sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					exit(EXIT_FAILURE);
				}
			}else if(except_i == except_size){
				except_size++;
				if ((exceptRND_temp = (int*)realloc(exceptRND, except_size * sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					free(exceptRND);
					exit(EXIT_FAILURE);
				}
				exceptRND = exceptRND_temp;
			}
			exceptRND[except_i] = z;
			except_i++;
		}
	}

	if (except_i<4){
		random = randInt(0,3,exceptRND,except_i);

		printf("Role o dado!\n");
		moveOneField(agentWorld, random, field);
		free(exceptRND);
		return(1);
	} else{
		free(exceptRND);
		return(0);
	}
}

int takeSuicideAction(int agentWorld[NOCOLS][NOROWS], struct coord field){
	int random,z;

	struct coord neighbors[4];
	neighborFieldsCoords(field, neighbors);

	int except_i;
	int except_size;

	int *exceptRND = NULL;
	int *exceptRND_temp = NULL;


	except_i = 0;
	except_size = 0;

	for(z=0; z<4; z++){
		if	(	(
					((neighbors[z].y > NOROWS-1) && (z == 0))
					||
					((neighbors[z].x > NOCOLS-1) && (z == 1))
					||
					((neighbors[z].y < 0) && (z == 2))
					||
					((neighbors[z].x < 0) && (z == 3))
				)
				||
				(testFlag(agentWorld, neighbors[z], VISITED))
				){
			if(except_i == 0){
				except_size++;
				if ((exceptRND = (int*)calloc(except_size, sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					exit(EXIT_FAILURE);
				}
			}else if(except_i == except_size){
				except_size++;
				if ((exceptRND_temp = (int*)realloc(exceptRND, except_size * sizeof(int))) == NULL) {
					fprintf(stderr, "ERRO.\n");
					free(exceptRND);
					exit(EXIT_FAILURE);
				}
				exceptRND = exceptRND_temp;
			}
			exceptRND[except_i] = z;
			except_i++;
		}
	}

	if (except_i<4){
		random = randInt(0,3,exceptRND,except_i);

		printf("Commeteu suicidio!\n");
		moveOneField(agentWorld, random, field);
		free(exceptRND);
		return(1);
	} else{
		free(exceptRND);
		return(0);
	}
}

void myPause (void){
  printf ( "Pressione [Enter] para continuar . . ." );
  fflush ( stdout );
  getchar();
}