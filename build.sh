cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=~/project/vcpkg/scripts/buildsystems/vcpkg.cmake -DMESHLIB_INSTALL_DIR=C:/lib -DCMAKE_PREFIX_PATH=C:/lib/CMake
cmake --build .