#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// //change row and column value to set the canvas size
int row = 1000;
int col = 1000;
int NUMTHREADS = 7;
int auxSection = 142;

//creates row boundary
int row_line(){
    printf("\n");
    for(int i=0; i<col; i++){printf(" -----");}
    printf("\n");
}

int check_sum(int **matrix) {
	int sum = 0;
	for(int i = 0; i < row; i++) {
		for(int j = 0; j < col; j++) {
			sum += matrix[i][j];
		}
	}
	return sum;
}
 
//returns the count of alive neighbours
int count_live_neighbour_cell(int **matrix, int r, int c){
    int i, j, count=0;
    for(i=r-1; i<=r+1; i++){
        for(j=c-1;j<=c+1;j++){
            if((i==r && j==c) || (i<0 || j<0) || (i>=row || j>=col)){
                continue;
            }
            if(matrix[i][j]==1){
                count++;
            }
        }
    }
    return count;
}
 
//print matrix
void print_matrix(int **matrix) {
	int i, j;
	row_line();
    for(i=0; i<row; i++){
        printf(":");
        for(j=0;j<col;j++){
            printf("  %d  :", matrix[i][j]);
        }
        row_line();
    }
}

//return 1 -> game over, 0 -> is alive
int check_game_over(int **matrix) {
	int i,j;

	for (i=0;i<row;i++) {
		for(j=0;j<col;j++) {
			if (matrix[i][j] == 1) {
				return 0;
			}
		}
	}
	return 1;
}

typedef struct
{
    long tid;
    int **matrix;
    int **next_matrix;
} dataPack;



void *thread_func(void *arg)
{
    dataPack *data = (dataPack *)arg;
    long tid = data->tid;
    int **matrix = data->matrix;
    int **next_matrix = data->next_matrix;
    int i, j;
    int neighbour_live_cell;
	//--------------------------------------------------- 
	for(i=tid*auxSection;i<
	((tid == NUMTHREADS - 1) ? (auxSection * (tid+1) + row%auxSection) : (auxSection * (tid+1)))
	;i++){
		for(j=0;j<col;j++){
			neighbour_live_cell = count_live_neighbour_cell(matrix,i,j);
			if(matrix[i][j]==1 && (neighbour_live_cell==2 || neighbour_live_cell==3)){
				next_matrix[i][j]=1;
			}

			else if(matrix[i][j]==0 && neighbour_live_cell==3){
				next_matrix[i][j]=1;
			}

			else{
				next_matrix[i][j]=0;
			}
		}
	}

    return NULL;
}

//compileaza: gcc game_of_life.c -o game_of_life
//ruleaza: ./game_of_life input.txt
int main(int argc, char *argv[]){
	clock_t start = clock();

	// code to be measured


    int **matrix = (int**)malloc(col*sizeof(int*));
	for(int i=0;i<col;i++){
		matrix[i] = (int*)malloc(row*sizeof(int));
	}
    int **next_matrix = (int**)malloc(col*sizeof(int*));
	for(int i=0;i<col;i++){
		next_matrix[i] = (int*)malloc(row*sizeof(int));
	}

	char line[row + 1];
    int i,j,k, generations = 10;
 	FILE *f;

	//read input matrix from input file
	f = fopen(argv[1], "r");
	for (j=0;j<col;j++) {

		fgets (line, row + 1, f);

		for (i=0;i<row;i++) {
			matrix[j][i] = line[i] - '0';
		}
	}
	fclose (f);
     
    //print array matrix
    //printf("Initial Stage:");
    //print_matrix(matrix);
	
    pthread_t threads[NUMTHREADS];
    dataPack data[NUMTHREADS];

	// //repeat for all generations
	for(k=0;k<generations;k++) {

		for (i = 0; i < NUMTHREADS; i++)
		{
			data[i].tid = i;
			data[i].matrix = matrix;
			data[i].next_matrix = next_matrix;
			pthread_create(&threads[i], NULL, thread_func, (void *)&data[i]);
		}

		// Wait for the threads to finish
		for (i = 0; i < NUMTHREADS; i++)
		{
			pthread_join(threads[i], NULL);
		}

		//check if game is still alive
		if (check_game_over(next_matrix) == 1) {
			break;
		}

		//save next generation in current generation
		for(i=0;i<row;i++) {
			for(j=0;j<col;j++) {
				matrix[i][j] = next_matrix[i][j];
				next_matrix[i][j]=0;
			}
		}
		
	}
		printf("\n rez e serial %d \n",check_sum(matrix));
	for(int i=0;i<col;i++){
		free(matrix[i]);
		free(next_matrix[i]);
	}
	free(matrix);
	free(next_matrix);
	
	//printf("GAME OVER\n");
	clock_t end = clock();
	double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Elapsed time: %f seconds\n", elapsed_time);

    return 0;
}