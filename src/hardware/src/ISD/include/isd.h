#ifndef _ISD_H_
#define _ISD_H_

#ifdef __cplusplus
extern "C" {
#endif
    
/* initialization */
int isd_initialize(char *isd_name, int *isdx);

/* message-passing wrappers */
int isd_poll_all(int isdx, float *yaw, float *pitch, float *roll);
int isd_boresight(int isdx);
int isd_unboresight(int isdx);

int isd_set_output_mode(int isdx, int is_polled, int is_ascii);
int isd_get_output_mode(int isdx, int *is_polled, int *is_ascii);

// default is POLLED, ASCII
// NON-POLLED (CONTINUOUS) is not yet supported
#ifdef __cplusplus
}
#endif

#endif /* _ISD_H_ */

