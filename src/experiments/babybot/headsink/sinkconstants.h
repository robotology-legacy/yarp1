#ifndef __SINK_CONSTANTS__
#define __SINK_CONSTANTS__

const int SINK_INHIBIT_NONE = 0;
const int SINK_INHIBIT_VOR = 1;
const int SINK_INHIBIT_VERGENCE = 2;
const int SINK_INHIBIT_SMOOTH = 4;
const int SINK_INHIBIT_SACCADES = 8;
const int SINK_INHIBIT_NECK = 16;
const int SINK_INHIBIT_SMOOTHPURSUIT = 32;
const int SINK_INHIBIT_ALL = 0xffffffff;

#endif