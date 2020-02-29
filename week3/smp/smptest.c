#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/types.h>
// #include <linux/smp.h>
/*
typedef struct {
    volatile int counter;
    } atomic_t;
    */
MODULE_LICENSE("GPL");

void print_atomic_counter(void) {

    
	printk(KERN_INFO "Begin");
    int cpu;
    /* disable kernel preemption and set “cpu” to the current processor */
    cpu = get_cpu();

    printk(KERN_INFO "Challendge: total cpus are %d", cpu);
	atomic_t v; /* define v */
    atomic_t u = ATOMIC_INIT(0); /* define u and initialize it to zero */

    atomic_set(&v, 4);
    /* v = 4 (atomically) */
    atomic_add(2, &v);
    /* v = v + 2 = 6 (atomically) */
    atomic_inc(&v);
    /* v = v + 1 = 7 (atomically) */
    printk("%d\n", atomic_read(&v));
    /* will print “7” */

    
	printk(KERN_INFO "END");
}

int init_module(void)
{

        printk(KERN_INFO "Lect Atomic: start.\n");
	    print_atomic_counter();

        return 0;
}

void cleanup_module(void)
{
        printk(KERN_INFO "Lect Atomic: end.\n");
}
