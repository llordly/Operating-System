#include "sched.h"
void init_mysched_rq(struct mysched_rq *mysched_rq)
{
	printk(KERN_INFO "***[MYSCHED] Myschedclass is online \n");
	INIT_LIST_HEAD(&mysched_rq->queue);
}

static void update_curr_mysched(struct rq *rq)
{
}
static void enqueue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    struct mysched_rq *mysched_rq = &rq->mysched;
    struct sched_mysched_entity *mysched = &p->mysched;
    
    list_add_tail(&mysched->run_list, &mysched_rq->queue);
    mysched_rq->nr_running++;
    
    printk(KERN_INFO "***[MYSCHED] enqueue: success cpu=%d, nr_running=%u,p->state=%ld,p->pid=%ld\n", p->on_cpu, mysched_rq->nr_running, p->state, (long)p->pid);
}

static void dequeue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    struct mysched_rq *mysched_rq = &rq->mysched;
    struct sched_mysched_entity *mysched = &p->mysched;

    printk(KERN_INFO "***[MYSCHED] dequeue: start\n");
    printk(KERN_INFO " ***[MYSCHED] dequeue: the dequeued task is curr, set TIF_NEED_RESCHED flag cpu=%d,p->state=%ld,p->pid=%ld,curr->pid=%ld\n", p->on_cpu, p->state, (long)p->pid, (long)p->pid);
    list_del_init(&mysched->run_list);
    mysched_rq->nr_running--;

    printk(KERN_INFO "***[MYSCHED] dequeue: end\n");
}
static void check_preempt_curr_mysched(struct rq *rq, struct task_struct *p, int flags) 
{ 
}
struct task_struct *pick_next_task_mysched(struct rq *rq, struct task_struct *prev)
{
    struct mysched_rq *mysched_rq = &rq->mysched;
    struct list_head *p = NULL;
    struct sched_mysched_entity *my = NULL;
    struct task_struct *next_task = NULL;
    if (mysched_rq->nr_running == 0) return NULL;
    if (prev->sched_class != &mysched_sched_class) put_prev_task(rq, prev);
    p = mysched_rq->queue.next;
    my = container_of(p, struct sched_mysched_entity, run_list);
    next_task = container_of(my, struct task_struct, mysched);
    printk(KERN_INFO " ***[MYSCHED] pick_next_task: cpu=%d,prev->pid=%ld,next_p->pid=%ld,nr_running=%u\n", prev->on_cpu, (long)prev->pid, (long)next_task->pid, mysched_rq->nr_running);
    printk(KERN_INFO "***[MYSCHED] pick_next_task: end\n");
    return next_task;
}
static void put_prev_task_mysched(struct rq *rq, struct task_struct *p) 
{
    printk(KERN_INFO " ***[MYSCHED] put_prev_task: do nothing, p->pid=%ld\n", (long)p->pid);
}
static int select_task_rq_mysched(struct task_struct *p, int cpu, int sd_flag, int flags)
{
	return task_cpu(p);
}
static void set_curr_task_mysched(struct rq *rq) 
{ 
}
static void task_tick_mysched(struct rq *rq, struct task_struct *p, int queued) 
{ 
}
static void prio_changed_mysched(struct rq *rq, struct task_struct *p, int oldprio) 
{ 
}
/* This routine is called when a task migrates between classes */
static void switched_to_mysched(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}
const struct sched_class mysched_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_mysched,
	.dequeue_task		= dequeue_task_mysched,
	.check_preempt_curr	= check_preempt_curr_mysched,
	.pick_next_task		= pick_next_task_mysched,
	.put_prev_task		= put_prev_task_mysched,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_mysched,
#endif
	.set_curr_task		= set_curr_task_mysched,
	.task_tick		= task_tick_mysched,
	.prio_changed		= prio_changed_mysched,
	.switched_to		= switched_to_mysched,
	.update_curr		= update_curr_mysched,
};
