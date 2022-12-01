#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <time.h>

uint8_t* Operation;
uint8_t* compiled_code;

void sharedmem_init(); // 공유 메모리에 접근
void sharedmem_exit();
void drecompile_init(); // memory mapping 시작 
void drecompile_exit(); 
void* drecompile(uint8_t *func); //최적화하는 부분

int main(void)
{
	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	double time = 0;

	int (*func)(int a);
	int i;

	sharedmem_init();
	drecompile_init();

	func = (int (*)(int a))drecompile(Operation);
	for (int i = 0; i < 1000000; i++)
		func(1);
//	printf("m = %d\n", func(1));

	drecompile_exit();
	sharedmem_exit();
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	time += (end.tv_sec - begin.tv_sec);
	time += (double)(end.tv_nsec - begin.tv_nsec)/1000000000;
	printf("total execution time : %lf sec", time);
	return 0;
}

void sharedmem_init() {
	int seg_id;
	seg_id = shmget(1234, PAGE_SIZE, 0);
	Operation = (uint8_t*)shmat(seg_id, NULL, 0);
}

void sharedmem_exit() { shmdt(Operation); }

void drecompile_init() {
	compiled_code = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (compiled_code == NULL)
		exit(1);
	return;
}

void drecompile_exit() {
	msync(compiled_code, PAGE_SIZE, MS_ASYNC);
	munmap(compiled_code, PAGE_SIZE);
}

void* drecompile(uint8_t* func)
{
	int i = 0;
	int k = 0;
	int dl_val;
	int div_count = 0;

	do {
		compiled_code[i] = func[k];
#ifdef dynamic
		if (func[k] == 0xb2) {
			dl_val = func[k+1];	
		}
		if (func[k] == 0x83) { // add, sub
			if (func[k+1] == 0xc0) { // add		
				i++; k++;
				compiled_code[i++] = func[k++];
				compiled_code[i] = func[k];
				while (func[k+1] == 0x83 && func[k+2] == 0xc0) { // next also add {
					k+=3;
					compiled_code[i] += func[k];
				}
			}
			else if (func[k+1] == 0xe8) {// sub
				k++; i++;
				compiled_code[i++] = func[k++];
				compiled_code[i] = func[k];
				while (func[k+1] == 0x83 && func[k+2] == 0xe8) { // next also sub 
					k+=3;
					compiled_code[i] += func[k];
				}
			}
		}
		else if (func[k] == 0x6b) { // mul
			k++; i++;
			compiled_code[i++] = func[k++];
			compiled_code[i] = func[k];
			while (func[k+1] == 0x6b) { // next also mul 
				k+=3;
				compiled_code[i] *= func[k];
			}
		}
		else if (func[k] == 0xf6) { // div
			while (func[k+2] == 0xf6) { // next also div
				k+=2;
				div_count++;
			}
			if (div_count == 0) // there's only one div
				compiled_code[++i] = func[++k];
			else {
				compiled_code[i++] = 0xb2;
				compiled_code[i] = dl_val; // mov dl_val %dl
				while (div_count--) {
					compiled_code[i] *= dl_val;	  
				}
				compiled_code[++i] = func[k];
				compiled_code[++i] = func[++k];
			}
		}
#endif
		i++;
	} while(func[k++] != 0xC3);

//	printf("i = %d, k = %d\n", i, k);	

	mprotect(compiled_code, PAGE_SIZE, PROT_EXEC);

	return (int(*)(int))compiled_code;
}

