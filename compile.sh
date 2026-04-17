mkdir -p build
cmake -B build -G "Unix Makefiles"
cmake --build build

echo Compiling Shaders....
glslc shaders/vert.vert -o shaders/vert.spv
glslc shaders/frag.frag -o shaders/frag.spv
echo Shaders Compiled.
