#include "sched.h"
static void put_prev_task_mypriority(struct rq *rq, struct task_struct *p);
static int select_task_rq_mypriority(struct task_struct *p, int cpu, int sd_flag, int flags);
static void set_curr_task_mypriority(struct rq *rq);
static void task_tick_mypriority(struct rq *rq,struct task_struct *p, int oldprio);
static void switched_to_mypriority(struct rq *rq, struct task_struct *p);
void init_mypriority_rq(struct mypriority_rq *mypriority_rq);
void init_task_mypriority(struct task_struct *p);
static void update_curr_mypriority(struct rq *rq);
static void enqueue_task_mypriority(struct rq *rq, struct task_struct *p, int flags);
static void dequeue_task_mypriority(struct rq *rq, struct task_struct *p, int flags);
static void check_preempt_curr_mypriority(struct rq *rq, struct task_struct *p,int flags);
struct task_struct *pick_next_task_mypriority(struct rq *rq, struct task_struct *prev);
static void prio_changed_mypriority(struct rq *rq, struct task_struct *p, int oldprio);


const struct sched_class mypriority_sched_class={
    .next=&mysched_sched_class,
    .enqueue_task=&enqueue_task_mypriority,
    .dequeue_task=dequeue_task_mypriority,
    .check_preempt_curr=check_preempt_curr_mypriority,
    .pick_next_task=pick_next_task_mypriority,
    .put_prev_task=put_prev_task_mypriority,
#ifdef CONFIG_SMP
    .select_task_rq=select_task_rq_mypriority,
#endif
    .set_curr_task=set_curr_task_mypriority,
    .task_tick=task_tick_mypriority,
    .prio_changed=prio_changed_mypriority,
    .switched_to=switched_to_mypriority,
    .update_curr=update_curr_mypriority,
};

void init_task_mypriority(struct task_struct *p)
{
    // init task
    struct sched_mypriority_entity *se = &p->mypriority;
    p->sched_class = &mypriority_sched_class;
    // default aging is 1
    se->aging = 1;
}


void init_mypriority_rq (struct mypriority_rq *mypriority_rq)
{
    printk(KERN_INFO "***[MYPRIORITY] Mypriority class is online \n");
    mypriority_rq->nr_running = 0;
    // max priority of entitiy in run queue; default is 0
    mypriority_rq->max_prio = 0;
    INIT_LIST_HEAD(&mypriority_rq->queue);
}
static void update_curr_mypriority(struct rq *rq){
    struct task_struct *curr = rq->curr;
    struct sched_mypriority_entity *mypriority = &curr->mypriority;
    struct mypriority_rq *mypriority_rq = &rq->mypriority;
    struct task_struct *task = NULL;
    
    if(curr->sched_class != &mypriority_sched_class)
        return;
    unsigned int sum;
    struct list_head *p = &mypriority_rq->queue;
    struct sched_mypriority_entity *my = NULL;
    sum = 0;
    list_for_each_entry(my, p, run_list) {
        //sum of aging
        sum += my->aging;
    }
    int average;
    // average of aging
    average = sum / mypriority_rq->nr_running;
    printk(KERN_INFO "***[MYPRIORITY] update_curr cpu=%d sum=%u nr_running=%u, average=%d\n",cpu_of(rq), sum, mypriority_rq->nr_running, average);
    
    // if entity's aging is larger than average aging, ++priority
    list_for_each_entry(my, p, run_list) {
        // if entitiy is currunt, skip
        if (my->priority == mypriority_rq->max_prio)
            continue;
        if (my->aging > average) {
            my->priority++;
            task = container_of(my, struct task_struct, mypriority);
            printk(KERN_INFO "***[MYPRIORITY] update PRIORITY\tcpu=%d pid=%ld priority=%u aging=%u average=%d\n",cpu_of(rq), (long)task->pid, my->priority,my->aging, average);
        }
    }
    
    // increase aging about 1 except current entity
    list_for_each_entry(my, p, run_list) {
        //aging 값++
        if (my->priority == mypriority_rq->max_prio)
            continue;
        my->aging++;
    }
    
}

static void enqueue_task_mypriority(struct rq *rq, struct task_struct *p, int flags) {
    struct mypriority_rq *mypriority_rq = &rq->mypriority;
    struct sched_mypriority_entity *mypriority = &p->mypriority;
    
    // check if the entity that has just entered has the highest priority
    if (mypriority->priority > mypriority_rq->max_prio){
        // if it is greater than the max value, the max value is updated and pushed to the front of the queue
        mypriority_rq->max_prio = mypriority->priority;
        list_add(&mypriority->run_list, &mypriority_rq->queue);
        mypriority_rq->nr_running++;
        printk(KERN_INFO "***[MYPRIORITY] enqueue and PREEMPTION: success cpu=%d, nr_running=%u,priority=%u, aging=%u, p->pid=%ld\n", cpu_of(rq), mypriority_rq->nr_running, mypriority->priority, mypriority->aging, (long)p->pid);
        // reschedule for preemption
        resched_curr(rq);
    }
    else {
        // if it is not the highest priority, it is put at the end of the queue
        list_add_tail(&mypriority->run_list, &mypriority_rq->queue);
        mypriority_rq->nr_running++;
        printk(KERN_INFO "***[MYPRIORITY] enqueue: success cpu=%d, nr_running=%u,priority=%u, aging=%u, p->pid=%ld\n", cpu_of(rq), mypriority_rq->nr_running, mypriority->priority, mypriority->aging, (long)p->pid);
    }
    
}
static void dequeue_task_mypriority(struct rq *rq, struct task_struct *p, int flags)
{
    struct mypriority_rq *mypriority_rq = &rq->mypriority;
    struct sched_mypriority_entity *mypriority = &p->mypriority;
    
    // remove the entity from the front of the queue
    list_del_init(&mypriority->run_list);
    mypriority_rq->nr_running--;
    
    // find the max priority among the entity remaining after removal
    unsigned int max = 0;
    struct list_head *l = &mypriority_rq->queue;
    struct sched_mypriority_entity *my = NULL;
    list_for_each_entry(my, l, run_list) {
        if (my->priority >= max)
            max = my->priority;
    }
    // update max priority
    mypriority_rq->max_prio = max;
    
    printk(KERN_INFO "***[MYPRIORITY] dequeue: success cpu=%d, nr_running=%u, priority=%u, aging=%u, p->pid=%ld\n", cpu_of(rq), mypriority_rq->nr_running, mypriority->priority, mypriority->aging, (long)p->pid);
    // reset aging
    mypriority->aging = 1;
}
void check_preempt_curr_mypriority(struct rq *rq, struct task_struct *p, int flags) {
    printk("***[MYPRIORITY] check_preempt_curr_mypriority\n");
}
struct task_struct *pick_next_task_mypriority(struct rq *rq, struct task_struct *prev)
{
    struct mypriority_rq *mypriority_rq = NULL;
    struct list_head *p = NULL;
    struct sched_mypriority_entity *my;
    struct task_struct *next_task = NULL;
    if(rq->mypriority.nr_running == 0){
        return NULL;
    }
    if (prev->sched_class != &mypriority_sched_class) put_prev_task(rq, prev);
    mypriority_rq = &rq->mypriority;
    p = &mypriority_rq->queue;
    // list traverse
    list_for_each_entry(my, p, run_list){
        // pick the highest priority process and select it as the next task
        if (my->priority >= mypriority_rq->max_prio) {
            next_task = container_of(my, struct task_struct, mypriority);
            // move next task to the front of the queue
            list_move(&my->run_list, &mypriority_rq->queue);
            break;
        }
    }
    
    printk(KERN_INFO " ***[MYPRIORITY] pick_next_task: cpu=%d,prev->pid=%ld,next_p->pid=%ld,nr_running=%u, priority=%u aging=%u\n", cpu_of(rq), (long)prev->pid, (long)next_task->pid, mypriority_rq->nr_running, my->priority, my->aging);
    
    return next_task;
}
void put_prev_task_mypriority(struct rq *rq, struct task_struct *p) {
    printk(KERN_INFO "\t***[MYPRIORITY] put_prev_task: do_nothing, p->pid=%d\n",p->pid);
}
int select_task_rq_mypriority(struct task_struct *p, int cpu, int sd_flag, int flags){return task_cpu(p);}
void set_curr_task_mypriority(struct rq *rq){
    printk(KERN_INFO"***[MYPRIORITY] set_curr_task_mypriority\n");
}
void task_tick_mypriority(struct rq *rq, struct task_struct *p, int queued) {
    update_curr_mypriority(rq);
}
void prio_changed_mypriority(struct rq *rq, struct task_struct *p, int oldprio) { }
/*This routine is called when a task migrates between classes*/
void switched_to_mypriority(struct rq *rq, struct task_struct *p)
{
    resched_curr(rq);
}
