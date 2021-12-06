#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define FALSE 0
#define TRUE 1

struct my_type {
	struct rb_node node;
	int key;
	int value;
};


unsigned long long calclock3(unsigned long long interval, unsigned long long *total_time, unsigned long long *total_count){
	//long temp, temp_n = 0;
	unsigned long long timedelay = interval;

	__sync_fetch_and_add(total_time, timedelay);
	__sync_fetch_and_add(total_count, 1);
	return timedelay;
}

int rb_insert(struct rb_root *root, struct my_type *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	while(*new) {
		struct my_type *this = container_of(*new, struct my_type, node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if(this->key < data->key)
			new = &((*new)->rb_right);
		else
			return FALSE;
	}
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);
	return TRUE;
}

struct my_type *rb_search(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;
	while(node){
		struct my_type *data = container_of(node, struct my_type, node);
		if(data->key > key) node = node->rb_left;
		else if(data->key < key) node = node->rb_right;
		else return data;
	}
	return NULL;
}

void rb_delete(struct rb_root *mytree, int key)
{
	struct my_type *data = rb_search(mytree, key);
	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
	}
}

void struct_example(void)
{
	struct rb_root my_tree = RB_ROOT;
	int i, ret;
	for(i=1; i<101; i*=10){
		int k = 1000 * i;
		int value;
		unsigned long long count = 0;
		struct __kernel_timex spclock[2];
		unsigned long long list_time;
		unsigned long long list_count;
		unsigned long long starttime;
		unsigned long long endtime;
		//do_clock_adjtime(CLOCK_REALTIME, &spclock[0]);
		starttime = ktime_get_ns();
		for(value = 0; value < k; value++){
			struct my_type *new = kmalloc(sizeof(struct my_type), GFP_KERNEL);
			if(!new) return;
			new->value = i;
			new->key = i;
			ret = rb_insert(&my_tree, new);
			count++;
		}
		//do_clock_adjtime(CLOCK_REALTIME, &spclock[1]);
		//calclock3(endtime-starttime, &list_time, &list_count);
		endtime = ktime_get_ns();
		printk("insert %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);

		//do_clock_adjtime(CLOCK_REALTIME, &spclock[0]);
		count = 0;
		starttime = ktime_get_ns();
		for(value=0; value<k; value++){
			struct my_type *find_node = rb_search(&my_tree, value);
			count++;
		}
		//do_clock_adjtime(CLOCK_REALTIME, &spclock[1]);
		endtime = ktime_get_ns();
		//calclock3(endtime-starttime, &list_time, &list_count);
		printk("search %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);

		//do_clock_adjtime(CLOCK_REALTIME, &spclock[0]);
		count = 0;
		starttime = ktime_get_ns();
		for(value = 0; value < k; value++){
			rb_delete(&my_tree, value);
			count++;
		}
		//do_clock_adjtime(CLOCK_REALTIME, &spclock[1]);
		endtime = ktime_get_ns();
		//calclock3(endtime-starttime, &list_time, &list_count);
		printk("delete %d times, time: %llu, count: %llu \n", k, endtime-starttime, count);
	}
}

int __init my_module_init(void)
{
	
	printk("start my module! \n");
	struct_example();
	return 0;
}

void __exit my_module_cleanup(void)
{
	printk("Bye my module!\n");
}

module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_LICENSE("GPL");
