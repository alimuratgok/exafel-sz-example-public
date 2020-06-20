CC = gcc
C++ = /usr/bin/g++ -std=c++11 -g
#CC = /usr/bin/g++

#CFLAGS = -lm
CFLAGS = -O3 -ggdb -Wall -lm
#CFLAGS = -O3 -ggdb -lm

all: exafelSZ_example_sta exafelSZ_example_dyn fixPythonBinPeakFile extractFirstPanel

#Static Linking:
exafelSZ_example_sta: ./exafelSZ_example.c 
	$(CC) -g ./exafelSZ_example.c -o exafelSZ_example_sta -I${SZ_HOME}/install/include ${SZ_HOME}/install/lib/libSZ.a ${SZ_HOME}/install/lib/libzstd.a -lz -lm

#Dynamic Linking:
exafelSZ_example_dyn: ./exafelSZ_example.c
	gcc -g ./exafelSZ_example.c -o exafelSZ_example_dyn -I${SZ_HOME}/install/include -L${SZ_HOME}/install/lib -lSZ -lz -lzstd -lm

#Other Tools:
fixPythonBinPeakFile: fixPythonBinPeakFile.c
	${C++} ${CFLAGS} -o $@ $@.c

extractFirstPanel: extractFirstPanel.c
	${C++} ${CFLAGS} -o $@ $@.c

clean:
	rm -f exafelSZ_example_sta exafelSZ_example_dyn fixPythonBinPeakFile extractFirstPanel
	rm -f comp.bin decomp.bin decomp.cxi


