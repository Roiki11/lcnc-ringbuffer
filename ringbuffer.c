/*
 ringbuffer.c
 A ringbuffer realtime component. Reads a ringbuffer in shared memory and publishes it.
 
*/

#include ”rtapi.h”
#include ”rtapi_app.h”		/* rtapi_app_main,exit() */
#include ”common.h”		/* shmem structure, SHMEM_KEY */
#include ”hal.h”
#include ”lwrb/lwrb.h”  //ringbuffer library

static int module;  
static int shmem_ring;		/* the shared memory ID */

static int SHMEM_RING_STACKSIZE = 9*8*20+1 ;	/* how big the ringbuffer is */

static buffdata_t *buffdata = 0

/* module information */
MODULE_AUTHOR(”Roiki11”);
MODULE_DESCRIPTION(”HAL ringbuffer”);
MODULE_LICENSE(”GPL3”);

/***********************************************************************
*                STRUCTURES AND GLOBAL VARIABLES                       *
************************************************************************/

/* this structure contains the HAL shared memory data */

typedef struct {
    hal_bit_t *buffer_full;
    hal_bit_t *enable;
    hal_bit_t *buffer_empty;	
} buffdata_t;

/* other globals */

static int comp_id;		/* component ID */
static int module;
static int export_pins(int n, *buffdata);


int rtapi_app_main(void)
{
    int retval;
    lwrb_t ringbuffer;
    uint8_t ringbuffer_data[SHMEM_RING_STACKSIZE];
    
    comp_id = hal_init(”ringbuffer”);
    if (comp_id < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, ”RINGBUFFER: ERROR: hal_init() failed\n”);
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
    shmem_data = hal_malloc(sizeof(*buffdata));
    if (shmem_data < 0) {
	rtapi_print(”shmem_ring init: hal_malloc returned %d\n”,
	    shmem_data);
	rtapi_exit(comp_id);
	return -1;
    }
    
    retval = rtapi_shmem_getptr(shmem_ring, (void **) &ringbuffer);
    if (retval < 0) {
    rtapi_print(”shmem_ring init: rtapi_shmem_getptr returned %d\n”,
	    retval);
	rtapi_exit(module);
	return -1;
    }
    
    retval = lwrb_init(&ringbuffer, ringbuffer_data, sizeof(ringbuffer_data)); /* Initialize buffer */
    if (retval < 0) {
    rtapi_print(”shmem_ring init: lwrb_init returned %d\n”,
	    retval);
	rtapi_exit(module);
	return -1;
    }
    
    retval = init_pins(n, &buffdata);
    if ( n == 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    "ringbuffer: ERROR: init_pins failed\n");
	retval = -EINVAL;
	hal_exit(comp_id);
	return -1;
    }
    
   hal_ready(comp_id);

}



void rtapi_app_exit(void)
{
    int retval;
    
    retval = rtapi_shmem_delete(shmem_ring, module);
    if (retval < 0) {
	rtapi_print(”shmemtask exit: rtapi_shmem_delete returned %d\n”,
	    retval);
    }
    
    /* Clean up and exit */
    hal_exit(comp_id);
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

static int export_pins(int n, buffdata_t *buffdata){

    int n, retval;

    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->enable), comp_id,
	"ringbuffer.%d.enable", num);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    "RINGBUFFER: ERROR: 'enable' pin export failed\n");
	return -EIO;
    }
    
    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->buffer_full), comp_id,
	"ringbuffer.%d.buffer_full", num);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    "RINGBUFFER: ERROR: 'buffer_full' pin export failed\n");
	return -EIO;
    }

    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->buffer_empty), comp_id,
	"ringbuffer.%d.buffer_empty", num);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    "RINGBUFFER: ERROR: 'buffer_empty' pin export failed\n");
	return -EIO;
    }
    
    *(buffdata->enable) = 0;
    *(buffdata->buffer_full) = 0;
    *(buffdata->buffer_empty) = 1;
    
    

return 0;
}

