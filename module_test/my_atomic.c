#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mutex.h>

#define NUMBER_OF_NODE 100000

int global, searchCount, deleteCount;

struct task_struct *thread1, *thread2, *thread3, *thread4;

struct my_node{
	struct list_head list;
	int data;
	spinlock_t lock;
};

spinlock_t list_lock;
struct rw_semaphore counter_rwse;
struct mutex my_mutex;
struct my_node node;

unsigned long long calclock3(unsigned long long interval, unsigned long long *total_time, unsigned long long *total_count){
	//long temp, temp_n = 0;
	unsigned long long timedelay = interval;

	__sync_fetch_and_add(total_time, timedelay);
	__sync_fetch_and_add(total_count, 1);
	return timedelay;
}

int writer_function(void *data)
{
	struct list_head my_list;
        INIT_LIST_HEAD(&my_list);
        unsigned long long count = 0;
        struct __kernel_timex spclock[2];
        unsigned long long list_time;
        unsigned long long list_count;
        unsigned long long starttime;
        unsigned long long endtime;
	starttime = ktime_get_ns();
	while(!kthread_should_stop()){
		mutex_lock(&my_mutex);
		for(;global<NUMBER_OF_NODE;global++){
			struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
			new->data = global;
			list_add(&new->list, &my_list);
		}
		if(global == NUMBER_OF_NODE){
			endtime = ktime_get_ns();
			printk("Mutex linked list insert time : %llu", endtime-starttime);
			global++;
		}
		
		struct my_node *current_node;
		struct list_head *p;
		list_for_each(p, &my_list){
			current_node = list_entry(p, struct my_node, list);
		}
		starttime = ktime_get_ns();
		list_for_each_entry(current_node, &my_list, list){
			int cur = current_node->data;
			searchCount++;
		}
		if(searchCount == NUMBER_OF_NODE){
			endtime = ktime_get_ns();
			printk("Mutex linked list search time : %llu", endtime-starttime);
			searchCount++;
		}
		
		struct my_node *tmp;
		starttime = ktime_get_ns();
		list_for_each_entry_safe(current_node, tmp, &my_list, list){
			list_del(&current_node->list);
			kfree(current_node);
			deleteCount = NUMBER_OF_NODE;
		}
		endtime = ktime_get_ns();
		printk("Mutex linked list delete time : %llu", endtime-starttime);
		deleteCount++;


		mutex_unlock(&my_mutex);
		msleep(500);
	}
	do_exit(0);
}



int __init hello_module_init(void)
{
	printk(KERN_EMERG "Hello Module! \n");
	global = 0;
	searchCount = 0;
	deleteCount = 0;
	thread1 = kthread_run(writer_function, NULL, "thread 1");
	thread2 = kthread_run(writer_function, NULL, "thread 2");
	thread3 = kthread_run(writer_function, NULL, "thread 3");
	thread4 = kthread_run(writer_function, NULL, "thread 4");

	return 0;
}

void __exit hello_module_cleanup(void)
{
	kthread_stop(thread1);
	kthread_stop(thread2);
	kthread_stop(thread3);
	kthread_stop(thread4);
	printk("Bye Module! \n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
