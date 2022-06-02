# inf1771-busca-local
T1 de INF1771 2022.1, busca local e A*.

## Running

### Genetic only
```sh
./buscalocal g
```

### A* GUI
```sh
./buscalocal a < mapfile
```

### A* headless
```sh
./buscalocal m < mapfile
```

### Genetic and A*
```sh
./buscalocal h < mapfile
```

## Settings
The program may be configured easily by creating a optional ```config.lua```
file on the same directory where it is run. The lua file must return a table,
where the config options are set.

### Supported config options:
- app_name: (string) name of the program window
- waypoints: (table) list of single character strings, should be on map
- agilities: (table) list of the characters' agilities, must have number of
  elements equal to character number.
- map_width: (integer) map width
- map_height: (integer) map height
- stage_num: (integer) number of stages
- char_num: (integer) number of characters, due to technical limitations, must
  be <= 8
- char_lives: (integer) number of character lives
- generation_num: (integer) number of generations
- population_cap: (integer) maximum number of individuals
- populations_step: (integer) starting number of individuals and number of
  generated each generation
- win_width: (integer) window width, ignored if scale is set
- win_width_scale: (float) window width scale based on map width
- win_height: (integer) window height, ignored if scale is set
- win_height_scale: (float) window height scale based on map height
- seed: (integer) random seed

### Example:
```lua
return {
	app_name = "INF1771 T1",
	waypoints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'E', 'G', 'H', 'I', 'J', 'K', 'L', 'N', 'O', 'P', 'Q', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'},
	aigilities = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7},
	difficulties = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170 , 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310},
	map_width = 300,
	map_height = 82,
	stage_num = 31,
	char_num = 7,
	char_lives = 8,
	generation_num = 1500,
	population_cap = 1000,
	populations_step = 300,
	win_width_scale = 5,
	win_height_scale = 5,
	mut_base = 60,
	seed = 10,
}
```

## Building

### Nix

On both of the options below, the build will be located at ```./result/bin/buscalocal```

#### Remote Repo
```sh
nix build github:rafaelrc7/inf1771-busca-local
```

#### Local Repo
```sh
git clone https://github.com/rafaelrc7/inf1771-busca-local
cd inf1771-busca-local
nix build
```

### GNU Autotools
The binary file will be located at ```build/src/buscalocal```

#### Dependencies
- GCC
- Make
- SDL2
- SDL2 TTF
- POSIX compliant system

#### Instructions
```sh
git clone https://github.com/rafaelrc7/inf1771-busca-local
cd inf1771-busca-local
mkdir build && cd build
../configure && make
```

#### Instructions (Maintainer)
##### Extra Dependency
- GNU Autotools >= 2.71

```sh
git clone https://github.com/rafaelrc7/inf1771-busca-local
cd inf1771-busca-local
./autogen.sh
mkdir build && cd build
../configure --enable-maintainer-mode && make
```


## Melhor resultado registrado
```
1000000 0100000 0100000 0010000 0010000 0001000 0000010 0000001 0000001 1010000 1010000 1000010 1000010 0100010 0101000 0100100 0100001 0100001 0100001 0010100 0011000 0000110 0001100 0001100 0000110 0000110 0000011 0001001 0000101 1011000 1011000 	1822.1074358063
```
SEED = 1629049572


## License
This software is licensed under the [MIT License](/COPYING).

