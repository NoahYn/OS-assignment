sync
echo 3 | sudo tee /proc/sys/vm/drop_caches
gcc -o test2 D_recompile_test.c
./test2
make
./drecompile > before.txt
./drecompile

sync
echo 3 | sudo tee /proc/sys/vm/drop_caches
./test2
make dynamic
./drecompile > after.txt
./drecompile
make clean
