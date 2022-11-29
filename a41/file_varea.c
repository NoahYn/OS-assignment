#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/sched/mm.h>
#include <linux/syscalls.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/syscall_wrapper.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>

#define __NR_ftrace 336

void **syscall_table;
void *org_ftrace;

static asmlinkage int file_varea (const struct pt_regs *regs)
{
 	int pid = (int)regs->di;

	struct task_struct *t;
	struct mm_struct *mm;
	struct vm_area_struct *vm;
	char* file_path;
	char buf[100];

	t = pid_task(find_vpid(pid), PIDTYPE_PID);
	mm = get_task_mm(t);
	vm = mm->mmap;

	printk("######## Loaded files of a process '%s[%d]' in VM ########", t->comm, pid);
	while (vm->vm_next) {
		if (vm->vm_file) {
			file_path = d_path(&vm->vm_file->f_path, buf, 100);
			printk("mem[%ld~%ld] code[%ld~%ld] data[%ld~%ld] heap[%ld~%ld] %s", vm->vm_start, vm->vm_end, mm->start_code, mm->end_code, mm->start_data, mm->end_data, mm->start_brk, mm->brk, file_path); 
		}
		vm = vm->vm_next;
	}
	printk("##################################################################");

	mmput(mm);	
	return pid;
}

void make_rw(void *addr)
{
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
}

void make_ro(void *addr)
{
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);

	pte->pte = pte->pte &~ _PAGE_RW;
}

static int __init hooking_init(void)
{
	syscall_table = (void**) kallsyms_lookup_name("sys_call_table");
	make_rw(syscall_table);

	org_ftrace = syscall_table[__NR_ftrace];
	syscall_table[__NR_ftrace] = file_varea;

	return 0;
}

static void __exit hooking_exit(void)
{
	syscall_table[__NR_ftrace] = org_ftrace;
	make_ro(syscall_table);
}

module_init(hooking_init);
module_exit(hooking_exit);
MODULE_LICENSE("GPL");
