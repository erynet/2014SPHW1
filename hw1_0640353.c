#include <linux/module.h>
#include <linux/tty.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>

#include <linux/jiffies.h>
#include <linux/delay.h>

#define procfs_name "hw1"

struct thread_info *thread;
struct task_struct *task;
struct seq_file m;

int total = 0, running = 0, sleeping = 0, stopped = 0, zombie = 0, dead = 0;

unsigned char pid_v[65536] = {0,};
unsigned int ppid_v[65536] = {0,};
char comm_v[65536][50];
unsigned char state_v[65536] = {0,};
unsigned long start_v[65536] = {0,};
unsigned long long user_v[65536] = {0,};
unsigned long long kernel_v[65536] = {0,};
unsigned int nvcsw_v[65536] = {0,};
unsigned int nivcsw_v[65526] = {0,};
unsigned long long user, kernel, entire;

static void calc(void){
	unsigned int pid;
	unsigned int i = 0;
	
	unsigned long tsec, tnsec;
	
	for(i = 0; i < 65536; i++)
		pid_v[i] = 0;
	
	total = 0, running = 0, sleeping = 0, stopped = 0, zombie = 0, dead = 0;
	for_each_process(task){
		
		pid = task->pid;
		pid_v[pid] = 1;
		strcpy(comm_v[pid], task->comm);
		ppid_v[pid] = task->real_parent->pid;
		
		user = jiffies_to_msecs(task->utime + task->signal->cutime + task->signal->utime);
		user_v[pid] = user;
		kernel = jiffies_to_msecs(task->stime + task->signal->cstime + task->signal->stime);
		kernel_v[pid] = kernel;
		//entire = user + kernel;
		
		tsec = (task->real_start_time).tv_sec;
		tsec = tsec * 1000; // convert sec -> m_sec.
		tnsec = (task->real_start_time).tv_nsec; //get boot based start time nano second value.
		do_div(tnsec, 1000000); // convert n_sec -> m_sec
		start_v[pid] = (tsec + tnsec);
		
		//start = (tsec + tnsec); // boot base의 시작시간인 나노초와 초단위의 값을 ms단위로 바꾸어 더해준다.
		
		switch(task->state) {
			case TASK_RUNNING:
					running++;
					state_v[pid] = 0;
					break;
			case __TASK_STOPPED:
					stopped++;
					state_v[pid] = 1;
					break;
			case TASK_WAKEKILL:
					stopped++;
					sleeping++;
					state_v[pid] = 2;
					break;
			case TASK_UNINTERRUPTIBLE:
					sleeping++;
					state_v[pid] = 2;
					break;
			case TASK_INTERRUPTIBLE:
					sleeping++;
					state_v[pid] = 2;
					break;
			case TASK_WAKING:
					sleeping++;
					state_v[pid] = 2;
					break;
			case __TASK_TRACED:
					running++;
					state_v[pid] = 0;
					break;
			case TASK_PARKED:
					sleeping++;
					state_v[pid] = 2;
					break;
			case TASK_STATE_MAX:
					break;
		}

		switch(task->exit_state) {
			case EXIT_ZOMBIE:
					zombie++;
					state_v[pid] = 3;
					break;
			case EXIT_DEAD:
					zombie++;
					state_v[pid] = 3;
					break;
		}
	}
}

static void *my_seq_start(struct seq_file *s, loff_t *pos){
	static unsigned long counter = 0;

	printk(KERN_INFO "Sequence Start!\n");
	if(*pos == 0){
		return &counter;
	}
	else{
		*pos = 0;
		return NULL;
	}
}
 
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos){
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	(*pos)++;
	printk(KERN_INFO "Sequence Next : \n");
	return NULL;
}

static void my_seq_stop(struct seq_file *s, void *v) {
	printk(KERN_INFO "Sequence Stop!!!\n");
}

static int my_seq_show(struct seq_file *s, void *v){
	unsigned long frequency = HZ; // HZ(250) 정보를 가져온다.
	unsigned long now_time = jiffies_to_msecs(jiffies - INITIAL_JIFFIES);
	
	unsigned long start; // 각 task의 시작시간을 계산하기 위한 변수.
	unsigned long long user, kernel, entire; // declare for user time, kernel time, total time.
	unsigned int  mstart=0, mentire=0, muser=0,mkernel=0; // ms단위를 소수점으로 출력해주기 위한 변수.
	unsigned long long utime, stime, chd_utime, chd_stime, thd_utime, thd_stime;
	// user와 kernel 시간을 얻기위해서는 cpu와 child, thread에서의 누적 시간으로 구해야 한다.
	unsigned long tsec, tnsec;
	// boot based time을 얻기위한 변수들.
	unsigned int i = 0;
	
	//debug
	calc();
	//debug
	
	printk(KERN_INFO "Sequence show!!\n");

	seq_printf(s, "-------------------------------------------------------------------------------------------------------------------\n");
	seq_printf(s, "                                              0640353 Dong-Hoe Lee\n");
	seq_printf(s, "-------------------------------------------------------------------------------------------------------------------\n");	
	seq_printf(s, "CURRENT SYSTEM INFORMATION >\n");
	seq_printf(s, "Total %d task, %d running, %d sleeping, %d stopped, %d zombies, %d dead\n",total,running,sleeping,stopped,zombie,dead);
	// init 함수에서 얻은 task 정보들을 출력해준다.
	seq_printf(s, "%ldHz, %ld ms after system boot time\n", frequency, now_time);
	// frequency와 현재시간을 출력해준다.
	seq_printf(s, "-------------------------------------------------------------------------------------------------------------------\n");	
	seq_printf(s,"            Command   pid   ppid     start(s)     total(s)      user(s)    kernel(s)     voluntary     involuntary\n");
	seq_printf(s, "-------------------------------------------------------------------------------------------------------------------\n");	
	
	for(i = 0; i < 65526; i++){
		if(pid_v[i] == 0)
			continue;
		
		start = start_v[i];
		mstart = do_div(start, 1000);
		
		user = user_v[i];
		kernel = kernel_v[i];
		entire = user + kernel;
		mentire = do_div(entire, 1000);
		muser = do_div(user, 1000);
		mkernel = do_div(kernel, 1000);
		
		seq_printf(s, "%19s%6d%7d%9ld.",comm_v[i], pid_v[i], ppid_v[i], start);
		if(mstart < 10)
			seq_printf(s,"00%1d%9lld.", mstart, entire);
		else if(mstart < 100)
			seq_printf(s,"0%2d%9lld.", mstart, entire);
		else
			seq_printf(s,"%3d%9lld.", mstart, entire);
			
		if(mentire < 10)
			seq_printf(s,"00%1d%9lld.",mentire,user);
		else if(mentire < 100)
			seq_printf(s,"0%2d%9lld.",mentire,user);
		else
			seq_printf(s,"%3d%9lld.",mentire,user); // 전체 수행시간의 밀리초 단위와 user_time의 초단위를 출력한다.

		if(muser < 10)
			seq_printf(s,"00%1d%9lld.",muser,kernel);
		else if(muser < 100)
			seq_printf(s,"0%2d%9lld.",muser,kernel);
		else
			seq_printf(s,"%3d%9lld.",muser,kernel); // user_time의 밀리초 단위와 kernel_time의 초단위를 출력한다.

		if(mkernel < 10)
			seq_printf(s,"00%1d%14ld%16ld\n",mkernel,task->nvcsw,task->nivcsw);
		else if(mkernel < 100)
			seq_printf(s,"0%2d%14ld%16ld\n",mkernel,task->nvcsw,task->nivcsw);
		else
			seq_printf(s,"%3d%14ld%16ld\n",mkernel,task->nvcsw,task->nivcsw);
	}
	
}

static struct seq_operations my_seq_ops = { // seq 수행 시에 대한 함수들을 정의한다.
	.start	= my_seq_start,
	.next	= my_seq_next,
	.stop	= my_seq_stop,
	.show	= my_seq_show
};

static int my_open(struct inode *inode, struct file *file){
	return seq_open(file, &my_seq_ops); // my_open 함수가 실행되면 seq_open이 수행된다.
};

static int my_release(struct inode *inode, struct file *file){
	return seq_release(inode, file);
}

static struct file_operations my_file_ops = { // my_file_ops로 module에 대응되는 값들을 정의한다.
	.owner	= THIS_MODULE,
	.open	= my_open,
	.read	= seq_read,
	.llseek = seq_lseek,
	.release = my_release
};

static int __init __hw1_init(void){
	printk(KERN_INFO "hw1 project start!\n");
	
	struct roc_dir_entry * proc_file;
	
	//thread = current_thread_info();
	//task = thread->task;
	
	thread = current_thread_info();
	task = thread->task;
	
	proc_file = proc_create(procfs_name, 0444, NULL, &my_file_ops);

	if(!proc_file)
	{
		printk(KERN_ERR "cannot create proc file...\n");
		remove_proc_entry(procfs_name, 0);
		return -ENOMEM;
	}
	
	printk(KERN_INFO "HW1_Project Initialized!\n");
	
	return 0;
}

static void __exit __hw1_exit(void){
	printk(KERN_INFO "hw1 project end!\n");
	
	remove_proc_entry(procfs_name, 0);
}

module_init(__hw1_init);
module_exit(__hw1_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("0640353-LDH");
MODULE_DESCRIPTION("task_struct Information");
