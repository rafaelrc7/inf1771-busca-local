# inf1771-busca-local
T1 de INF1771 2022.1, busca local e A*.

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

#### Dependencies
- GNU Autotools >= 2.71
- GCC
- SDL2
- POSIX compliant system

#### Instructions
```sh
git clone https://github.com/rafaelrc7/inf1771-busca-local
cd inf1771-busca-local
aclocal
automake --add-missing
autoconf
./configure && make
```


#### Melhor resultado registrado
	1000000 0100000 0100000 0010000 0010000 0000100 0000001 0000001 1010000 1010000 1000010 1001000 1001000 0100100 0100100 0101000 0100001 0100001 0100001 0010010 0010010 0001010 0001010 0001010 0001010 0001100 0000011 0000101 0000101 1010100 1010100 	1805.548602

## License
This software is licensed under the [MIT License](/COPYING).

