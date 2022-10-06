#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 50

// Definir sempre a quantidade de threads
#define MAXThread 8

// Struct para o argumento para mandar para a thread
typedef struct
{
    int **C_grid;
    int **N_grid;
    int inicio;
} Matrizes_arg;

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
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            char c = grid[i][j] == 1 ? '*' : ' ';
            printf("[%c]", c);
        }

        printf("\n");
    }
}

int GetAliveNeighbors(int **grid, int line, int column)
{
    int alive = 0;
    int j;

    // obter índice de linha acima
    int above = line == 0 ? N - 1 : line - 1;

    // checar linha de cima
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        int current = j % N;
        if (current < 0)
            current = N - 1;

        if (grid[above][current] == 1)
            alive++;
    }

    // obter linha abaixo
    int below = (line + 1) % N;

    // checar linha de baixo
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        int current = j % N;
        if (current < 0)
            current = N - 1;

        if (grid[below][current] == 1)
            alive++;
    }

    // checar esquerda
    int left = column > 0 ? column - 1 : N - 1;
    if (grid[line][left] == 1)
        alive++;

    // checar direita
    int right = (column + 1) % N;
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

void ShowGeneration(int **grid, int currentGeneration)
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

    printf("Geração %d: %d\n", currentGeneration, alive);
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

// Função realizada pela thread
void *Thread_PlayGameOfLife(void *Strutura)
{ // A quantidade de colunas deve ser >= ao número de threads

    // estrutura que recebe o parãmetro
    Matrizes_arg Structe = *(Matrizes_arg *)Strutura;

    // j começa com o valor da casa inicial que a thread vai percorrer
    int i, j = Structe.inicio, var;

    for (i = 0; i < N; i++)
    {
        // j é iniciado antes do for e resetamos a nova posição de j logo após este segundo for
        // j é acrecentado de MAXThread em MAXThread (exemplo: de 8 em 8 )
        for (; j < N; j += MAXThread)
        {
            Structe.N_grid[i][j] = GetNewState(Structe.C_grid, i, j);
        }

        // j é o exedente referente a quantidade de colunas após o for
        // exemplo se j é 52, na proxima linah deve começar na coluna 1:   j = 52 - 50 -1 = 1
        // MAXThread é irrelevante nesse caso pois já é somado com o excedente no fim do for
        // Se j=N então ele deve começar na coluna MAXThread -1
        j = j == N ? MAXThread - 1 : j - N - 1;
    }

    pthread_exit(NULL);
}

void PlayGameOfLife(int **gridA, int **gridB, int iterations)
{
    // Cria um argumento para cada thread, eles irão compartilhar os grids A e B, mas terão um valor inicial diferente
    Matrizes_arg strutura_1;
    Matrizes_arg strutura_2;
    Matrizes_arg strutura_3;
    Matrizes_arg strutura_4;
    Matrizes_arg strutura_5;
    Matrizes_arg strutura_6;
    Matrizes_arg strutura_7;
    Matrizes_arg strutura_8;

    // Instancia variaveis para a threads
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;
    pthread_t t5;
    pthread_t t6;
    pthread_t t7;
    pthread_t t8;

    int i, j, k, create;
    for (k = 0; k < iterations; k++)
    {
        // int **nextGrid = GetNextGrid(gridA, gridB, k);
        // int **currentGrid = GetCurrentGrid(gridA, gridB, k);

        // ShowGeneration(currentGrid, k);

        // Vai arrumando os argumentos
        strutura_1.C_grid = GetCurrentGrid(gridA, gridB, k);
        strutura_1.N_grid = GetNextGrid(gridA, gridB, k);
        strutura_1.inicio = 0;

        ShowGeneration(strutura_1.C_grid, k);

        strutura_2.C_grid = strutura_1.C_grid;
        strutura_2.N_grid = strutura_1.N_grid;
        strutura_2.inicio = 1;

        strutura_3.C_grid = strutura_1.C_grid;
        strutura_3.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 2;

        strutura_4.C_grid = strutura_1.C_grid;
        strutura_4.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 3;

        strutura_5.C_grid = strutura_1.C_grid;
        strutura_5.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 4;

        strutura_6.C_grid = strutura_1.C_grid;
        strutura_6.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 5;

        strutura_7.C_grid = strutura_1.C_grid;
        strutura_7.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 6;

        strutura_8.C_grid = strutura_1.C_grid;
        strutura_8.N_grid = strutura_1.N_grid;
        strutura_1.inicio = 7;

        // Cria e roda as trheads
        create = pthread_create(&t1, NULL, Thread_PlayGameOfLife, &strutura_1);
        create = pthread_create(&t2, NULL, Thread_PlayGameOfLife, &strutura_2);
        create = pthread_create(&t3, NULL, Thread_PlayGameOfLife, &strutura_3);
        create = pthread_create(&t4, NULL, Thread_PlayGameOfLife, &strutura_4);
        create = pthread_create(&t5, NULL, Thread_PlayGameOfLife, &strutura_5);
        create = pthread_create(&t6, NULL, Thread_PlayGameOfLife, &strutura_6);
        create = pthread_create(&t7, NULL, Thread_PlayGameOfLife, &strutura_7);
        create = pthread_create(&t8, NULL, Thread_PlayGameOfLife, &strutura_8);

        // Espera as threads se juntarem
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
        pthread_join(t5, NULL);
        pthread_join(t6, NULL);
        pthread_join(t7, NULL);
        pthread_join(t8, NULL);

        /*for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                nextGrid[i][j] = GetNewState(currentGrid, i, j);
            }
        }*/
    }
}

int main()
{
    int **gridA, **gridB;
    gridA = (void *)malloc(N * sizeof(int));
    gridB = (void *)malloc(N * sizeof(int));

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

    PlayGameOfLife(gridA, gridB, 2000);

    ShowGeneration(GetCurrentGrid(gridA, gridB, 2000), 2000);

    return 0;
}