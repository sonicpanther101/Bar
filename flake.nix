{
  description = "Bar - Wayland bar using SableUI";

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
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Build tools
            gcc
            cmake
            clang-tools
            pkg-config
            
            # Wayland build-time tools
            wayland-scanner
            libffi
            
            # Graphics libraries
            libGL
            libglvnd
            glew
            libGLU
            glm
            mesa
            egl-wayland
            glfw
            
            # X11 libraries
            libX11
            libXrandr
            libXinerama
            libXcursor
            libXi
            
            # Wayland libraries
            wayland
            wayland-utils
            wayland-protocols
            libxkbcommon
          ];

          shellHook = ''
            export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [ 
              pkgs.wayland 
              pkgs.libxkbcommon 
              pkgs.libGL 
              pkgs.libglvnd
              pkgs.libX11 
              pkgs.libXcursor
              pkgs.libXi
              pkgs.libXinerama
              pkgs.libXrandr
            ]}:/run/opengl-driver/lib:/run/opengl-driver-32/lib"
            
            export XDG_RUNTIME_DIR=/run/user/$(id -u)
            export WAYLAND_DISPLAY=''${WAYLAND_DISPLAY:-wayland-1}
            export DISPLAY=''${DISPLAY:-:0}
            
            echo "Bar development environment loaded"
            echo "Build with: mkdir -p build && cd build && cmake .. && make -j\$(nproc)"
          '';
        };
      }
    );
}