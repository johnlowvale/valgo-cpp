export CPATH="/usr/local/include/libbson-1.0:/usr/local/include/libmongoc-1.0"
gcc -o mongoc mongoc.c -lbson-1.0 -lmongoc-1.0

export LD_LIBRARY_PATH="/usr/local/lib"
./mongoc

