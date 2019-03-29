# DLSR_Framework_RIPEMD160
The code is written in C++. Note that we provide several options for the size of solutions stored in RAM. Please choose it accordingly. For Linux system, please choose the size 0x10000000 = 2^28.

Please compile the program in Linux with the following command: "g++ -std=c++11  -ffast-math -march=native -pthread -O3 -fno-bounds-check -fprefetch-loop-arrays -DRELEASE RIPEMD160.h RIPEMD160.cpp main.cpp -o ./out"

You will be required to input the left bound (l) and right bound (r). For the solution to (X_9,X_10), only those will be stored in RAM which satisfy l * solutionSize <= X_9 \xor X_10 < r * solutionSize. 

For example, if l=0, r=1, and you choose solutionSize = 0x10000000, then only those (X_9, X_10) satisfying 0<= X_9 \xor X_10 < 0x10000000 will be stored in RAM.
