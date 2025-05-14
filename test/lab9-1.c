#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    int cost;
    int distance;
} Cell;

typedef struct {
    Cell** data;
    int rows;
    int cols;
} Grid;

Grid* create_grid(int rows, int cols) {
    Grid* grid = malloc(sizeof(Grid));
    if (!grid) return NULL;
    grid->data = malloc(rows * sizeof(Cell*));
    if (!grid->data) {
        free(grid);
        return NULL;
    }
    grid->rows = rows;
    grid->cols = cols;
    for (int i = 0; i < rows; i++) {
        grid->data[i] = malloc(cols * sizeof(Cell));
        if (!grid->data[i]) {
            for (int j = 0; j < i; j++)
                free(grid->data[j]);
            free(grid->data);
            free(grid);
            return NULL;
        }
    }
    return grid;
}

void free_grid(Grid* grid) {
    if (!grid) return;
    for (int i = 0; i < grid->rows; i++)
        free(grid->data[i]);
    free(grid->data);
    free(grid);
}

int read_input(Grid** grid, int* sr, int* sc, int* er, int* ec, FILE* in) {
    int rows, cols;
    if (fscanf(in, "%d %d", &rows, &cols) != 2) return 1;

    if (rows < 1 || cols < 1 || rows > 1000 || cols > 1000) return 2;

    int c;
    while ((c = fgetc(in)) != EOF && c != '\n');

    Grid* g = create_grid(rows, cols);
    if (!g) return 3;

    char buffer[1024];
    for (int i = 0; i < rows; i++) {
        if (!fgets(buffer, sizeof(buffer), in)) {
            free_grid(g);
            return 1;
        }
        char* ptr = buffer;
        int j;
        for (j = 0; j < cols; j++) {
            char* endptr;
            long num = strtol(ptr, &endptr, 10);
            if (endptr == ptr) {
                free_grid(g);
                return 1;
            }
            g->data[i][j].cost = (int)num;
            ptr = endptr;
        }
        while (*ptr && *ptr != '\n') {
            if (!isspace((unsigned char)*ptr)) {
                free_grid(g);
                return 1;
            }
            ptr++;
        }
    }

    if (fscanf(in, "%d %d %d %d", sr, sc, er, ec) != 4) {
        free_grid(g);
        return 1;
    }

    if (*sr <0 || *sc <0 || *er <0 || *ec <0 ||
        *sr >= rows || *sc >= cols || *er >= rows || *ec >= cols) {
        free_grid(g);
        return 4;
    }

    if (g->data[*sr][*sc].cost == -1) return 5;

    *grid = g;
    return 0;
}

void lee(Grid* grid, int sr, int sc) {
    int max_size = grid->rows * grid->cols;
    int* qr = malloc(max_size * sizeof(int));
    int* qc = malloc(max_size * sizeof(int));
    if (!qr || !qc) {
        free(qr);
        free(qc);
        return;
    }

    for (int i = 0; i < grid->rows; i++)
        for (int j = 0; j < grid->cols; j++)
            grid->data[i][j].distance = -1;

    int front = 0, rear = 0;
    grid->data[sr][sc].distance = 0;
    qr[rear] = sr;
    qc[rear++] = sc;

    const int dr[] = {0, 0, 1, -1};
    const int dc[] = {1, -1, 0, 0};

    while (front < rear) {
        int r = qr[front], c = qc[front++];
        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < grid->rows && nc >= 0 && nc < grid->cols &&
                grid->data[nr][nc].cost != -1 && grid->data[nr][nc].distance == -1) {
                grid->data[nr][nc].distance = grid->data[r][c].distance + 1;
                qr[rear] = nr;
                qc[rear++] = nc;
            }
        }
    }

    free(qr);
    free(qc);
}

int main() {
    FILE* in = fopen("in.txt", "r");
    FILE* out = fopen("out.txt", "w");
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    Grid* grid = NULL;
    int sr, sc, er, ec;
    int error = read_input(&grid, &sr, &sc, &er, &ec, in);
    const char* errors[] = {
            "Invalid data format",
            "Invalid grid size",
            "Memory error",
            "Invalid coordinates",
            "Start is wall"
    };

    if (error) {
        fprintf(out, "%s\n", errors[error-1]);
        fclose(in);
        fclose(out);
        return 0;
    }

    if (grid->data[er][ec].cost == -1) {
        fprintf(out, "No path found\n");
        free_grid(grid);
        fclose(in);
        fclose(out);
        return 0;
    }

    lee(grid, sr, sc);
    if (grid->data[er][ec].distance == -1)
        fprintf(out, "No path found\n");
    else
        fprintf(out, "%d\n", grid->data[er][ec].distance);

    free_grid(grid);
    fclose(in);
    fclose(out);
    return 0;
}
