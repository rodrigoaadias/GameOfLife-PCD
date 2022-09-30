#include <stdio.h>
#include <stdlib.h>

#define N 50

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

    int above = line > 0 ? line - 1 : N - 1;
    // checar linha de cima
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        int current = j % N;
        if (j < 0)
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
        if (j < 0)
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

void UpdateCellState(int **currentGrid, int **newGrid, int line, int column)
{
    int neighbors = GetAliveNeighbors(currentGrid, line, column);

    // ser a célula está morta
    if (currentGrid[line][column] == 0)
    {
        if (neighbors == 3)
        {
            newGrid[line][column] = 1;
            return;
        }

        newGrid[line][column] = 0;
    }
    else // a célula está viva
    {
        if (neighbors == 2 || neighbors == 3)
        {
            newGrid[line][column] = 1;
            return;
        }

        newGrid[line][column] = 0;
    }
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

void PlayGameOfLife(int **gridA, int **gridB, int iterations)
{
    int i, j, k;
    for (k = 0; k < iterations; k++)
    {
        ShowGeneration(GetCurrentGrid(gridA, gridB, k), k);

        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                UpdateCellState(GetCurrentGrid(gridA, gridB, k),
                                GetNextGrid(gridA, gridB, k),
                                i, j);
            }
        }
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