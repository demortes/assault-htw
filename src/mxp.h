/* strings */

#define MXP_BEG "<"                                         /* becomes < */
#define MXP_END ">"                                         /* becomes > */
#define MXP_AMP "&"                                         /* becomes & */

/* characters */

#define MXP_BEGc '<'                                        /* becomes < */
#define MXP_ENDc '>'                                        /* becomes > */
#define MXP_AMPc '&'                                        /* becomes & */

/* constructs an MXP tag with < and > around it */

//#define MXPTAG(arg) MXP_BEG arg MXP_END
#define MXPTAG(d,arg) ((d && d->mxp) ? MXP_BEG arg MXP_END : "")

#define ESC "\x1B"                                          /* esc character */

#define MXPMODE(arg) ESC "[" #arg "z"
