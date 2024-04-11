
/**
 * @file utils.h
 * @author Laurent Bonnet (laurent.woolcap@free.fr)
 * @brief Chip evaluation firmware, utils interface file
 * @copyright Copyright (c) 2023
 */
#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

extern int trigger_init(void);
extern int trigger_low(void);
extern int trigger_high(void);
extern int sub_trigger_low(void);
extern int sub_trigger_high(void);
#ifdef __cplusplus
}
#endif

#endif // __UTILS_H