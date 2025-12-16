# Clib
C libs with many usefull functions

## first compilation
```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release	#Debug/RelWithDebInfo/MinSizeRel
```

## compiling
```bash
cmake --build build
mv build/libclib.a ./clib.a
```

## to clean it up
```bash
cmake --build build --target clean
rm -rf build
```