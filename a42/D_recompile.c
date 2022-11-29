#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/mman.h>

uint8_t* Operation;
uint8_t* compiled_code;

void sharedmem_init(); // 공유 메모리에 접근
void sharedmem_exit();
void drecompile_init(); // memory mapping 시작 
void drecompile_exit(); 
void* drecompile(uint8_t *func); //최적화하는 부분

int main(void)
{
	int (*func)(int a);
	int i;

	sharedmem_init();
	drecompile_init();

	func = (int (*)(int a))drecompile(Operation);

	drecompile_exit();
	sharedmem_exit();
	return 0;
}

void sharedmem_init()
{
}

void sharedmem_exit()
{
}

void drecompile_init(uint8_t *func)
{
}

void drecompile_exit()
{
}

void* drecompile(uint8_t* func)
{
}

