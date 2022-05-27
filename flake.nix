{
  description = "T1 de INF1771 (busca local e A*)";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils }: flake-utils.lib.eachDefaultSystem (system:
    let pkgs = import nixpkgs { inherit system; };
        buscalocal = with pkgs; stdenv.mkDerivation rec {
          name = "buscalocal";
          src = ./.;
          buildInputs = [ ];
          nativeBuildInputs = [ pkg-config SDL2 autoreconfHook ];
        };
    in {

      defaultPackage = buscalocal;

      devShell = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          autoconf
          automake
          bash
          binutils
          gcc
          gnumake
          gdb
          pkg-config
          SDL2
          valgrind
        ];
      };
    }
  );

}

