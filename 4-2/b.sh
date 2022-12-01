for (( i = 0; i < 50; i++)); do

	sync
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	gcc -o test2 D_recompile_test.c
	./test2
	make
	./drecompile >> result.csv
	echo -n ", " >> result.csv

	sync
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	./test2
	make dynamic
	./drecompile >> result.csv
	echo >> result.csv

done
