{
  description = "A simple C++ hello world";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        
        sableUI = pkgs.fetchFromGitHub {
          owner = "oliwilliams1";
          repo = "SableUI";
          rev = "master";
          hash = "sha256-/bBuDxAenF5dH1sKIBmoONy1Esrj7ZqnZShQgzg/3VY=";
          fetchSubmodules = true;
        };
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "Bar";
          version = "0.1.0";
          
          src = ./.;
          
          nativeBuildInputs = [ 
            pkgs.cmake 
            pkgs.pkg-config
            pkgs.wayland-scanner  # Build-time tool for generating Wayland protocol code
          ];
          
          buildInputs = with pkgs; [
            libGL
            libGLU
            glm
            # X11 libraries
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            # Wayland libraries
            wayland
            wayland-protocols
            libxkbcommon
          ];
          
          preConfigure = ''
            mkdir -p vendor
            cp -r ${sableUI} vendor/SableUI
            chmod -R u+w vendor/SableUI
          '';
          
          meta = with pkgs.lib; {
            description = "A simple C++ hello world";
            license = licenses.mit;
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            cmake
            clang-tools
            pkg-config
            wayland-scanner  # Add here too
            libGL
            libGLU
            glm
            # X11 libraries
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            # Wayland libraries
            wayland
            wayland-protocols
            libxkbcommon
          ];
        };
      }
    );
}