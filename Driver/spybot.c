#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <asm/desc_defs.h>
#include <asm/desc.h>
#include <linux/smp.h>
#include <linux/mutex.h>

#define MODULE_NAME "spybot"
#define ERROR_OP -1
#define KILL 0
#define SUSPEND 1
#define CONT 2
#define SCT_SCAN 3
#define SCT_FIX 4
#define IDT_SCAN 5
#define GET_HOOK 6
#define SPYBOT_IOC_MAGIC 'k'
#define SPYBOT_IOC_SEND _IOWR(SPYBOT_IOC_MAGIC, 1, int[2])
#define SPYBOT_IOC_RECV _IOWR(SPYBOT_IOC_MAGIC, 1, int)
#define MIN_HOOK_ID 1000
#define MAX_HOOK_ID 9999
#define MAX_IDT_SIZE 1 << 5 // = 32 - shift 1 to right 5 times


/* Driver Description. */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SpyBot");
MODULE_DESCRIPTION("SpyBot module for system management");

/* get the address of the kallsyms_lookup_name function that help to get the syscall table address */
static unsigned long (*gkallsyms_lookup_name)(const char *name);
static unsigned long kallsyms_lookup_addr;
module_param(kallsyms_lookup_addr, ulong, S_IRUGO);

/* Device variables */
static int major_number;
static struct cdev spybot_cdev;

/**/
static const struct file_operations spybot_fops;
struct gate_struct old_gate, new_gate;

/* idt scan params */
struct desc_ptr idtr;
struct gate_struct temp_idt[MAX_IDT_SIZE];

/* sct data */
unsigned long *sys_call_table;
unsigned long temp_sct[__NR_syscalls];

/* sct hook vars */
asmlinkage int (*execve_scf)(const struct pt_regs *regs);

unsigned int hook_id = MIN_HOOK_ID;
static DEFINE_MUTEX(hook_mutex);

/* hook data */
struct syscall_hook{
    int hook_id;
    int pid;
    int parent_pid;
    struct syscall_hook *next; 
};

struct syscall_hook *hooks_list; 

int result = 0;  // Default result value

/* Functions Signatures */
static bool sendSignalToTask(int target_pid, int sig);
static long spybot_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
void syscallHijack(void);
void removeSyscallHijack(void);
int setPage_ro(unsigned long addr);
int setPage_rw(unsigned long addr);
asmlinkage int new_execve(const struct pt_regs *regs);
void add_hook(struct syscall_hook **head, struct syscall_hook* obj);
struct syscall_hook * pop_hook(struct syscall_hook **head);
void create_temp_sct(void);
int compare_scts(void);
bool fix_sct(int syscall);
void create_temp_idt(void);
int compare_idts(void);

/* 
This function is the init function when the driver enters the kernel 
*/
static int __init init_spybot(void) {
    printk(KERN_INFO "%s: msg | log: The Spybot module is loading...\n", MODULE_NAME);
    
    //get the idt address and size and store it in idtr.
    store_idt(&idtr);

    /* Copy the idt to temp idt */
    create_temp_idt();

    //get the kallsyms_lookup name address by paramter, then get the syscall table address with that function.
    gkallsyms_lookup_name = (void*) kallsyms_lookup_addr;
    sys_call_table = (unsigned long*)gkallsyms_lookup_name("sys_call_table");

    /* Process the hijack of the execve syscall */    
    syscallHijack();

    /* Copy the sct to temp sct */
    create_temp_sct();
    

    // Register character device.
    major_number = register_chrdev(0, MODULE_NAME, &spybot_fops);
    if (major_number < 0) {
        printk(KERN_ERR "%s: error | log: Failed to register a major number\n", MODULE_NAME);
        return major_number;
    }
    printk(KERN_INFO "%s: msg | log: Major number allocated: %d\n", MODULE_NAME, major_number);

    // Initialize cdev.
    cdev_init(&spybot_cdev, &spybot_fops);

    // Add cdev to the system.
    if (cdev_add(&spybot_cdev, MKDEV(major_number, 0), 1) < 0) {
        printk(KERN_ERR "%s: error | log: Failed to add cdev\n", MODULE_NAME);
        unregister_chrdev(major_number, MODULE_NAME);
        return -1;
    }

    //log message when the module successfully loaded.
    printk(KERN_INFO "%s: msg | log: The Spybot module has successfully loaded!\n", MODULE_NAME);

    return 0;
}

/* 
This function process the signal sending.
input:
target_pid - the pid of the target process.
sig - the signal that we want to send to the process.
output: if the signale send successfully.
*/
static bool sendSignalToTask(int target_pid, int sig) {
    struct task_struct *target_task;

    // get the process task to kill
    target_task = pid_task(find_vpid(+target_pid), PIDTYPE_PID);
    
    // check if the task exists
    if (target_task) {
        // send the signal to the process
        send_sig(sig, target_task, 0);
        return true;  // Success
    } else {
        return false;  // Failure
    }
}

/*
This function do the main tasks by communicate with the process communicator in the backend.
input:
file - 
cmd - 
arg - 
output: Errors
*/
static long spybot_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int param_1;
    int option;
    struct syscall_hook hook_data;
    struct syscall_hook *tmp;

    switch (cmd) {
        case SPYBOT_IOC_SEND: {
            int user_data[2];  // Temporary buffer for copying data

            // Copy data from user space to kernel space
            if (copy_from_user(user_data, (int __user *)arg, sizeof(user_data))) {
                return -EFAULT; // Error copying data from user space
            }

            option = user_data[0];  // Extract the signal value
            param_1 = user_data[1];  // Extract the target PID

            switch (option) {
                case KILL:
                    printk(KERN_INFO "%s: msg | log: Received request to KILL process with PID %d.\n", MODULE_NAME, param_1);
                    // Call the existing function and set result accordingly
                    result = sendSignalToTask(param_1, SIGKILL) ? 1 : 0;
                    break;

                case SUSPEND:
                    printk(KERN_INFO "%s: msg | log: Received request to STOP process with PID %d.\n", MODULE_NAME, param_1);
                    // Call the existing function and set result accordingly
                    result = sendSignalToTask(param_1, SIGSTOP) ? 1 : 0;
                    break;

                case CONT:
                    printk(KERN_INFO "%s: msg | log: Received request to CONTINUE process with PID %d.\n", MODULE_NAME, param_1);
                    // Call the existing function and set result accordingly
                    result = sendSignalToTask(param_1, SIGCONT) ? 1 : 0;
                    break;
                
                case SCT_SCAN:
                    printk(KERN_INFO "%s: msg | log: Starting sct scanning...\n", MODULE_NAME);
                    // Start compare process between the temp sct and the real sct
                    result = compare_scts();
                    break;
                
                case SCT_FIX:
                    printk(KERN_INFO "%s: msg | log: Trying to fix the syscall location(0x%x)\n", MODULE_NAME, param_1);
                    // Try to fix the syscall
                    result = fix_sct(param_1) ? 1 : 0;
                    break;

                case IDT_SCAN:
                    printk(KERN_INFO "%s: msg | log: Starting idt scanning...\n", MODULE_NAME);
                    // Start compare process between the temp idt and the real idt
                    result = compare_idts();
                    break;

                case GET_HOOK:
                    printk(KERN_INFO "%s: msg | log: Return the first struct of hook data from the hooks_list\n", MODULE_NAME);                                        

                    mutex_lock(&hook_mutex);
                    tmp = pop_hook(&hooks_list);
                    mutex_unlock(&hook_mutex);
                    
                    hook_data.pid = tmp->pid;
                    hook_data.parent_pid = tmp->parent_pid;
                    hook_data.hook_id = tmp->hook_id;
                    kfree(tmp);

                    // Copy the result back to user space
                    if (copy_to_user((int __user *)arg, &hook_data, sizeof(struct syscall_hook))) {
                        return -EFAULT; // Error copying data to user space
                    }

                    return 0;

                default:
                    return -EINVAL; // Invalid option value
            }            

            break;
        }
        case SPYBOT_IOC_RECV: {
            // Copy the result back to user space
            if (copy_to_user((int __user *)arg, &result, sizeof(int))) {
                return -EFAULT; // Error copying data to user space
            }
            break;
        }

        default:
            return -ENOTTY; // Not a valid ioctl command
    }

    return 0; // Success
}

/*
This functions hijack syscalls.
*/
void syscallHijack(void) {
    //set the permissions of the syscall table page to read & write
    setPage_rw((unsigned long)sys_call_table);

    //getting and save the old address of execve syscall
    execve_scf = (void*) sys_call_table[__NR_execve];

    //hook the execve syscall by the new execve function
    sys_call_table[__NR_execve] = (unsigned long)new_execve;

    //change the permissions of the syscall tabel page to read only
    setPage_ro((unsigned long)sys_call_table);

    //log after the hook successfully enter
    printk(KERN_INFO "%s: hook_process | syscall: execve | log: hook process enter successfully\n", MODULE_NAME);
}

/*
This functions clean the syscall table from all the syscall hijacks.
*/
void removeSyscallHijack(void) {
    //set the permissions of the syscall table page to read & write
    setPage_rw((unsigned long)sys_call_table);

    //change the execve address to the old execve syscall address
    sys_call_table[__NR_execve] = (unsigned long)execve_scf;

    //change the permissions of the syscall tabel page to read only
    setPage_ro((unsigned long)sys_call_table);

    //log after the hook exit successfully exit
    printk(KERN_INFO "%s: hook_process | syscall: execve | log: hook process exit successfully\n", MODULE_NAME);
}

/*
This function is used for hijack the execve syscall, 
so we can get details about all the processes that use that syscall.
input: 
regs - the input that the real execve syscall need.
output: the output of the execve syscall.
*/
asmlinkage int new_execve(const struct pt_regs *regs) {
    struct syscall_hook *hook_data = (struct syscall_hook *)kmalloc(sizeof(struct syscall_hook), GFP_KERNEL);
    hook_data->pid = current->pid;
    hook_data->parent_pid = current->parent->pid;
    hook_data->hook_id = hook_id;

    mutex_lock(&hook_mutex);
    add_hook(&hooks_list, hook_data);
    mutex_unlock(&hook_mutex);

    //print the pid of the parenr process that run the execve syscall and the pid of the child process that run the syscall
    printk(KERN_INFO "%s: execve_hook | syscall: execve | hook_id: %d | parent_pid: %d | pid: %d\n", MODULE_NAME, hook_id, current->parent->pid, current->pid);

    hook_id++;
    if(hook_id > MAX_HOOK_ID){
        hook_id = MIN_HOOK_ID;
    }
    
    return (*execve_scf)(regs);
}

/*
This function add object to the end of the linked list.
input: 
head - the head of the linked list.
obj - the object to add in the end of the list.
output: void.
*/
void add_hook(struct syscall_hook **head, struct syscall_hook* obj){    
    //check if the list isnt empty.
    if(*head){
        //check if the next object from the head isnt empty
        if((*head)->next){          
            //run the function again until it will return the last object in the list
            add_hook(&((*head)->next), obj);
        }
        else{
            (*head)->next = obj;
        }
    }
    else{
        *head = obj;
    }
}

/*
This function return and delete the first struct object from the linked list.
input:
head - the head of the linked list.
output: the first object from the linked list.
*/
struct syscall_hook *pop_hook(struct syscall_hook **head){    
    struct syscall_hook *hook_data = NULL;

    //check if the linked list isnt empty.
    if(*head) {
        //set the head of the list to the next object in the list.
        hook_data = *head;
        *head = (*head)->next;
    }
    
    return hook_data;
}

/*
This function change the permissions of page in the memory to 'ro' - "read only".
input: address of the page.
output: 0.
*/
int setPage_ro(unsigned long addr) {
        unsigned int level;
        pte_t *pte;

        //get the page struct
        pte = lookup_address(addr, &level);

        //change the permission of the page to read only
        pte->pte = pte->pte &~_PAGE_RW;

        return 0;
}

/*
This function change the permissions of page in the memory to 'rw' - "read & write".
input: address of the page.
output: 0.
*/
int setPage_rw(unsigned long addr) {

        unsigned int level;
        pte_t *pte;

        //get the page struct
        pte = lookup_address(addr, &level);

        //change the permission of the page to read and write
        if (pte->pte &~ _PAGE_RW) {
                pte->pte |= _PAGE_RW;
        }

        return 0;
}

/*
This function copy the real sct to temp sct table.
input: void.
output: void.
*/
void create_temp_sct(void){
    int i = 0;

//loop that copy all the syscalls from the real sct to the temp sct.
    for(i = 0; i < __NR_syscalls; i++){
        temp_sct[i] = sys_call_table[i];
    }
}

/*
This function compare between the real sct to the temp sct,
if it detect change in the real sct it will return the index of the syscall that has been changed, 
and if the real sct is valid it will return -1.
input: void.
output: the index of the syscall that has been changed or -1 if all of the syscalls are valid.
*/
int compare_scts(void){
    int i = 0;

    //loop that compare the sct syscalls with the temp sct.
    for(i = 0; i < __NR_syscalls; i++){
        //check if that syscall has been changed or it valid.
        if(sys_call_table[i] != temp_sct[i]){
            printk(KERN_ALERT "%s: msg | alert: hook found in syscall number 0x%x, the real address 0x%lx, hook address 0x%lx\n", MODULE_NAME, i, temp_sct[i], sys_call_table[i]);
            return i;
        }            
    }    

    //print log message if the sct is valid.
    printk(KERN_INFO "%s: msg | log: sct is ok\n", MODULE_NAME);

    return -1;
}

/*
This function get index of syscall in the sct and fix it.
input:
syscall - the index of the syscall to fix.
output: if the syscall index is valid or not.
*/
bool fix_sct(int syscall){
    bool is_fixed = false;

    //check if the syscall index is in the valid range or not.
    if(syscall >= 0 && syscall <= __NR_syscalls){
        //fix the syscall in that index.
        setPage_rw((unsigned long)sys_call_table);
        sys_call_table[syscall] = temp_sct[syscall];
        setPage_ro((unsigned long)sys_call_table);
        printk(KERN_INFO "%s: msg | log: Syscall location(0x%x) Successfully fixed\n", MODULE_NAME, syscall);
        is_fixed = true;
    }
    else {
        //the syscall index is out of the valid range.
        printk(KERN_WARNING "%s: msg | warning: Syscall location(0x%x) out of range\n", MODULE_NAME, syscall);
        is_fixed = false;
    
    }

    return is_fixed;
}

/*
This function copy the real idt to temp idt table.
input: void.
output: void.
*/
void create_temp_idt(void) {
    unsigned short entry;

    //loop that copy all the idt entrys to the temp idt table. 
    for (entry = 0; entry < MAX_IDT_SIZE; entry++) {
        temp_idt[entry] = *((struct gate_struct *)idtr.address + entry);
    }
}

/*
This function compare between the temp idt to the real idt, 
and if it detect any change in the real idt it return the index of the entry that changed.
input: void.
output: the index of the entry that has been changed in the idt or -1 if the idt is valid.
*/
int compare_idts(void) {
    unsigned short entry;
    struct gate_struct tmp_gate;

    //loop that check all the idt entrys.
    for (entry = 0; entry < MAX_IDT_SIZE; entry++) {
        tmp_gate = ((struct gate_struct *)idtr.address)[entry];
        if (temp_idt[entry].offset_low    != tmp_gate.offset_low ||
            temp_idt[entry].offset_middle != tmp_gate.offset_middle

//This line will run only if the macro 'CONFIG_X86_64' exists, 
//because only in 64-bit x86 architecture the 'offset_high' exists
#ifdef CONFIG_X86_64
            || temp_idt[entry].offset_high != tmp_gate.offset_high
#endif
      ) {
            printk(KERN_ALERT "%s: msg | alert: hook found in IDT-Entry number 0x%x\n", MODULE_NAME, entry);
            return entry;
        }
    }

    printk(KERN_INFO "%s: msg | log: IDT is ok\n", MODULE_NAME);

    return -1;
}


/* This function is the exit function when the driver exits the kernel */
static void __exit cleanup_spybot(void) {
    struct syscall_hook *hook_data;
    
    //remove the hook function from the syscall table
    removeSyscallHijack();

    
    //clean and free all the hook-data structs from the hooks_list
    while((hook_data = pop_hook(&hooks_list)) != NULL) {
        kfree(hook_data);        
    }

    //
    cdev_del(&spybot_cdev);
    
    //
    unregister_chrdev(major_number, MODULE_NAME);

    //log message after all the process tasks finished
    printk(KERN_INFO "%s: msg | log: Unloading spybot module.\n", MODULE_NAME);
}

// File operations structure
static const struct file_operations spybot_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = spybot_ioctl,
    // Include other necessary file operations
};

// Set the init and exit functions
module_init(init_spybot);
module_exit(cleanup_spybot);
