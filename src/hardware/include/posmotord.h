


#ifndef _MOTORD_H_
#define _MOTORD_H_

#include <sys/types.h>

#ifndef _MEID_ERR_CODES_
#define _MEID_ERR_CODES_

/* meid-specific error codes (others inherited from MEI lib) */
#define MEID_ERR_SYS   -1
#define MEID_ERR_NOMEM -2
#define MEID_ERR_INVAL -3

#endif /* _MEID_ERR_CODES_ */


#ifndef _MEID_NO_PROTOTYPES_  /* we do same thing!  see below... */

/* we want all the constants and keywords from the MEI lib header file,
    but we don't want any function prototypes ('cause we don't call any
    of those functions!)
   conventiently, those MEI guys did put a switch in there for us.
   (if you do happen to want the prototypes (daemon code), make sure
    that <mei/pcdsp.h> precedes any instance of "meid.h".)
*/
#define NO_PROTOTYPES
#include <mei/pcdsp.h>
#undef NO_PROTOTYPES


#define TROL 0
#define TPIT 1
#define TYAW 2
#define LSHA 3
#define LSHB 4
#define LELA 5
#define LELB 6
#define LWRA 7
#define LWRB 8
#define LTHU 9
#define LPAW 10
#define RSHA 11
#define RSHB 12
#define RELA 13
#define RELB 14
#define RWRA 15
#define RWRB 16
#define RTHU 17
#define RPAW 18
#define NJOINTS 19

// filter vals
#define P  0
#define I  1
#define D  2
#define SH 3

/* initialization */
int motord_initialize(char *motord_name, int *motordx);

/* message-passing wrappers */
int motord_init(int motordx);
int motord_gains_up(int motordx, char * filename);
int motord_gains_down(int motordx);
int motord_set_gain(int motordx, int16 axis, int param, int value);
int motord_get_gain(int motordx, int16 axis, int param, int * value);
int motord_set_position(int motordx, int16 axis, double pos);
int motord_set_positions(int motordx, int count, int16 * axes, double * pos);
int motord_get_position(int motordx, int16 axis, int * pos);
int motord_get_angle(int motordx, int16 axis, int * angle);
int motord_get_angles(int motordx, int *angles);
int motord_torso_calibrate(int motordx);
int motord_torso_set_ctrl(int motordx, int do_force);


#endif /* _MEID_NO_PROTOTYPES_ */

#endif /* _MOTORD_H_ */





