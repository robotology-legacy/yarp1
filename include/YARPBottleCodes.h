
#ifndef YARPBottleCodes_INC
#define YARPBottleCodes_INC

// Automatically generated file -- do not edit directly.
// Generated in directory /home/mb/cdp/src/YARP/conf/vocab


enum {
  YBC_NULL,
  YBC_GAZE,
  YBC_GAZE_FIXATE,
  YBC_VOICE,
  YBC_VOICE_HEARD,
  YBC_VOICE_HEARING,
  YBC_TICK,
  YBC_TICK_SECOND,
  YBC_ASSESSMENT,
  YBC_POSITIVE,
  YBC_NEGATIVE,
  YBC_GAZE_FREEZE,
  YBC_GAZE_UNFREEZE,
  YBC_MOTOR,
  YBC_HEAD,
  YBC_STABLE,
  YBC_OBJECT,
  YBC_FOUND,
  YBC_ABOUT_TO_POKE,
  YBC_REACHING_STARTED,
  YBC_REACHING_ENDED,
  YBC_POKING_STARTED,
  YBC_POKING_ENDED,
  YBC_PRECISE_FIXATION,
  YBC_EGOMAP,
  YBC_TARGET_SET,
  YBC_TARGET_REVERT,
  YBC_TARGET_APPEAR,
  YBC_TARGET_RESET,
  YBC_TARGET_ORIENT,
  YBC_SHLC_NONE,
  YBC_SHLC_START_REACHING,
  YBC_SHLC_STOP_REACHING,
  YBC_SHLC_START_PUSHING,
  YBC_SHLC_STOP_PUSHING,
  YBC_SHLC_ASK_TORSO,
  YBC_SHLC_REPLY_TORSO,
  YBC_STABILIZE,
  YBC_SHLC_COMPUTE_BEST_ACTION,
  YBC_SHLC_COMPUTE_BEST_ACTION_REPLY,
  YBC_IMAGE_MOTION,
  YBC_SHLC_START_RECORDING,
  YBC_SHLC_STOP_RECORDING,
  YBC_REACH_RANGE,
  YBC_REACH_OUT_OF_RANGE,
  YBC_LOG_PERCEPT,
  YBC_PERCEPT_TEST0,
  YBC_PERCEPT_TEST1,
  YBC_PERCEPT_TEST2,
  YBC_PERCEPT_TEST3,
  YBC_PERCEPT_REACHING_STARTED,
  YBC_PERCEPT_REACHING_STOPPED,
  YBC_PERCEPT_RECORDING_STARTED,
  YBC_PERCEPT_RECORDING_STOPPED,
  YBC_PERCEPT_PUSHING_STARTED,
  YBC_PERCEPT_PUSHING_STOPPED,
  YBC_PERCEPT_REQUEST_ACTION,
  YBC_PERCEPT_COMPUTED_ACTION,
  YBC_PERCEPT_PROCESSING_STARTED,
  YBC_PERCEPT_PROCESSING_FINISHED,
  YBC_PERCEPT_OBSERVING_STARTED,
  YBC_PERCEPT_OBSERVING_FINISHED,
  YBC_PERCEPT_PROCESSING_RESULT,
  YBC_PERCEPT_IMAGE_POSITIONS,
  YBC_PERCEPT_IMAGE_POSE_SHAPE,
  YBC_PERCEPT_OBJECT_RECOGNIZED_AS,
  YBC_PERCEPT_ACTION_INTERPRETED_AS,
  YBC_PERCEPT_PUSHING_VECTOR,
  YBC_PERCEPT_ISSUED_ACTION,
  YBC_PERCEPT_BEST_MATCH_NEURON,
  YBC_PERCEPT_GOOD_OBJECT_MATCH,
  YBC_PERCEPT_ADDING_NEURON,
  YBC_PERCEPT_MIMICRY_MODE,
  YBC_PERCEPT_LAST_OBJECT_OBSERVED,
  YBC_PERCEPT_IMAGE_POSE_DIRECTION,
  YBC_PERCEPT_CURRENT_OBJECT_OBSERVED,
  YBC_PERCEPT_OBJECT_AFFORDANCE,
  YBC_PERCEPT_AFFORDANCE_MODE,
};

template <class T>
const char *ybc_label(T x) {
  switch (x) {
    case YBC_NULL: return "null";
    case YBC_GAZE: return "gaze";
    case YBC_GAZE_FIXATE: return "gaze_fixate";
    case YBC_VOICE: return "voice";
    case YBC_VOICE_HEARD: return "voice_heard";
    case YBC_VOICE_HEARING: return "voice_hearing";
    case YBC_TICK: return "tick";
    case YBC_TICK_SECOND: return "tick_second";
    case YBC_ASSESSMENT: return "assessment";
    case YBC_POSITIVE: return "positive";
    case YBC_NEGATIVE: return "negative";
    case YBC_GAZE_FREEZE: return "gaze_freeze";
    case YBC_GAZE_UNFREEZE: return "gaze_unfreeze";
    case YBC_MOTOR: return "motor";
    case YBC_HEAD: return "head";
    case YBC_STABLE: return "stable";
    case YBC_OBJECT: return "object";
    case YBC_FOUND: return "found";
    case YBC_ABOUT_TO_POKE: return "about_to_poke";
    case YBC_REACHING_STARTED: return "reaching_started";
    case YBC_REACHING_ENDED: return "reaching_ended";
    case YBC_POKING_STARTED: return "poking_started";
    case YBC_POKING_ENDED: return "poking_ended";
    case YBC_PRECISE_FIXATION: return "precise_fixation";
    case YBC_EGOMAP: return "egomap";
    case YBC_TARGET_SET: return "target_set";
    case YBC_TARGET_REVERT: return "target_revert";
    case YBC_TARGET_APPEAR: return "target_appear";
    case YBC_TARGET_RESET: return "target_reset";
    case YBC_TARGET_ORIENT: return "target_orient";
    case YBC_SHLC_NONE: return "shlc_none";
    case YBC_SHLC_START_REACHING: return "shlc_start_reaching";
    case YBC_SHLC_STOP_REACHING: return "shlc_stop_reaching";
    case YBC_SHLC_START_PUSHING: return "shlc_start_pushing";
    case YBC_SHLC_STOP_PUSHING: return "shlc_stop_pushing";
    case YBC_SHLC_ASK_TORSO: return "shlc_ask_torso";
    case YBC_SHLC_REPLY_TORSO: return "shlc_reply_torso";
    case YBC_STABILIZE: return "stabilize";
    case YBC_SHLC_COMPUTE_BEST_ACTION: return "shlc_compute_best_action";
    case YBC_SHLC_COMPUTE_BEST_ACTION_REPLY: return "shlc_compute_best_action_reply";
    case YBC_IMAGE_MOTION: return "image_motion";
    case YBC_SHLC_START_RECORDING: return "shlc_start_recording";
    case YBC_SHLC_STOP_RECORDING: return "shlc_stop_recording";
    case YBC_REACH_RANGE: return "reach_range";
    case YBC_REACH_OUT_OF_RANGE: return "reach_out_of_range";
    case YBC_LOG_PERCEPT: return "log_percept";
    case YBC_PERCEPT_TEST0: return "percept_test0";
    case YBC_PERCEPT_TEST1: return "percept_test1";
    case YBC_PERCEPT_TEST2: return "percept_test2";
    case YBC_PERCEPT_TEST3: return "percept_test3";
    case YBC_PERCEPT_REACHING_STARTED: return "percept_reaching_started";
    case YBC_PERCEPT_REACHING_STOPPED: return "percept_reaching_stopped";
    case YBC_PERCEPT_RECORDING_STARTED: return "percept_recording_started";
    case YBC_PERCEPT_RECORDING_STOPPED: return "percept_recording_stopped";
    case YBC_PERCEPT_PUSHING_STARTED: return "percept_pushing_started";
    case YBC_PERCEPT_PUSHING_STOPPED: return "percept_pushing_stopped";
    case YBC_PERCEPT_REQUEST_ACTION: return "percept_request_action";
    case YBC_PERCEPT_COMPUTED_ACTION: return "percept_computed_action";
    case YBC_PERCEPT_PROCESSING_STARTED: return "percept_processing_started";
    case YBC_PERCEPT_PROCESSING_FINISHED: return "percept_processing_finished";
    case YBC_PERCEPT_OBSERVING_STARTED: return "percept_observing_started";
    case YBC_PERCEPT_OBSERVING_FINISHED: return "percept_observing_finished";
    case YBC_PERCEPT_PROCESSING_RESULT: return "percept_processing_result";
    case YBC_PERCEPT_IMAGE_POSITIONS: return "percept_image_positions";
    case YBC_PERCEPT_IMAGE_POSE_SHAPE: return "percept_image_pose_shape";
    case YBC_PERCEPT_OBJECT_RECOGNIZED_AS: return "percept_object_recognized_as";
    case YBC_PERCEPT_ACTION_INTERPRETED_AS: return "percept_action_interpreted_as";
    case YBC_PERCEPT_PUSHING_VECTOR: return "percept_pushing_vector";
    case YBC_PERCEPT_ISSUED_ACTION: return "percept_issued_action";
    case YBC_PERCEPT_BEST_MATCH_NEURON: return "percept_best_match_neuron";
    case YBC_PERCEPT_GOOD_OBJECT_MATCH: return "percept_good_object_match";
    case YBC_PERCEPT_ADDING_NEURON: return "percept_adding_neuron";
    case YBC_PERCEPT_MIMICRY_MODE: return "percept_mimicry_mode";
    case YBC_PERCEPT_LAST_OBJECT_OBSERVED: return "percept_last_object_observed";
    case YBC_PERCEPT_IMAGE_POSE_DIRECTION: return "percept_image_pose_direction";
    case YBC_PERCEPT_CURRENT_OBJECT_OBSERVED: return "percept_current_object_observed";
    case YBC_PERCEPT_OBJECT_AFFORDANCE: return "percept_object_affordance";
    case YBC_PERCEPT_AFFORDANCE_MODE: return "percept_affordance_mode";
  }
  return "UNKNOWN";
};


#endif
