#include "tinyOS.h"
void tTaskInit (tTask * task, void (*entry)(void *), void * param, uint32_t prio, tTaskStack * stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)(entry);
	*(--stack) = (unsigned long)0x14; //R14
	*(--stack) = (unsigned long)0x12; //R12
	*(--stack) = (unsigned long)0x3; //R3
	*(--stack) = (unsigned long)0x2; //R2
	*(--stack) = (unsigned long)0x1; //R1
	*(--stack) = (unsigned long)param; //R0
	
	//above: automatic
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x9;
	*(--stack) = (unsigned long)0x8;
	*(--stack) = (unsigned long)0x7;
	*(--stack) = (unsigned long)0x6;
	*(--stack) = (unsigned long)0x5;
	*(--stack) = (unsigned long)0x4;
	
	task->slice = TINYOS_SLICE_MAX;
	task->stack = stack; //https://blog.csdn.net/faihung/article/details/79190039
	task->delayTicks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	
	tNodeInit(&(task->delayNode));
	tNodeInit(&(task->linkNode));
	
	tTaskSchedRdy(task);
}
void tTaskSuppend (tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(!(task->state & TINYOS_TASK_STATE_SUSPEND))
	{
		if(++task->suspendCount <= 1)
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedUnRdy(task);
			if (task == currentTask)
			{
				tTaskSched();
			}
		}
	}
	tTaskExitCritical(status);
}

void tTaskWakeUp (tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	if (task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		if (--task->suspendCount == 0)
		{
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
}
