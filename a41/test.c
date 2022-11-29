#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

#define __NR_ftrace 336

int main(){
    syscall(__NR_ftrace, getpid());
    return 0;
}
