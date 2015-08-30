#include "config.h"

//int ground_config_loadfile(config_t *config, char *file)
//{
//    if(!config || !file)
//    {
//        return NUMBER_ERROR;
//    }
//    FILE *fp;

//    fp = fopen( file, "r" );
//    if( fp == NULL )
//        return NUMBER_ERROR;			// Not a real failure	

//    return NUMBER_SUCCESS;
//}


//int ground_config_init(config_t *config)
//{
//    char s[MAX_STRING];
//    sprintf(s, "%s", "ground.config");
//    int ret = ground_config_loadfile(config, s);
//    printf("ground_config_loadfile = %d\n", ret);
//    return NUMBER_NOERROR;
//}


//int ground_config_savefile(config_t *config, char *file)
//{
//    return NUMBER_NOERROR;
//}
