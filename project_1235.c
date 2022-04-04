#define pr_fmt(fmt) "[csse332]: " fmt



#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/kthread.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Collin, Sal");
MODULE_DESCRIPTION("Basic Hello World Module");

//struct list_head head;
static LIST_HEAD(head);

#define HISTORY_LEN 5
#define MAX_QUEUES 3

struct task_struct *scheduler_thread;
struct csse_332_info *current_scheduled_process;

static struct list_head head1;
static struct list_head head2;
static struct list_head head3;

int which_queue_to_enter;

struct timer_list timer;
struct timer_list reset_timer;
/* struct workqueue_struct * work_queue; */
/* struct work_struct * work; */
static DEFINE_MUTEX(lock);

struct csse_332_info {
	int value;
	struct list_head llist;
	int pid;
	char * name;
	int hist_cpu[HISTORY_LEN];
	int hist_proc_time[HISTORY_LEN];
	long hist_state[HISTORY_LEN];
	struct task_struct *proc_task_struct;
  int quantum;
};

struct run_queue {
	int priority;
	int quantum;
	struct list_head queue_head;
};

struct run_queue priority_queues[MAX_QUEUES];
/* struct list_head heads[MAX_QUEUES]; */

static int thr = -1;
module_param(thr, int, 0644);
MODULE_PARM_DESC(thr, "The threshhold to kill processes");

static inline void
set_task_state(struct task_struct *tsk, long state)
{
	smp_store_mb(tsk->state, state);
}


int
scheduler_fn(void *arg)
{
	unsigned int time;
	struct csse_332_info *element;
	pr_info("Kernel thread is born! Happy birthday to me...\n");
	time = 0;
	/* set_current_state(TASK_INTERRUPTIBLE); */
	while(!kthread_should_stop()) {
		del_timer(&timer);
		mutex_lock(&lock);
		if(current_scheduled_process != NULL){
			if (list_empty(&head3) == 0) {
				set_task_state(current_scheduled_process->proc_task_struct, TASK_INTERRUPTIBLE);
				element = list_first_entry(&head3, struct csse_332_info, llist);
				current_scheduled_process = element;
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head3);
				wake_up_process(current_scheduled_process->proc_task_struct);
				time = 1000;
			} else if (list_empty(&head2) == 0) {
				set_task_state(current_scheduled_process->proc_task_struct, TASK_INTERRUPTIBLE);
				element = list_first_entry(&head2, struct csse_332_info, llist);
				current_scheduled_process = element;
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head2);
				wake_up_process(current_scheduled_process->proc_task_struct);
				time = 2000;
			} else if (list_empty(&head1) == 0) {
				set_task_state(current_scheduled_process->proc_task_struct, TASK_INTERRUPTIBLE);
				element = list_first_entry(&head1, struct csse_332_info, llist);
				current_scheduled_process = element;
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head1);
				wake_up_process(current_scheduled_process->proc_task_struct);
				time = 1000;
			} else{
				element = NULL;
				current_scheduled_process = NULL;
			}
		}
		else {
			if(!list_empty(&head3)){
				element = list_first_entry(&head3, struct csse_332_info, llist);
				current_scheduled_process = element;
				wake_up_process(current_scheduled_process->proc_task_struct);
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head3);
				time = 3000;
			} else if(!list_empty(&head2)){
				element = list_first_entry(&head2, struct csse_332_info, llist);
				current_scheduled_process = element;
				wake_up_process(current_scheduled_process->proc_task_struct);
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head2);
				time = 2000;
			} else if(!list_empty(&head1)){
				element = list_first_entry(&head1, struct csse_332_info, llist);
				current_scheduled_process = element;
				wake_up_process(current_scheduled_process->proc_task_struct);
				list_del_init(&element->llist);
				list_add_tail(&element->llist, &head1);
				time = 1000;
			} else {
				element = NULL;
				current_scheduled_process = NULL;
			}
		}
		mutex_unlock(&lock);

		if(element != NULL){
			pr_info("Setting timer to %d\n", time);
			mod_timer(&timer, jiffies + msecs_to_jiffies(time));
		}
		/* tell the scheduler we're ready to leave for now */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();

		/* print hello message and then go back to sleep, unless you should exit */
		pr_info("Hello from kernel thread =)\n");
	}
	return 0;
}



static void timer_callback(struct timer_list *timer){
	//pr_info("Timer is initializing queues");
	//make a new work
	wake_up_process(scheduler_thread);


	// if(!queue_work(work_queue, work)){
	//   //sprintf("work was already queued");
	//   //exit(1);
	//   return;
	// }
}

static void timer_callback_reset(struct timer_list *timer){
  struct csse_332_info *element, *tmp;
  if(!list_empty(&head1)){
    list_for_each_entry_safe(element, tmp, &head1, llist){
      element->quantum = 2;
      list_del_init(&element->llist);
      list_add_tail(&element->llist, &head3);
    }
  }
  if(!list_empty(&head2)){
    list_for_each_entry_safe(element, tmp, &head2, llist){
      element->quantum = 2;
      list_del_init(&element->llist);
      list_add_tail(&element->llist, &head3);
    }
  }
  mod_timer(&reset_timer, jiffies + msecs_to_jiffies(1000));
}

#if 0
static void work_handler(struct work_struct *work){
	//pr_info("Work hander is doing work");
	/* add code here */
	struct list_head *plist;
	list_for_each(plist, &head) {
		struct csse_332_info *element;
		//plist = (struct list_head *) v;
		element = list_entry(plist, struct csse_332_info, llist);
		element->pid = element->proc_task_struct->pid;
		element->name = element->proc_task_struct->comm;

		pr_info("PID: %d Name: %s", element->pid, element->name);
		mutex_lock_interruptible(&lock);
		int i;
		//pr_info("Moving hist");
		for (i = 0;i > HISTORY_LEN-1;i++) {
			element->hist_proc_time[i] = element->hist_proc_time[i+1];
			element->hist_state[i] = element->hist_state[i+1];
			element->hist_cpu[i] = element->hist_cpu[i+1];
		}
		mutex_unlock(&lock);

		//lock
		mutex_lock_interruptible(&lock);
		//pr_info("Updating hist");
		element->hist_proc_time[HISTORY_LEN-1] = element->proc_task_struct->utime;
		element->hist_state[HISTORY_LEN-1] = element->proc_task_struct->state;
		element->hist_cpu[HISTORY_LEN-1] = element->proc_task_struct->cpu;
		mutex_unlock(&lock);
		pr_info("proc time: %lld", element->proc_task_struct->utime);
		if(element->proc_task_struct->utime > thr){
			pr_info("Killing process");
			struct csse_332_info *tmp;
			list_for_each_entry_safe(element, tmp, &element->proc_task_struct->children, llist){
				send_sig_info(SIGKILL, 1, tmp->proc_task_struct);
				list_del(&tmp->llist);
				kfree(tmp);
			} 
			send_sig_info(SIGKILL, 1, element->proc_task_struct);
			list_del(&element->llist);
			kfree(element);

		}

	}
	mod_timer(&timer,jiffies + msecs_to_jiffies(5000));
}
#endif

static ssize_t csse332_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp){
	struct csse_332_info *element, *tmp;
	char inst;
	int val; //first is index to write in the array, second is the value to be put in
	char* inp;
	int current_pid;
	current_pid = current->pid;
	inp = (char*)kmalloc(count + sizeof(char), GFP_KERNEL);
	if(inp == NULL){ //checking if error in mallocing space
		pr_info("Error mallocing...\n");
		return -1;
	}

	copy_from_user(inp, buff, count);
	inp[count] = '\0';

	sscanf(inp, "%c, %d", &inst, &val);

	if(inst == 'A'){
		element = kmalloc(sizeof(struct csse_332_info), GFP_KERNEL);
		element->value = val;
		list_add_tail(&element->llist, &head);
	} else if (inst == 'D') {
		list_for_each_entry_safe(element, tmp, &head, llist){
			if(element->value == val){//Delete Entry
				list_del(&element->llist);
				kfree(element);
			}
		}
	} else if(inst == 'R'){

		//pr_info("in registration");
		if(current_pid == val){
			element = kmalloc(sizeof(struct csse_332_info), GFP_KERNEL);
			//queue_element = kmalloc(sizeof(struct run_queue), GFP_KERNEL);
			int i;
			for (i = 0;i < HISTORY_LEN;i++) {
				element->hist_cpu[i] = -1;
				element->hist_proc_time[i] = -1;
				element->hist_state[i] = -1;
			}
			//pr_info("finished initialization");
			element->pid = val;
			element->proc_task_struct = current;
			element->name = current->comm;
			//lock

			mutex_lock(&lock);
      list_add_tail(&element->llist, &head3);
			// if(which_queue_to_enter == 2){
      //   element->quantum = 2;
			// 	list_add_tail(&element->llist, &head3);
			// 	which_queue_to_enter--;
			// } else if(which_queue_to_enter == 1){
      //   element->quantum = 1;
			// 	list_add_tail(&element->llist, &head2);
			// 	which_queue_to_enter--;
			// }
      // else{
      //   element->quantum = 0;
			// 	list_add_tail(&element->llist, &head1);
			// 	which_queue_to_enter = 2;
			// }
			mutex_unlock(&lock);
			if(current_scheduled_process == NULL){
				wake_up_process(scheduler_thread);
			}

			set_task_state(element->proc_task_struct, TASK_INTERRUPTIBLE);
			//set_current_state(TASK_INTERRUPTIBLE);
			schedule();

		} else{
			pr_info("Process being registered is not current process\n");
			return -1;
		}
	} else if(inst == 'W'){
		if(current->pid == val){
			mutex_lock(&lock);
			if (current_scheduled_process->pid == val) {
				pr_info("Deregistering %d\n", val);
				del_timer(&timer);
				list_del(&current_scheduled_process->llist);
				kfree(current_scheduled_process);
				current_scheduled_process = NULL;
				wake_up_process(scheduler_thread);
			} else {
				pr_err("WTH is happening?");
			}
#if 0
			list_for_each_entry_safe(element, tmp, &head3, llist){
				if(element->pid == val){//Delete Entry
					if(current_scheduled_process == element){
						current_scheduled_process = NULL;
						was_current_process = true;
					}
					list_del(&element->llist);
					pr_info("kfree from head3\n");
					kfree(element);
					found = 1;
				}
			}
			if (!found)
				list_for_each_entry_safe(element, tmp, &head2, llist){
					if(element->pid == val){//Delete Entry
						if(current_scheduled_process == element){
							current_scheduled_process = NULL;
							was_current_process = true;
						}
						list_del(&element->llist);
						pr_info("kfree from head2\n");
						kfree(element);
						found = 1;
					}
				}
			if (!found)
				list_for_each_entry_safe(element, tmp, &head1, llist){
					if(element->pid == val){//Delete Entry
						if(current_scheduled_process == element){
							current_scheduled_process = NULL;
							was_current_process = true;
						}
						list_del(&element->llist);
						kfree(element);
					}
				}
#endif
			mutex_unlock(&lock);
			/* if(was_current_process){ */
			/* 	wake_up_process(scheduler_thread); */
			/* } */
		} else{
			pr_info("Process being de registered is not current process\n");
			return -1;
		}
	} else if(inst == 'Y'){
		//check to see if the yielding process is the currently scheduled one?
		wake_up_process(scheduler_thread);
	}

	else {
		return -EINVAL;
	}

	kfree(inp);
	return count; // please don't remove this line until you are ready to write your own!
}



/*
 * sttostr - Return the string representation of a process state
 *
 * @state: The process state to print
 * @return: A const char * pointer to a string
 */
static const char *sttostr(long state)
{
	if (state == -1)
		return "TASK EMPTY";



	if (!state)
		return "TASK_RUNNING";
	else if (state & TASK_INTERRUPTIBLE)
		return "TASK_INTERRUPTIBLE";
	else if (state & TASK_UNINTERRUPTIBLE)
		return "TASK_UNINTERRUPTIBLE";
	else
		return "TASK_OTHER";
}

static char *p_info(struct csse_332_info* input){
	pr_info("I am running p_info");
	char* to_return = kmalloc(200*sizeof(char), GFP_KERNEL);
	char* first = to_return;
	int moved = sprintf(to_return, "Process Id: %d\n", input->pid);
	//pr_info("%s", to_return);
	//char* temp_string = kmalloc(200*sizeof(char), GFP_KERNEL);
	to_return += moved;
	//sprintf(temp_string, "The string:  %s", "String to be put in");
	//pr_info("%s", temp_string);
	//pr_info("PID: %d", input->pid);
	moved = sprintf(to_return, "Program Name: %s \n", input->name);

	to_return += moved;

	moved = sprintf(to_return, "User Time: %d %d %d %d %d\n", input->hist_proc_time[0], input->hist_proc_time[1], input->hist_proc_time[2], input->hist_proc_time[3], input->hist_proc_time[4]);
	to_return += moved;

	moved = sprintf(to_return, "CPU Used: %d %d %d %d %d\n", input->hist_cpu[0], input->hist_cpu[1], input->hist_cpu[2], input->hist_cpu[3], input->hist_cpu[4]);
	to_return += moved;

	sprintf(to_return, "Sched Status: %s %s %s %s %s\n", sttostr(input->hist_state[0]), sttostr(input->hist_state[1]), sttostr(input->hist_state[2]), sttostr(input->hist_state[3]), sttostr(input->hist_state[4]));
	to_return += moved;

	//access task struct
	moved = sprintf(to_return, "Children:");
	to_return += moved;
	struct list_head *temp;
	list_for_each(temp, &input->proc_task_struct->children){
		struct task_struct* task = list_entry(temp, struct task_struct, sibling);
		moved = sprintf(to_return, "%d %s", task->pid, task->comm);
		to_return += moved;
	}

	sprintf(to_return, "\n");
	return first;
}

static void *csse332_start(struct seq_file *s, loff_t *pos)
{
	seq_printf(s, "[");
	return seq_list_start(&head, *pos);
}

static void *csse332_next(struct seq_file *s, void *v, loff_t *pos)
{
	seq_printf(s, ",");
	return seq_list_next(v, &head, pos);
}

static int csse332_show(struct seq_file *s, void *v)
{
	//pr_info("I am running SHOW");
	struct list_head *plist;
	struct csse_332_info *element;
	plist = (struct list_head *) v;
	element = list_entry(plist, struct csse_332_info, llist);



	//lock?
	mutex_lock(&lock);
	seq_printf(s, "%s", p_info(element));
	mutex_unlock(&lock);


	return 0;
}

static void csse332_stop(struct seq_file *s, void *v)
{
	seq_printf(s, "] \n");
}

static const struct seq_operations csse332_seqops = {
	.start = csse332_start,
	.next  = csse332_next,
	.show  = csse332_show,
	.stop  = csse332_stop,
};

static int
csse332_open(struct inode *inode, struct file *filp)
{
	return seq_open(filp, &csse332_seqops);
}


static const struct proc_ops csse332_fops = {
	.proc_open    = csse332_open,
	.proc_read    = seq_read,
	.proc_write   = csse332_write,
	.proc_lseek   = seq_lseek,
	.proc_release = seq_release,
};

struct proc_dir_entry *parent_entry;
struct proc_dir_entry *child;
struct proc_ops *fops;
static int
__init csse_init(void)
{



	//MILESTONE 1
	pr_info("Module csse332 initializing...\n");
	/* return a non zero value if failure occurs */

	parent_entry = proc_mkdir("csse332", NULL);
	if(parent_entry == NULL){ //checking if error in mallocing space
		pr_info("Error allocating space for entry...\n");
		return -2;
	}

	pr_info("%d", thr);
	//Timer set-up

	timer_setup(&timer, timer_callback, 0); 

  timer_setup(&reset_timer, timer_callback_reset, 0); 

  mod_timer(&reset_timer, jiffies + msecs_to_jiffies(1000));

	INIT_LIST_HEAD(&head1);
	INIT_LIST_HEAD(&head2);
	INIT_LIST_HEAD(&head3);
	/* heads[0] = head1; */
	/* heads[1] = head2; */
	/* heads[2] = head3; */

	child = proc_create("status", 0666, parent_entry, &csse332_fops);
	current_scheduled_process = NULL;
	which_queue_to_enter = 2;
	scheduler_thread = kthread_run(scheduler_fn, NULL, "csse332_scheduler_thread");
	if (!scheduler_thread) {
		pr_err("Could not create kernel thread, abandoning project\n");
		return 0;
	}

	//initialize 3 run queues
	/* for(i = 0; i < MAX_QUEUES; i++){ */
	/*   struct run_queue temp = {(i+1), (i+1), heads[i]}; */
	/* } */

	// //Work Queue Set-up
	/* work = kmalloc(sizeof(struct work_struct), GFP_KERNEL); */
	/* work_queue = create_workqueue("work_queue"); */
	/* INIT_WORK(work, work_handler); */

	return 0;
}



static void
__exit csse_exit(void)
{
	struct csse_332_info *element, *tmp;
	pr_info("Module csse332 exiting...\n");
	/* Do exit stuff here... */
	del_timer(&timer);
	kthread_stop(scheduler_thread);

	list_for_each_entry_safe(element, tmp, &head, llist){
		list_del(&element->llist);
		kfree(element);
	}

	remove_proc_entry("status", parent_entry);
	remove_proc_entry("csse332", NULL);
}



module_init(csse_init);
module_exit(csse_exit);
