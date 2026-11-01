// SPDX-License-Identifier: Dual BSD/GPL
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 * (License header left unchanged)
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_AUTHOR("Bohdana Zhuravel <zhuravel.bohdana_io42@edu.kpi.ua>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct hello_time_node {
	struct list_head list_node;
	ktime_t time;
};

static LIST_HEAD(hello_time_list);

static int __init hello_init(void)
{
	uint i;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10))
		pr_warn("Warning: print_count is %u (0 or 5-10)\n", print_count);
	else if (print_count > 10) {
		pr_err("Error: print_count %u > 10. Module won't load.\n",
		       print_count);
		return -EINVAL;
	}

	for (i = 0; i < print_count; i++) {
		struct hello_time_node *node;

		node = kmalloc(sizeof(*node), GFP_KERNEL);
		if (!node) {
			struct hello_time_node *cursor, *tmp;

			pr_err("Memory allocation failed\n");

			list_for_each_entry_safe(cursor, tmp, &hello_time_list,
						 list_node) {
				list_del(&cursor->list_node);
				kfree(cursor);
			}
			return -ENOMEM;
		}

		node->time = ktime_get();
		list_add_tail(&node->list_node, &hello_time_list);

		pr_emerg("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_time_node *cursor, *tmp;

	list_for_each_entry_safe(cursor, tmp, &hello_time_list, list_node) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(cursor->time));
		list_del(&cursor->list_node);
		kfree(cursor);
	}

	pr_info("Module unloaded.\n");
}

module_init(hello_init);
module_exit(hello_exit);
