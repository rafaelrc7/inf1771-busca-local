



// FILEIRAS E COLUNAS DO MAPA

#define ROW 9
#define COL 10

struct coords{  // coordenadas x e y
	int x;
	int y;
};

typedef struct coords Coords;

struct cost{  // coordenadas + custo de movimento
	double fcost;
	Coords pos;
};

typedef struct cost Cost;

struct cell{
	int parent_i, parent_y;
	double f, g, h;
};

typedef struct cell Cell;

int checkCell(int row, int col){

	if ((row >= 0) && (row < ROW) && (col >= 0) && (col < COL)){
		return 1;
	} else{
		return 0;
	}
}

int  cellCost(int grid[][COL], int row, int col){
	int cost = 0;
	if (grid[row][col] == 'M'){
		cost += 200;
	}
	else if(grid[row][col] == '.'){
		cost += 1;
	}
	else if (grid[row][col] == 'R'){
		cost += 5;
	}
	else if (grid[row][col] == 'V'){
		cost += 10;
	}
	else(grid[row][col] == 'A'){
		cost += 15;
	}
	return cost;
}

int isDest(int row, int col, Coords dest){
	if (row == dest.x && col == dest.y){
		return 1;
	}
	else{
		return 0;
	}

int gridCalculation(int row, int col, Coords dest){

	double h_dist = ((row - dest.x) * (row - dest.x) + (col - dest.y) * (col - dest.y));
	h_dist = sqrt(h_dist);
	return h_dist;
}

void fastestPath(cell cellDetails[][COL], Coords dest){
	int auxPrint = 0;
	int auxPath = 0;
	printf("\nThe Path is ");
    int row = dest.x;
    int col = dest.y;
 
    Coords Path[24600];
 
    while (!((cellDetails[row][col].parent_i == row) && (cellDetails[row][col].parent_j == col))) {
		Path[auxPath].x = row;
		Path[auxPath].y = col;
		auxPath++;
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }

	auxPath++;
    Path[auxPath].x = row;
	Path[auxPath].y = col;

    while (auxPath != 0) {
        Coords aux = Path[auxPrint];
        auxPrint++;
		auxPath--;
        printf("-> (%d,%d) ", aux.x, aux.y);
    }
 
    return;

}

void aSearch(grid[][COL], Coords start, Coords dest){
	int auxList = 0;
	int auxCount = 0;

	if (checkCell(start.x, start.y) == false){
		printf("Start point is invalid\n");
		return;
	}

	if (checkCell(dest.x, dest.y) == false){
		printf("Destination point is invalid\n");
		return;
	}

	if (isDest(start.x, start.y, dest) == true){
        printf("We are already at the destination\n");
        return;
    }

	bool closedList[ROW][COL];
	memset(closedList, false, sizeof(closedList));

	cell CellDetails[ROW][COL];

	int i, j;

	for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

	i = start.x, j = start.y;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

	Cost openList[24600];

	openList[auxList].fcost = 0.0;
	openList[auxList].pos.x = i;
	openList[auxList].pos.y = j;

	auxList++;

	bool destReach = false;

	while(auxList != 0){
		Cost aux = openList[auxCount];

		i = aux.pos.x;
		j = aux.pos.y;
		closedList[i][j] = true;
	


	double gNew, hNew, fNew;
	// OTHERS CELLS CHECK

	// NORTE
	if (checkCell(i - 1, j) == true) {
            if (isDest(i - 1, j, dest) == true) {
                cellDetails[i - 1][j].parent_i = i;
                cellDetails[i - 1][j].parent_j = j;
                printf("The destination cell is found\n");
                fastestPath(cellDetails, dest);
                foundDest = true;
                return;
            }
	
	else if (closedList[i - 1][j] == false){
                gNew = cellDetails[i][j].g + cellCost(grid, i - 1, j);
                hNew = gridCalculation(i - 1, j, dest);
                fNew = gNew + hNew;
	
				if ((cellDetails[i - 1][j].f == FLT_MAX) || (cellDetails[i - 1][j].f > fNew)){
						auxCount++;
                    	openList[auxCount].fcost = fNew;
						openList[auxCount].pos.x = i - 1;
						openList[auxCount].pos.y = j;
						auxList++;
 
                    	// Update the details of this cell
                    	cellDetails[i - 1][j].f = fNew;
                    	cellDetails[i - 1][j].g = gNew;
                    	cellDetails[i - 1][j].h = hNew;
                    	cellDetails[i - 1][j].parent_i = i;
                    	cellDetails[i - 1][j].parent_j = j;
                }
            }
	}

	// SUL
	if (checkCell(i + 1, j) == true) {
            if (isDest(i + 1, j, dest) == true) {
                cellDetails[i + 1][j].parent_i = i;
                cellDetails[i + 1][j].parent_j = j;
                printf("The destination cell is found\n");
                fastestPath(cellDetails, dest);
                foundDest = true;
                return;
            }
	else if (closedList[i + 1][j] == false){
                gNew = cellDetails[i][j].g + cellCost(grid, i + 1, j);
                hNew = gridCalculation(i + 1, j, dest);
                fNew = gNew + hNew;
	
				if ((cellDetails[i + 1][j].f == FLT_MAX) || (cellDetails[i + 1][j].f > fNew)){
						auxCount++;
                    	openList[auxCount].fcost = fNew;
						openList[auxCount].pos.x = i + 1;
						openList[auxCount].pos.y = j;
						auxList++;
 
                    	// Update the details of this cell
                    	cellDetails[i + 1][j].f = fNew;
                    	cellDetails[i + 1][j].g = gNew;
                    	cellDetails[i + 1][j].h = hNew;
                    	cellDetails[i + 1][j].parent_i = i;
                    	cellDetails[i + 1][j].parent_j = j;
                }
            }
	}

	// LESTE
	if (checkCell(i, j + 1) == true) {
            if (isDest(i, j + 1, dest) == true) {
                cellDetails[i][j + 1].parent_i = i;
                cellDetails[i][j + 1].parent_j = j;
                printf("The destination cell is found\n");
                fastestPath(cellDetails, dest);
                foundDest = true;
                return;
            }
	else if (closedList[i][j + 1] == false){
                gNew = cellDetails[i][j].g + cellCost(grid, i, j + 1);
                hNew = gridCalculation(i, j + 1, dest);
                fNew = gNew + hNew;
	
				if ((cellDetails[i][j + 1].f == FLT_MAX) || (cellDetails[i][j + 1].f > fNew)){
						auxCount++;
                    	openList[auxCount].fcost = fNew;
						openList[auxCount].pos.x = i;
						openList[auxCount].pos.y = j + 1;
						auxList++;
 
                    	// Update the details of this cell
                    	cellDetails[i][j + 1].f = fNew;
                    	cellDetails[i][j + 1].g = gNew;
                    	cellDetails[i][j + 1].h = hNew;
                    	cellDetails[i][j + 1].parent_i = i;
                    	cellDetails[i][j + 1].parent_j = j;
                }
            }
	}

	// OESTE
	if (checkCell(i, j - 1) == true) {
            if (isDest(i, j - 1, dest) == true) {
                cellDetails[i][j - 1].parent_i = i;
                cellDetails[i][j - 1].parent_j = j;
                printf("The destination cell is found\n");
                fastestPath(cellDetails, dest);
                foundDest = true;
                return;
            }
	else if (closedList[i][j - 1] == false){
                gNew = cellDetails[i][j].g + cellCost(grid, i, j - 1);
                hNew = gridCalculation(i, j - 1, dest);
                fNew = gNew + hNew;
	
				if ((cellDetails[i][j - 1].f == FLT_MAX) || (cellDetails[i][j - 1].f > fNew)){
						auxCount++;
                    	openList[auxCount].fcost = fNew;
						openList[auxCount].pos.x = i;
						openList[auxCount].pos.y = j + 1;
						auxList++;
 
                    	// Update the details of this cell
                    	cellDetails[i][j - 1].f = fNew;
                    	cellDetails[i][j - 1].g = gNew;
                    	cellDetails[i][j - 1].h = hNew;
                    	cellDetails[i][j - 1].parent_i = i;
                    	cellDetails[i][j - 1].parent_j = j;
                }
            }
		}
		auxList--;
	}
	
	if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

	return;
}

int main(){
	/* Description of the Grid-
     1--> The cell is not blocked
     0--> The cell is blocked    */
    int grid[ROW][COL]
        = { { 'M', 'M', 'R', '.', 'M', '.', 'A', 'A', 'A', 'A' },
            { 'M', 'V', 'V', 'V', 'M', '.', 'R', 'R', 'A', 'A' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' },
            { 'A', 'A', 'A', '.', '.', '.', '.', '.', '.', 'R' } };
 
    // Source is the left-most bottom-most corner
    Coords start;
	start.x = 8;
	start.y = 0;
 
    // Destination is the left-most top-most corner
    Coords dest;
	dest.x = 0;
	dest.y = 0;
 
    aSearch(grid, src, dest);
 
    return 0;
}