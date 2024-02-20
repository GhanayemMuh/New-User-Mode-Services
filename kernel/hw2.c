#include <linux/kernel.h>
asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_weight(int weight) {
	if (weight < 0)
		return -EINVAL;
	current->pweight = weight;
	return 0;
}

asmlinkage long sys_get_weight(void) {
	return current->pweight;
}


long helper_get_ancestor_weight_sum(struct task_struct* child) {
	if (child->real_parent == NULL) {
		//reached (init process)
		return 0;
	}
	else {
		long weight = child->real_parent->pweight;
		weight += helper_get_ancestor_weight_sum(child->real_parent);
		return weight;
	}
} 
asmlinkage long sys_get_ancestor_sum(void) {
	helper_get_ancestor_weight_sum(current);
}

asm long sys_get_heaviest_descendant(void) {
    struct task_struct *heaviest_descendant = NULL;
    int max_weight = 0;

    if (list_empty(&(current->children)))
        return -ECHILD;

    struct list_head *child_list;
    struct task_struct *child;

    list_for_each(child_list, &(current->children)) {
        child = list_entry(child_list, struct task_struct, sibling);

        if (child->pweight > max_weight) {
            max_weight = child->pweight;
            heaviest_descendant = child;
        }
	else if (child->pweight == max_weight && child->pid < heaviest_descendant->pid) {
            heaviest_descendant = child;
        }
    }

    if (heaviest_descendant == NULL)
        return -ECHILD;

    return heaviest_descendant->pid;
}
