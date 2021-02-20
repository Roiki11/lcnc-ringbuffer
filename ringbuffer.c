/*
 ringbuffer.c
 A ringbuffer realtime component. Reads a ringbuffer in shared memory and publishes it.
 
*/

#include ”rtapi.h”
#include ”rtapi_app.h”		/* rtapi_app_main,exit() */
#include ”common.h”		/* shmem structure, SHMEM_KEY */
#include ”hal.h”
#include ”lwrb/lwrb.h”  //ringbuffer library
#include ”ringbuffer.h” //ringbuffer decls

#define data_blocks 20 //number of data points to keep in buffer.

static int shmem_ring;	// ringbuffer ID

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
    hal_float_t *buf_joint0;
    hal_float_t *buf_joint1;
    hal_float_t *buf_joint2;
    hal_float_t *buf_joint3;
    hal_float_t *buf_joint4;
    hal_float_t *buf_joint5;
    hal_float_t *buf_joint6;
    hal_float_t *buf_joint7;
    hal_float_t *buf_joint8;	
} buffdata_t;

/* other globals */

static int comp_id;		/* component ID */

double joint_data[9] = {0}; // joint data structure. 9 doubles for 9 axes.

/* Size of the ring buffer. size of one joint data block times the number of data points to keep in buffer and 1 extra byte for checks. see lwrb docs for more on this. increase the number of data blocks to keep more of them in memory if undesired overwrites occur. */
static int buffsize = sizeof(joint_data) * data_blocks + 1;

//init ringuffer library

lwrb_t ringbuffer;
uint8_t ringbuffer_data[buffsize];

/***********************************************************************
*                  LOCAL FUNCTION DECLARATIONS                         *
************************************************************************/

    
static int export_pins(int n, *buffdata);
static void buffer_pop(void *arg, long period);

/***********************************************************************
*                           INIT FUNCTIONS                             *
************************************************************************/

int rtapi_app_main(void)
{
    int retval;
    
    comp_id = hal_init(”ringbuffer”);
    if (comp_id < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, ”RINGBUFFER: ERROR: hal_init() failed\n”);
	return -EINVAL;
	hal_exit(comp_id);
    }

    /* allocate and initialize the ring buffer shared memory structure */
    shmem_ring = rtapi_shmem_new(shmem_ring_key, comp_id, buffsize);
    if (shmem_ring < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, ”shmem_ring init: rtapi_shmem_new returned %d\n”,
	    shmem_mem);
	rtapi_exit(comp_id);
	return -1;
    }
    
      /* allocate and initialize the shared memory structure */
    shmem_data = hal_malloc(sizeof(*buffdata));
    if (shmem_data < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,”shmem_data init: hal_malloc returned %d\n”,
	    shmem_data);
	rtapi_exit(comp_id);
	return -1;
    }
    //get ringuffer pointer
    retval = rtapi_shmem_getptr(shmem_ring, (void **) &ringbuffer);
    if (retval < 0) {
    rtapi_print_msg(RTAPI_MSG_ERR,”shmem_ring init: rtapi_shmem_getptr returned %d\n”,
	    retval);
	rtapi_exit(comp_id);
	return -1;
    }
    //initialize ringbuffer library
    retval = lwrb_init(&ringbuffer, ringbuffer_data, sizeof(ringbuffer_data)); //Initialize buffer
    if (retval < 0) {
    rtapi_print_msg(RTAPI_MSG_ERR,”shmem_ring init: lwrb_init returned %d\n”,
	    retval);
	rtapi_exit(comp_id);
	return -1;
    }
    //initialize the HAL pins.
    retval = init_pins(n, &buffdata);
    if ( n =! 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    ”ringbuffer: ERROR: init_pins failed\n”);
	retval = -EINVAL;
	hal_exit(comp_id);
	return -1;
    }
    
    retval = lwrb_is_ready(&buffdata);
    if(retvaL = 0){
        rtapi_print_msg(RTAPI_MSG_ERR, "Ringbuffer not ready\n");
        retval = -EINVAL;
        hal_exit(comp_id);
        return -1;
    }
    
    retval= hal_export_func(buffer_pop, buffer_pop, buffdata_t,1,0,comp_id);
    if(retval >0){
        rtapi_print_msg(RTAPI_MSG_ERR,"HAL export function failed!\n");
        retval = -EINVAL;
        hal_exit(comp_id);
        return -1;
    }
    
   hal_ready(comp_id);

}



void rtapi_app_exit(void)
{
    int retval;
    
    retval = rtapi_shmem_delete(shmem_ring, comp_id);
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

static void buffer_pop(void *arg, long period)
{
/* realtime function to run in servo thread. pop one joint position array from the ring buffer and assign the values to something. Either HAL pins or directly to motion controller if possible. Maybe into the HAL shared memory structure or something. */
extern double joint_data[];
buffdata_t *buffdata;
int len;
//read buffer into local array
lwrb_read(&ringbuffer, joint_data, 72);
//check if were enabled and copy data from local array to hal pins
if(*(buffdata->enable) = 1){

    joint_data[0] = *(buffdata->joint0);
    joint_data[1] = *(buffdata->joint1);
    joint_data[2] = *(buffdata->joint2);
    joint_data[3] = *(buffdata->joint3);
    joint_data[4] = *(buffdata->joint4);
    joint_data[5] = *(buffdata->joint5);
    joint_data[6] = *(buffdata->joint6);
    joint_data[7] = *(buffdata->joint7);
    joint_data[8] = *(buffdata->joint8);
    

}
//if the buffer is full, set the hal bit
if((len=lwrb_get_full(&ringbuffer))<1){
    *(buffdata->buffer_full) = 1;
}

}

/***********************************************************************
*                   LOCAL FUNCTION DEFINITIONS                         *
************************************************************************/

static int export_pins(int n, buffdata_t *buffdata){

    int n, i, retval;

    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->enable), comp_id,
	”ringbuffer.enable”);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’enable’ pin export failed\n”);
	return -EIO;
    }
    
    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->buffer_full), comp_id,
	”ringbuffer.buffer-full”);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buffer_full’ pin export failed\n”);
	return -EIO;
    }

    retval = hal_pin_bit_newf(HAL_IN, &(buffdata->buffer_empty), comp_id,
	”ringbuffer.buffer-empty”);
    if (retval != 0 ) {
	rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buffer_empty’ pin export failed\n”);
	return -EIO;
    }
    
    retval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint0), comp_id, 
    "ringbuffer.joint.0");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint1), comp_id, 
    "ringbuffer.joint.1");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint2), comp_id, 
    "ringbuffer.joint.2");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
      retval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint3), comp_id, 
    "ringbuffer.joint.3");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint4), comp_id, 
    "ringbuffer.joint.4");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint5), comp_id, 
    "ringbuffer.joint.5");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
      retval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint6), comp_id, 
    "ringbuffer.joint.6");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint7), comp_id, 
    "ringbuffer.joint.7");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint1’ pin export failed\n”);
	return -EIO;
    }
    
    etval= hal_pin_float_newf(HAL_OUT, &(buffdata->buff_joint8), comp_id, 
    "ringbuffer.joint.8");
    if(retval=0){
       rtapi_print_msg(RTAPI_MSG_ERR,
	    ”RINGBUFFER: ERROR: ’buff_joint8’ pin export failed\n”);
	return -EIO;
    }
    
    *(buffdata->enable) = 0;
    *(buffdata->buffer_full) = 0;
    *(buffdata->buffer_empty) = 1;
    
    

return 0;
}

