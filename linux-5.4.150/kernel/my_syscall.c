#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

asmlinkage long sys_mycall(void) {
	printk("System Call Example! *Lee Chang Yeop*\n");
	return 0;
}
