#ifndef CONFIG_SERVER_H
#define CONFIG_SERVER_H

#include "../shared/config.h"
#include "../shared/common.h"
#include "../shared/logger/logger.h"

void   init_config(string);
void   post_init_config();
void   check_config();
void   init_log_profiles();

int    load_args(int argc, char **argv);
void   help_args();

#endif
