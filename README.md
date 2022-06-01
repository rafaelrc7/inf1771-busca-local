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

