## Usage

```bash
# Enter the nix shell (loads all dependencies)
nix develop

# Build with cmake & make
git submodule update --init --recursive
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run your bar
./Bar
```