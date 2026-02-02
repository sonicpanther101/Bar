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
          hash = "";
          fetchSubmodules = true;
        };
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "Bar";
          version = "0.1.0";
          
          src = ./.;
          
          nativeBuildInputs = with pkgs; [ 
            cmake 
            pkg-config
            wayland-scanner  # Build-time tool for generating Wayland protocol code
            libffi
          ];
          
          buildInputs = with pkgs; [
            libGL
            libglvnd
            glew
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
            wayland-utils  # debugging
            wayland-protocols
            libxkbcommon
            mesa                                # EGL + OpenGL ES
            egl-wayland                         # wl_egl_window_create()
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
            wayland-scanner
            libffi
            libGL
            libglvnd
            glew
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
            wayland-utils  # debugging
            wayland-protocols
            libxkbcommon
            mesa                                # EGL + OpenGL ES
            egl-wayland                         # wl_egl_window_create()
          ];

          # Try commenting this out to see if native Wayland works now
          # export GLFW_PLATFORM=x11 

          shellHook = ''
            export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [ 
              pkgs.wayland 
              pkgs.libxkbcommon 
              pkgs.libGL 
              pkgs.libglvnd
              pkgs.xorg.libX11 
              pkgs.xorg.libXcursor
              pkgs.xorg.libXi
              pkgs.xorg.libXinerama
              pkgs.xorg.libXrandr
            ]}:/run/opengl-driver/lib:/run/opengl-driver-32/lib"
            
            export XDG_RUNTIME_DIR=/run/user/$(id -u)
            export WAYLAND_DISPLAY=''${WAYLAND_DISPLAY:-wayland-1}
            export DISPLAY=''${DISPLAY:-:0}
          '';
        };
      }
    );
}