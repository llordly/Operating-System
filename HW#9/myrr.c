#include "sched.h"
static void put_prev_task_myrr(struct rq *rq, struct task_struct *p);
static int select_task_rq_myrr(struct task_struct *p, int cpu, int sd_flag, int flags);
static void set_curr_task_myrr(struct rq *rq);
static void task_tick_myrr(struct rq *rq,struct task_struct *p, int oldprio);
static void switched_to_myrr(struct rq *rq, struct task_struct *p);
void init_myrr_rq(struct myrr_rq *myrr_rq);
static void update_curr_myrr(struct rq *rq);
static void enqueue_task_myrr(struct rq *rq, struct task_struct *p, int flags);
static void dequeue_task_myrr(struct rq *rq, struct task_struct *p, int flags);
static void check_preempt_curr_myrr(struct rq *rq, struct task_struct *p,int flags);
struct task_struct *pick_next_task_myrr(struct rq *rq, struct task_struct *prev);
static void prio_changed_myrr(struct rq *rq, struct task_struct *p, int oldprio);

#define MYRR_TIME_SLICE 4

const struct sched_class myrr_sched_class={
    .next=&fair_sched_class,
    .enqueue_task=&enqueue_task_myrr,
    .dequeue_task=dequeue_task_myrr,
    .check_preempt_curr=check_preempt_curr_myrr,
    .pick_next_task=pick_next_task_myrr,
    .put_prev_task=put_prev_task_myrr,
#ifdef CONFIG_SMP
    .select_task_rq=select_task_rq_myrr,
#endif
    .set_curr_task=set_curr_task_myrr,
    .task_tick=task_tick_myrr,
    .prio_changed=prio_changed_myrr,
    .switched_to=switched_to_myrr,
    .update_curr=update_curr_myrr,
};


void init_myrr_rq (struct myrr_rq *myrr_rq)
{
    printk(KERN_INFO "***[MYRR] Myrr class is online \n");
    myrr_rq->nr_running = 0;
    INIT_LIST_HEAD(&myrr_rq->queue);
}
static void update_curr_myrr(struct rq *rq){
    struct task_struct *curr = rq->curr;
    struct sched_myrr_entity *myrr = &curr->myrr;
    
    if(curr->sched_class != &myrr_sched_class)
        return;
    
    myrr->update_num++;
    printk(KERN_INFO "***[MYRR] update_curr_myrr\tpid=%ld update_num=%u\n", (long)curr->pid, myrr->update_num);
    
    if(myrr->update_num > MYRR_TIME_SLICE){
        list_move_tail(&myrr->run_list, &rq->myrr.queue);
        myrr->update_num = 0;
        resched_curr(rq);
        printk("***[MYRR] after rescheduled\n");
    }
}

static void enqueue_task_myrr(struct rq *rq, struct task_struct *p, int flags) {
    struct myrr_rq *myrr_rq = &rq->myrr;
    struct sched_myrr_entity *myrr = &p->myrr;
    
    list_add_tail(&myrr->run_list, &myrr_rq->queue);
    myrr_rq->nr_running++;
    printk(KERN_INFO "***[MYRR] enqueue: success cpu=%d, nr_running=%u,p->pid=%ld\n", cpu_of(rq), myrr_rq->nr_running, (long)p->pid);
}
static void dequeue_task_myrr(struct rq *rq, struct task_struct *p, int flags)
{
    struct myrr_rq *myrr_rq = &rq->myrr;
    struct sched_myrr_entity *myrr = &p->myrr;
    
    list_del_init(&myrr->run_list);
    myrr_rq->nr_running--;
    printk(KERN_INFO "***[MYRR] dequeue: success cpu=%d, nr_running=%u,p->pid=%ld\n", cpu_of(rq), myrr_rq->nr_running, (long)p->pid);
}
void check_preempt_curr_myrr(struct rq *rq, struct task_struct *p, int flags) {
    printk("***[MYRR] check_preempt_curr_myrr\n");
}
struct task_struct *pick_next_task_myrr(struct rq *rq, struct task_struct *prev)
{
    struct myrr_rq *myrr_rq = NULL;
    struct list_head *p = NULL;
    struct sched_myrr_entity *my = NULL;
    struct task_struct *next_task = NULL;
    if(rq->myrr.nr_running == 0){
        return NULL;
    }
    if (prev->sched_class != &myrr_sched_class) put_prev_task(rq, prev);
    myrr_rq = &rq->myrr;
    p = myrr_rq->queue.next;
    my = container_of(p, struct sched_myrr_entity, run_list);
    next_task = container_of(my, struct task_struct, myrr);
    printk(KERN_INFO " ***[MYRR] pick_next_task: cpu=%d,prev->pid=%ld,next_p->pid=%ld,nr_running=%u\n", cpu_of(rq), (long)prev->pid, (long)next_task->pid, myrr_rq->nr_running);
    
    return next_task;
}
void put_prev_task_myrr(struct rq *rq, struct task_struct *p) {
    printk(KERN_INFO "\t***[MYRR] put_prev_task: do_nothing, p->pid=%d\n",p->pid);
}
int select_task_rq_myrr(struct task_struct *p, int cpu, int sd_flag, int flags){return task_cpu(p);}
void set_curr_task_myrr(struct rq *rq){
    printk(KERN_INFO"***[MYRR] set_curr_task_myrr\n");
}
void task_tick_myrr(struct rq *rq, struct task_struct *p, int queued) {
    update_curr_myrr(rq);
}
void prio_changed_myrr(struct rq *rq, struct task_struct *p, int oldprio) { }
/*This routine is called when a task migrates between classes*/
void switched_to_myrr(struct rq *rq, struct task_struct *p)
{
    resched_curr(rq);
}
