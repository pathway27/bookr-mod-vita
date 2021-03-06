
// TODO: Move these to fz controls header
#define FZ_CTRL_SELECT 0x000001
#define FZ_CTRL_START 0x000008
#define FZ_CTRL_UP 0x000010
#define FZ_CTRL_RIGHT 0x000020
#define FZ_CTRL_DOWN 0x000040
#define FZ_CTRL_LEFT 0x000080
#define FZ_CTRL_LTRIGGER 0x000100
#define FZ_CTRL_RTRIGGER 0x000200
#define FZ_CTRL_TRIANGLE 0x001000
#define FZ_CTRL_CIRCLE 0x002000
#define FZ_CTRL_CROSS 0x004000
#define FZ_CTRL_SQUARE 0x008000
#define FZ_CTRL_HOME 0x010000
#define FZ_CTRL_HOLD 0x020000
#define FZ_CTRL_NOTE 0x800000

// These are numbers based on when using the analog stick.
#ifdef __vita__
  #define FZ_ANALOG_CENTER 128
  #define FZ_ANALOG_THRESHOLD 50
  #define FZ_ANALOG_SENSITIVITY -10
#elif defined(SWITCH)
  #define FZ_ANALOG_CENTER 1000
  #define FZ_ANALOG_THRESHOLD 20000
  #define FZ_ANALOG_SENSITIVITY 6000
#else
  #define FZ_ANALOG_CENTER 128
  #define FZ_ANALOG_THRESHOLD 50
  #define FZ_ANALOG_SENSITIVITY 1000
#endif

#ifdef __vita__
#define FZ_CTRL_L3 0x000002
#define FZ_CTRL_R3 0x000004
#define FZ_CTRL_INTERCEPTED 0x010000
#define FZ_CTRL_VOLUP 0x100000
#define FZ_CTRL_VOLDOWN 0x200000
#elif defined(PSP)
#define FZ_CTRL_L3
#define FZ_CTRL_R3
#define FZ_CTRL_INTERCEPTED
#define FZ_CTRL_VOLUP
#define FZ_CTRL_VOLDOWN
#endif

#define FZ_REPS_SELECT 1
#define FZ_REPS_START 2
#define FZ_REPS_UP 3
#define FZ_REPS_RIGHT 4
#define FZ_REPS_DOWN 5
#define FZ_REPS_LEFT 6
#define FZ_REPS_LTRIGGER 7
#define FZ_REPS_RTRIGGER 8
#define FZ_REPS_TRIANGLE 9
#define FZ_REPS_CIRCLE 10
#define FZ_REPS_CROSS 11
#define FZ_REPS_SQUARE 12
#define FZ_REPS_HOME 13
#define FZ_REPS_HOLD 14
#define FZ_REPS_NOTE 15
