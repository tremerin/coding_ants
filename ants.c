#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct s_cell{
    int rsc;    //recursos
    int mya;    //mis hormias
    int opa;    //hormigas oponente  
    int typ;    //tipo, 0>vacia 1>huevos 2>cristales  
    int n[6];   //casillas vecinas
    int mys_d;
    int opb_d;
    bool listed;
}   t_cell;

typedef struct s_path{
    int *path;
    int *objetives;
    int cells_with_resources;
    int path_len;
    int max_len;
    int base_pos;
}   t_path;

void count_dist(t_cell *cells, int ini_pos, int number_of_cells) //revisar distancias
{
    int *possitions = malloc(sizeof(int) * number_of_cells);
    possitions[0] = ini_pos;
    int dist = 0;
    int find = 0;
    int write = 1;
    int cell;
    int i = 0;
    cells[ini_pos].mys_d = dist;
    cells[ini_pos].listed = true;
    while (find < write)
    {
        i = 0;
        dist = cells[possitions[find]].mys_d + 1;
        while (i < 6)
        {
            cell = cells[possitions[find]].n[i];
            if (cell != -1)
            {
                if (cells[cell].listed == false)
                {
                    possitions[write] = cell;
                    cells[cell].listed = true;
                    cells[cell].mys_d = dist;
                    write++;
                }
            }
            i++;
        }
        find++;
    }
}

void count_dist_all(t_cell *cells, int ini_pos, int number_of_cells, int **distances) //calcula las distancias entre todas la casillas 
{
    int *possitions = malloc(sizeof(int) * number_of_cells);
    possitions[0] = ini_pos;
    int dist = 0;
    int find = 0;
    int write = 1;
    int cell;
    int i = 0;
    cells[ini_pos].listed = true;
    cells[ini_pos].mys_d = 0;
    distances[ini_pos][ini_pos] = 0;
    while (find < write)
    {
        i = 0;
        dist = cells[possitions[find]].mys_d + 1;
        while (i < 6)
        {
            cell = cells[possitions[find]].n[i];
            if (cell != -1)
            {
                if (cells[cell].listed == false)
                {
                    possitions[write] = cell;
                    cells[cell].listed = true;
                    cells[cell].mys_d = dist;
                    distances[ini_pos][cell] = dist;
                    write++;
                }
            }
            i++;
        }
        find++;
    }
}

int get_dist(int cell1, int cell2, int **distances) //devuelve la distancia entre dos casillas
{
    return (distances[cell1][cell2]);
}

void unlisted(t_cell *cells, int number_of_cells) //listed = false para toda las casillas
{
    for (int i = 0; i < number_of_cells; i++){
        cells[i].listed = false;
    }
}

int next_resource(int cell, t_path *path, int **distances) //devuelve la  casilla con recursos mas cercana a la casilla indicada
{
    int next = path->objetives[0];
    int next_dist = get_dist(cell, path->objetives[0], distances);
    int comp_dist;
    int i = 0;
    while (i < path->cells_with_resources) 
    {
        if (path->objetives[i] != -1)
        {
            comp_dist =  get_dist(cell, path->objetives[i], distances);
            if (comp_dist < next_dist)
            {
                next_dist = comp_dist;
                next = path->objetives[i];
            }
        }
        i++;
    }
    return (next);
}

int next_neighbor(t_cell *cells, int cell1, int cell2, int **distances) //devuelve el vecino de la casilla 2 mas cercano a la casilla 1
{
    int next;
    int next_dist = 100;
    int comp_dist;
    int i = 0;
    while (i < 6)
    {
        if (cells[cell2].n[i] != -1)
        {
            comp_dist = get_dist(cell1, cells[cell2].n[i], distances);
            if (comp_dist < next_dist)
            {
                next_dist = comp_dist;
                next = cells[cell2].n[i];
            }
        }
        i++;
    }
    return (next);
}

int is_neighbor(int **distances, int cell1, int cell2) // devuelve 1 si la casilla1 es vecina de la casilla2
{
    if (get_dist(cell1, cell2, distances) == 1)
        return (1);
    return (0);
}

void set_path(t_cell *cells, t_path *path, int **distances) //crea la ruta a recorrer
{
    int next = next_resource(path->base_pos, path, distances);
    path->path[1] = next;
    path->path_len = 2;
    while (is_neighbor(distances, path->path[0], next) == 0)
    {
        next = next_neighbor(cells, path->path[0], next, distances);
        path->path[path->path_len] = next;
        path->path_len++;
    }
}

void print_path(t_path *path) //printf de todas las balizas del camino
{
    int i = 0;
    while (i < path->path_len)
    {
        // BEACON <cellIdx> <strength> \n
        printf("BEACON %d %d\n", path->path[i], 1);
        i++;
    }
}

int main()
{
    int obj_cell;
    int force = 1;
    t_path  path;
    // amount of hexagonal cells in this map
    int number_of_cells;
    scanf("%d", &number_of_cells);
    int **distances = malloc(sizeof(int*) * number_of_cells);
    for (int i = 0; i < number_of_cells; i++){
        distances[i] = malloc(sizeof(int) * number_of_cells);
    }
    t_cell *cells= malloc(sizeof(t_cell) * number_of_cells);
    for (int i = 0; i < number_of_cells; i++) { //leer casillas vecinas de cada casilla, recursos y tipo
        scanf("%d%d%d%d%d%d%d%d", &cells[i].typ, &cells[i].rsc, &cells[i].n[0], &cells[i].n[1], &cells[i].n[2], &cells[i].n[3], &cells[i].n[4], &cells[i].n[5]);
        cells[i].listed = false;
        if (cells[i].rsc > 0) //contamos el numero de objetivos
            path.cells_with_resources++;
    }
    path.objetives = malloc(sizeof(int) * path.cells_with_resources);
    int number_of_bases;
    scanf("%d", &number_of_bases);
    for (int i = 0; i < number_of_bases; i++) { //mi base
        int my_base_index;
        scanf("%d", &my_base_index);
        path.base_pos = my_base_index;
    }
    for (int i = 0; i < number_of_bases; i++) { //base enemiga
        int opp_base_index;
        scanf("%d", &opp_base_index);
    }
    for (int i = 0; i < number_of_cells; i++) //calcudo de distancias entre casillas
    {
        unlisted(cells, number_of_cells);
        count_dist_all(cells, i, number_of_cells, distances);
    }
    //count_dist(cells, base_pos, number_of_cells); //calculo de distancia con la base
    path.path = malloc(sizeof(int) * number_of_cells);   //casillas del camino
    path.max_len = 10;                                    //tamañano maximo del camino, #### TODO calcular dependiendo de las hormigas ####
    path.path_len = 1;                                    //largo del camino
    int objetive_cell;
    path.path[0] = path.base_pos;                                  //primera casilla es nuestra  base

    while (1) { //################ game loop ##################
        path.cells_with_resources = 0;
        for (int i = 0; i < number_of_cells; i++) { //lectura de recursos(rsc), mis hormigas(mya) y hormigas enemigas(opa) por casilla
            scanf("%d%d%d", &cells[i].rsc, &cells[i].mya, &cells[i].opa);
            if (cells[i].rsc > 0)
            {
                path.cells_with_resources++;
                path.objetives[path.cells_with_resources] = i; //lista con las casillas que continen recursos
            }
        }
        //##### Calcular el orden de los objetivos y el camino #####
        // 1> buscar el objetivo mas cercano
        // 2> buscar las casillas entre la base y objetivo
        unlisted(cells, number_of_cells);
        set_path(cells, &path, distances);
        print_path(&path);
        //int nextbase = next_resource(path.base_pos, &path, distances);
        //fprintf(stderr, "objetive: %d\n", nextbase);

        // WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>
        //printf("LINE %d %d %d\n", path.base_pos, obj_cell, force);
    }

    return 0;
}
