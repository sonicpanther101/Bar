{
  description = "A simple C++ hello world";

  # Use this command to enter the environment
  # nix develop -c $SHELL
  # or nix-develop

  # Use this command to build linked cmake file
  # nix build

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "Bar";
          version = "0.1.0";
          
          src = ./.;
          
          nativeBuildInputs = [ pkgs.cmake ];
          
          meta = with pkgs.lib; {
            description = "A simple C++ hello world";
            license = licenses.mit;
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            cmake
            clang-tools  # for clangd LSP
          ];
        };
      }
    );
}