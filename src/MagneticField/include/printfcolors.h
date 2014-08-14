#ifndef _PRINTFCOLORS_H_
#define _PRINTFCOLORS_H_

// funny colors: http://www.faqs.org/docs/abs/HTML/colorizing.html
//           or: http://linuxgazette.net/issue65/padala.html
//
//#define PFC_RED       "\033[31m"
//#define PFC_GREEN     "\033[32m"
//#define PFC_BLUE      "\033[34m"
//#define PFC_BOLD      "\033[1m"
//#define PFC_BOLDRED   "\033[31m\033[1m"
//#define PFC_BOLDGREEN "\033[32m\033[1m"
//#define PFC_BOLDBLUE  "\033[34m\033[1m"
//#define PFC_BACKRED   "\033[31m\033[7m"
//#define PFC_BACKGREEN "\033[32m\033[7m"
//#define PFC_BACKBLUE  "\033[34m\033[7m"
//#define PFC_UNDERLINE "\033[4m"
//#define PFC_BLINKING  "\033[5m"
//
// As this will only work in a terminal and not in a GUI we
// define everything first of all only for the GUI case...
//
#define NORMAL    ""
#define DEFAULT   ""

#define CONFIG_MSG  "  (C)   "
#define STATUS_MSG  "  (S)   "
#define WARNING_MSG "--(W)-- "
#define ERROR_MSG   "**(E)** "
//
// ... and then once again for the terminal case:
//     (i.e. none of the GUI flags is set)
//
#ifndef G4UI_USE_QT
#ifndef G4UI_USE_XM
#ifndef G4UI_USE_XAW
#undef  NORMAL
#undef  DEFAULT
#undef  CONFIG_MSG
#undef  STATUS_MSG
#undef  WARNING_MSG
#undef  ERROR_MSG
#define NORMAL    "\033[0m"
#define DEFAULT   "\033[0m\033[49m\033[39m"
#define CONFIG_MSG  "\033[34m  (C)   "
#define STATUS_MSG  "\033[1m  (S)   "
#define WARNING_MSG "\033[31m--(W)-- "
#define ERROR_MSG   "\033[31m\033[1m**(E)** "
#endif //G4UI_USE_XAW
#endif //G4UI_USE_XM
#endif //G4UI_USE_QT

#endif
