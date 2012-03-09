#ifndef CONFIG_CLIENT_H
#define CONFIG_CLIENT_H

#include "../shared/utility/common.h"

void   init_config(string);
void   post_init_config();
void   check_config();
void   init_log_profiles();

int    load_args(int argc, char **argv);
void   help_args();

#endif
