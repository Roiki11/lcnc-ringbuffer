/*
  shmemtask.c
  Set up a periodic task that increments a heartbeat in shared memory.
*/

#include "rtapi.h"
#include "rtapi_app.h"		/* rtapi_app_main,exit() */
#include "common.h"		/* shmem structure, SHMEM_KEY */

static int module;
static int shmem_task;		/* the task ID */
static int shmem_mem;		/* the shared memory ID */
enum { TIMER_PERIOD_NSEC = 1000000 };	/* timer period, in nanoseconds */
enum { SHMEM_PERIOD_NSEC = 1000000 };	/* task period, in nanoseconds */
enum { SHMEM_STACKSIZE = 1024 };	/* how big the stack is */

static int key = SHMEM_KEY;
static SHMEM_STRUCT *shmem_struct = 0;

/* task code, executed periodically */
void shmem_code(void *arg)
{
   

    return;
}

/* part of the Linux kernel module that kicks off the shmem task */
int rtapi_app_main(void)
{
    int retval;
    int shmem_prio;
    long period;

    module = rtapi_init("SHMEMTASK");
    if (module < 0) {
	rtapi_print("shmemtask init: rtapi_init returned %d\n", module);
	return -1;
    }
    /* allocate and initialize the shared memory structure */
    shmem_mem = rtapi_shmem_new(key, module, sizeof(SHMEM_STRUCT));
    if (shmem_mem < 0) {
	rtapi_print("shmemtask init: rtapi_shmem_new returned %d\n",
	    shmem_mem);
	rtapi_exit(module);
	return -1;
    }
    retval = rtapi_shmem_getptr(shmem_mem, (void **) &shmem_struct);
    if (retval < 0) {
	rtapi_print("shmemtask init: rtapi_shmem_getptr returned %d\n",
	    retval);
	rtapi_exit(module);
	return -1;
    }


 

    

}

void rtapi_app_exit(void)
{
    int retval;
    retval = rtapi_shmem_delete(shmem_mem, module);
    if (retval < 0) {
	rtapi_print("shmemtask exit: rtapi_shmem_delete returned %d\n",
	    retval);
    }
    
    /* Clean up and exit */
    rtapi_exit(module);
}