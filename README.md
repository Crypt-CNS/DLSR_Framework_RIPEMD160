# DLSR_Framework_RIPEMD160
The code is written in C++. Note that we provide several options for the size of solutions stored in RAM. Please choose it accordingly. For Linux system, please choose the size 0x10000000 = 2^28.

Please compile the program in Linux with the following command: "g++ -std=c++11  -ffast-math -march=native -pthread -O3 -fno-bounds-check -fprefetch-loop-arrays -DRELEASE RIPEMD160.h RIPEMD160.cpp main.cpp -o ./out"
