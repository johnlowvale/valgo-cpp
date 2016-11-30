export CPATH="/usr/local/include:/usr/local/include/bsoncxx/v_noabi:/usr/local/include/mongocxx/v_noabi"
g++-6 -std=c++14 -o mongocxx mongocxx.cpp -lmongocxx -lbsoncxx
export LD_LIBRARY_PATH="/usr/lib:/usr/local/lib"
./mongocxx
