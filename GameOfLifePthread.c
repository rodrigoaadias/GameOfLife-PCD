#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define N 2048
#define N_THREADS 4

typedef struct
{
    int **currentGrid;
    int **newGrid;
    int start;
} TData;

void FillGlider(int **grid)
{
    // GLIDER
    int lin = 1, col = 1;

    grid[lin][col + 1] = 1;
    grid[lin + 1][col + 2] = 1;
    grid[lin + 2][col] = 1;
    grid[lin + 2][col + 1] = 1;
    grid[lin + 2][col + 2] = 1;
}

void FillRPentonimo(int **grid)
{
    // R-pentomino
    int lin = 10;
    int col = 30;
    grid[lin][col + 1] = 1;
    grid[lin][col + 2] = 1;
    grid[lin + 1][col] = 1;
    grid[lin + 1][col + 1] = 1;
    grid[lin + 2][col + 1] = 1;
}

void PrintGrid(int **grid)
{
    int i, j;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            char c = grid[i][j] == 1 ? '*' : '.';
            printf("%c", c);
        }

        printf("\n");
    }
    printf("\n\n\n");
}

int GetAliveNeighbors(int **grid, int line, int column)
{
    int alive = 0;
    int j = 0;
    int current = 0;

    // obter índice de linha acima
    int above = line == 0 ? N - 1 : line - 1;

    // checar linha de cima
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        current = j;
        if (current < 0)
            current = N - 1;

        if (current > N - 1)
            current = 0;

        if (grid[above][current] == 1)
            alive++;
    }

    // obter linha abaixo
    int below = (line + 1) % N;

    // checar linha de baixo
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        current = j;
        if (current < 0)
            current = N - 1;

        if (current > N - 1)
            current = 0;

        if (grid[below][current] == 1)
            alive++;
    }

    // checar esquerda
    int left = column > 0 ? column - 1 : N - 1;
    if (grid[line][left] == 1)
        alive++;

    // checar direita
    int right = column < N - 1 ? column + 1 : 0;
    if (grid[line][right] == 1)
        alive++;

    return alive;
}

int GetNewState(int **grid, int line, int column)
{
    int neighbors = GetAliveNeighbors(grid, line, column);

    // A célula está viva?
    if (grid[line][column] == 1)
    {
        // mantém-se viva
        if (neighbors == 2 || neighbors == 3)
            return 1;

        // deve morrer
        return 0;
    }

    // célula está morta
    // tem vizinhos suficientes para viver?
    if (neighbors == 3)
        return 1;

    return 0;
}

int GetSurvivors(int **grid)
{
    int alive = 0;
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            alive += grid[i][j];
        }
    }

    return alive;
}

void ShowGeneration(int **grid, int currentGeneration)
{
    printf("Geração %d: %d\n", currentGeneration, GetSurvivors(grid));
}

int **GetCurrentGrid(int **gridA, int **gridB, int iteration)
{
    if (iteration % 2 == 0)
        return gridA;
    else
        return gridB;
}

int **GetNextGrid(int **gridA, int **gridB, int iteration)
{
    if (iteration % 2 == 0)
        return gridB;
    else
        return gridA;
}

void *Thread_ProccessGeneration(void *arg)
{
    int i, j;
    TData *data = (TData *)arg;

    // FOR acrescenta num threads no loop
    for (i = data->start; i < N; i += N_THREADS)
    {
        // printf("Thread %d: running loop %d \n", data->start, i);
        for (j = 0; j < N; j++)
        {
            data->newGrid[i][j] = GetNewState(data->currentGrid, i, j);
        }
    }

    pthread_exit(NULL);
}

void PlayGameOfLife(int **gridA, int **gridB, int iterations)
{
    int i, k;
    char *thread_num = getenv("OMP_NUM_THREADS");
    int th_num = (thread_num) ? atoi(thread_num) : N_THREADS;

    printf("Numero de threads: %d\n\n", th_num);

    pthread_t threads[th_num];
    TData datas[th_num];

    for (k = 0; k < iterations; k++)
    {
        int **nextGrid = GetNextGrid(gridA, gridB, k);
        int **currentGrid = GetCurrentGrid(gridA, gridB, k);

        /*if (k < 5)
            PrintGrid(currentGrid);*/

        for (i = 0; i < th_num; i++)
        {
            datas[i].currentGrid = currentGrid;
            datas[i].newGrid = nextGrid;
            datas[i].start = i;
            pthread_create(&threads[i], NULL, Thread_ProccessGeneration, (void *)&datas[i]);
        }

        for (i = 0; i < th_num; i++)
        {
            pthread_join(threads[i], NULL);
        }
    }
}

int main()
{
    int **gridA, **gridB;

    gridA = (int **)malloc(N * sizeof(int *));
    gridB = (int **)malloc(N * sizeof(int *));

    struct timeval start, end;
    double time_ms;

    int i = 0, j = 0;
    for (i = 0; i < N; i++)
    {
        gridA[i] = (int *)malloc(N * sizeof(int));
        gridB[i] = (int *)malloc(N * sizeof(int));

        for (j = 0; j < N; j++)
        {
            gridA[i][j] = 0;
            gridB[i][j] = 0;
        }
    }

    FillGlider(gridA);
    FillRPentonimo(gridA);

    printf("*** High Life (pthread)\n");
    printf("Condição inicial: %d\n", GetSurvivors(gridA));

    gettimeofday(&start, NULL);
    PlayGameOfLife(gridA, gridB, 2000);
    gettimeofday(&end, NULL);

    time_ms = (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000);

    printf("Última geração (2000 iterações): %d\n", GetSurvivors(gridB));
    printf("Tempo execução: %lf ms\n", time_ms / 1000);

    pthread_exit(NULL);

    return 0;
}