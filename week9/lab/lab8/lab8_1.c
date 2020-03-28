#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/mm.h>
#include <asm/current.h>
#include <linux/mm_types.h>
#include <linux/slab.h>

#define TAG "Lab8_1"
#define MAX_NUM 100
#define DELAY 100

static struct task_struct *t1;
static struct task_struct *t2;
static struct task_struct *t3;
static struct task_struct *t4;


DEFINE_SPINLOCK(lock);
DEFINE_SPINLOCK(complock);

static int counter = 0;
static int tmp;
// array of size MAX 
static int a[MAX_NUM]={0};
static int part = 0; 


int gen_int(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

static int threadMethod(void *unused) {
    	int i = 0;
	int local_counter; //read shared data
	int n; 
    	spin_lock(&lock);
    	local_counter = counter; //read shared data
/*	
    if (strcmp(current->comm, "mythread1") == 0) {
 	msleep(2);
    } else {
        msleep(1);
    }
*/
	
    
    for (i=0;i<5;i++) {
	n = gen_int();
	    if (printk_ratelimit()) {
	     //  printk(KERN_INFO "random number is %d\n", n);
		printk(KERN_INFO "I'm %s, random number is: %d, in counter: %d", current->comm, n,local_counter);
		a[local_counter] = n;
	    }
       udelay(DELAY);
       local_counter++; //update 
	
	
    }
    
    counter = local_counter; //store back
    spin_unlock(&lock);

    return 0;
}


void myswap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


// Function to print an array 
void printArray(void) 
{ 
    int i; 
    for (i = 0; i < MAX_NUM; i += 1) 
    {
printk(KERN_INFO "Sorted by %s,  in counter: %d, random number is: %d,", current->comm, i, a[i]);
/*	if (printk_ratelimit()) {
		printk(KERN_INFO "I'm %s,  in counter: %d, random number is: %d,", current->comm, i, a[i]);
	}
*/
    }
} 



  
// merge function for merging two parts 
void merge(int low, int mid, int high) 
{ 
    int n1, n2;
    int k, i, j;
    int* left = kmalloc(sizeof(int)*(mid - low + 1), GFP_KERNEL);
    int* right = kmalloc( (sizeof(int)*(high - mid)) , GFP_KERNEL);
  
    // n1 is size of left part and n2 is size 
    // of right part 
    n1 = mid - low + 1; 
    n2 = high - mid ; 
  
    // storing values in left part 
    for (i = 0; i < n1; i++) 
        left[i] = a[i + low]; 
  
    // storing values in right part 
    for (i = 0; i < n2; i++) 
        right[i] = a[i + mid + 1]; 
  
    k = low; 
    i = j = 0; 
  
    // merge left and right in ascending order 
    while (i < n1 && j < n2) { 
        if (left[i] <= right[j]) 
            a[k++] = left[i++]; 
        else
            a[k++] = right[j++]; 
    } 
  
    // insert remaining values from left 
    while (i < n1) { 
        a[k++] = left[i++]; 
    } 
  
    // insert remaining values from right 
    while (j < n2) { 
        a[k++] = right[j++]; 
    } 
} 
  
// merge sort function 
void merge_sort_func(int low, int high) 
{ 
    // calculating mid point of array 
    int mid = low + (high - low) / 2; 
    if (low < high) { 
  
        // calling first half 
        merge_sort_func(low, mid); 
  
        // calling second half 
        merge_sort_func(mid + 1, high); 
  
        // merging the two halves 
        merge(low, mid, high); 
    } 
} 
  
// thread function for multi-threading 
void* merge_sort(void* arg) 
{ 
int thread_part, low, high, mid;
    // which part out of 4 parts 
    thread_part = part++; 
  
    // calculating low and high 
    low = thread_part * (MAX_NUM / 4); 
    high = (thread_part + 1) * (MAX_NUM / 4) - 1; 
  
    // evaluating mid point 
    mid = low + (high - low) / 2; 
    if (low < high) { 
        merge_sort_func(low, mid); 
        merge_sort_func(mid + 1, high); 
        merge(low, mid, high); 
    } 
} 
  


static int threadSort(void *arg) {
   //lock
   //do sort between from and to inside array
	 
   //unlock

	
	int thread_part;
    int low, high, mid;
    int i;
    spin_lock(&lock);
	

    // struct tasks_ids *data = (struct tasks_ids*)arg;
    // doing something
/*
    if (strcmp(current->comm, "mythread1") == 0) {
 	msleep(2);
    } else {
        msleep(1);
    }
*/
// which part out of 4 parts 
    thread_part = part; 
    part = thread_part+1; //store back
    
    // calculating low and high 
    low = thread_part * (MAX_NUM / 4); 
    high = (thread_part + 1) * (MAX_NUM / 4) - 1; 
  
    // evaluating mid point 
    mid = low + (high - low) / 2; 
    if (low < high) { 
        merge_sort_func(low, mid); 
        merge_sort_func(mid + 1, high); 
       /* if (data->id == 3)
        complete(data->comp); */
        merge(low, mid, high); 
    } 
	printk(KERN_INFO "Parts of array: \n low %d \n high %d \n part %d \n",low,high, thread_part);
	for (i=low; i <= high; i++)
    {
        printk(KERN_INFO "Sorting in %s,  in counter: %d, random number is: %d,", current->comm, i, a[i]);
    }
    
    spin_unlock(&lock);
    

    return 0;
	
}




void createThreads(void) {

   
// creating 4 threads 
       spin_lock(&complock);
   	t1 = kthread_run(threadSort, NULL, "mythread1");
    t2 = kthread_run(threadSort, NULL, "mythread2");
    t3 = kthread_run(threadSort, NULL, "mythread3");
       /* data[3]->comp = &comp;
        data[3]->id = 3;*/
	t4 = kthread_run(threadSort, NULL, "mythread4");
  
    spin_lock(&lock);
    // merging the final 4 parts 
    merge(0, (MAX_NUM / 2 - 1) / 2, MAX_NUM / 2 - 1); 
    merge(MAX_NUM / 2, MAX_NUM/2 + (MAX_NUM-1-MAX_NUM/2)/2, MAX_NUM - 1); 
    merge(0, (MAX_NUM - 1)/2, MAX_NUM - 1); 
   
    spin_unlock(&lock);
    spin_unlock(&complock);
	
}

int init_module(void)
{
	int n;
	int i;
        printk(KERN_INFO "START %s\n", TAG);

	for (i=0; i<MAX_NUM; i++)
	{
		n = gen_int();
		a[i] = n;
		if (printk_ratelimit()) {
	     //  printk(KERN_INFO "random number is %d\n", n);
		printk(KERN_INFO "I'm %s, random number is: %d, in counter: %d", current->comm, a[i],i);
	    }

	}

	
	
        createThreads();
udelay(DELAY);
// displaying sorted array 
    spin_lock(&complock);
    
   // msleep(2);
	printArray();
    spin_unlock(&complock);
        return 0;
}

void cleanup_module(void)
{
        printk(KERN_DEBUG "END %s\n", TAG);
}
