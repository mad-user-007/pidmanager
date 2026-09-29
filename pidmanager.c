#include "pidmanager.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN_PID 300;
#define MAX_PID 5000;

/* Structure to store and manage pids */
struct PidManager {
	unsigned int* pids;
	unsigned int required_pids;
	unsigned char no_of_blocks;
	unsigned char pid_per_block;
};

static struct PidManager p_man;

int allocate_map(){
	p_man.required_pids = MAX_PID - MIN_PID;

	/* Initialize Pointer */
	unsigned char required_ints = p_man.required_pids / (8*sizeof(int)) + 1;
	
	p_man.pids = (unsigned int*)calloc(required_ints, sizeof(int));
	if(p_man.pids == NULL) return -1;
	
	p_man.no_of_blocks = required_ints;
	p_man.pid_per_block = sizeof(int) * 8;
	return 1;
}

int allocate_pid(){
	unsigned int* temp;
	
	/* Traverse and see which pid is free */
	int block = 0;
	int i;
	for(i=0; i<p_man.no_of_blocks; i++){ /* Each pid block */
		temp = (p_man.pids + i);
		unsigned char pid_offset;
		
		/* Each pid in each block */
		for(pid_offset = 0; pid_offset < p_man.pid_per_block && ((i<<3)*sizeof(int) + pid_offset) <= p_man.required_pids; pid_offset++){
			if(*temp & (1 << pid_offset)) continue;

			/* Set available pid bit */
			*temp = *temp | (1 << pid_offset);
			return ((i<<3)*sizeof(int) + pid_offset + 1);
		}
	}

	return -1;
}	

void release_pid(int pid){
	unsigned char block = pid / (sizeof(int)<<3);
	unsigned char pid_offset = (pid % p_man.pid_per_block);

	/* Unset the pid bit */
	unsigned int *pid_block = p_man.pids + block;

	*pid_block = *pid_block ^ (1<<pid_offset); 
}
