#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

int counter;
spinlock_t counter_lock;
struct task_struct *writer_thread1, *writer_thread2;



struct int writer_function(void *data)
{
	while(!kthread_should_stop()){
		spin_lock(
	}
}
