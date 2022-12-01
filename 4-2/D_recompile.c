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

//	do {
//		printf("%x ", Operation[i]);
//	} while(Operation[i++] != 0xC3);

	func = (int (*)(int a))drecompile(Operation);

	drecompile_exit();
	sharedmem_exit();
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	time += (end.tv_sec - begin.tv_sec);
	time += (double)(end.tv_nsec - begin.tv_nsec)/1000000000;
	printf("total execution time : %lf sec\n", time);
	return 0;
}

#ifndef dynamic
// inside here is dynamic code
#endif


void sharedmem_init()
{
	int seg_id;
	seg_id = shmget(1234, PAGE_SIZE, 0);
	Operation = (uint8_t*)shmat(seg_id, NULL, 0);

	return;
}

void sharedmem_exit()
{
//	shmdt(Operation);
	return;
}

void drecompile_init(uint8_t *func)
{
	int fd;
	int pagesize;

//	fd = open("test2", O_RDWR);
	pagesize = getpagesize();
//	compiled_code = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//	if (compiled_code == NULL)
//		exit(1);
	return;
}

void drecompile_exit()
{
	int pagesize = getpagesize();
	
//	msync(compiled_code, pagesize, MS_ASYNC);
//	munmap(compiled_code, pagesize);
}

void* drecompile(uint8_t* func)
{
//	mprotect(func, getpagesize(), PROT_EXEC);
	//((int(*)(int))func)(1);
	

//	func(1);
//	compiled_code[0] &= ~0x20;

}

