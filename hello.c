// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2026 Student
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Lab 4: Kernel Module");
MODULE_VERSION("0.1");

static uint count = 1;
module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "Number of times to print 'Hello, world!'");

struct my_event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	int i;

	if (count > 10) {
		pr_err("Error: count (%u) is greater than 10\n", count);
		return -EINVAL;
	}

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("Warning: count is %u\n", count);

	for (i = 0; i < count; i++) {
		struct my_event *element;

		element = kmalloc(sizeof(*element), GFP_KERNEL);
		if (!element) {
			struct my_event *md, *tmp;

			pr_err("Memory allocation failed\n");
			list_for_each_entry_safe(md, tmp,
						 &event_list_head, list) {
				list_del(&md->list);
				kfree(md);
			}
			return -ENOMEM;
		}

		element->time = ktime_get();
		list_add_tail(&element->list, &event_list_head);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct my_event *md, *tmp;

	list_for_each_entry_safe(md, tmp, &event_list_head, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(md->time));
		list_del(&md->list);
		kfree(md);
	}

	pr_info("Module unloaded successfully\n");
}

module_init(hello_init);
module_exit(hello_exit);
