



// FILEIRAS E COLUNAS DO MAPA

#define ROW 82
#define COL 300

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