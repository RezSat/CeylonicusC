# Clean the broken build

`Remove-Item -Recurse -Force build`

# Use GCC instead of Clang

`cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_C_COMPILER=gcc`

`cmake --build build`
- Later add LLVM libs to run with clang so it doesn't default to Nmake