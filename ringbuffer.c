/*
 ringbuffer.c
 A ringbuffer realtime component. Reads a ringbuffer in shared memory and publishes it.
 
*/

#include "rtapi.h"
#include "rtapi_app.h"		/* rtapi_app_main,exit() */
#include "common.h"		/* shmem structure, SHMEM_KEY */
#include "hal.h"

static int module;  
static int shmem_ring;		/* the shared memory ID */
static int shmem_data;      /* shared memory data ID */

#define shmem_ring_key 1011124 /* ringbuffer shared key */
#define shmem_data_key 1248234 /* ringuffer data shared key */

enum { SHMEM_RING_STACKSIZE = 9*8*20+1 };	/* how big the ringbuffer is */

static int key = SHMEM_KEY;
static SHMEM_STRUCT *shmem_struct = 0;

/* module information */
MODULE_AUTHOR("Roiki11”);
MODULE_DESCRIPTION("HAL ringbuffer");
MODULE_LICENSE("GPL3");

/***********************************************************************
*                STRUCTURES AND GLOBAL VARIABLES                       *
************************************************************************/

/* this structure contains the HAL shared memory data */

typedef struct {
    hal_bit_t *buff_full;
    hal_bit_t *enable;	
} buffdata_t;

/* other globals */

static int comp_id;		/* component ID */



int rtapi_app_main(void)
{
    int retval;
    long period;
    
    comp_id = hal_init("ringbuffer");
    if (comp_id < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, "RINGBUFFER: ERROR: hal_init() failed\n");
	return -EINVAL;
    }

    /* allocate and initialize the shared memory structure */
    shmem_ring = rtapi_shmem_new(shmem_ring_key, module, sizeof(SHMEM_RING_STACKSIZE));
    if (shmem_ring < 0) {
	rtapi_print(”shmem_ring init: rtapi_shmem_new returned %d\n”,
	    shmem_mem);
	rtapi_exit(module);
	return -1;
    }
    
      /* allocate and initialize the shared memory structure */
    shmem_data = rtapi_shmem_new(key, module, sizeof(SHMEM_STRUCT));
    if (shmem_data < 0) {
	rtapi_print(”shmem_ring init: rtapi_shmem_new returned %d\n”,
	    shmem_mem);
	rtapi_exit(module);
	return -1;
    }
    retval = rtapi_shmem_getptr(shmem_mem, (void **) &shmem_struct);
    if (retval < 0) {
	rtapi_print(”shmemtask init: rtapi_shmem_getptr returned %d\n”,
	    retval);
	rtapi_exit(module);
	return -1;
    }
    
    hal_ready(comp_id);
    return 0;
fail:
    
    hal_exit(comp_id);
    return retval;
}



void rtapi_app_exit(void)
{
    int retval;
    retval = rtapi_shmem_delete(shmem_mem, module);
    if (retval < 0) {
	rtapi_print(”shmemtask exit: rtapi_shmem_delete returned %d\n”,
	    retval);
    }
    
    /* Clean up and exit */
    rtapi_exit(module);
}

/***********************************************************************
*                       REALTIME FUNCTIONS                             *
************************************************************************/

static void buffer_run(void *arg, long l)
{

}

/***********************************************************************
*                   LOCAL FUNCTION DEFINITIONS                         *
************************************************************************/
