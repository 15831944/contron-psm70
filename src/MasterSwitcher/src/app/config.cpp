#include "config.h"

#include "sys/platform.h"
#include <ctype.h>


#define get_config_string( config, name )				\
    if( strcmp( key, #name ) == 0 )				\
    {							\
        strcpy( config->name, value );			\
    }
#define get_config_number( config, name )				\
    if( strcmp( key, #name ) == 0 )				\
    {							\
        sscanf( value, "%i", &config->name );		\
    }

config_t *loadConfig()
{
    config_t *result = (config_t *)malloc(sizeof(config_t));
    if(NULL==result)
    {
        return NULL;
    }
    memset(result, 0, sizeof(config_t));

    FILE *fp;
    char file[] = "../ini/config.ini";
    char s[1024];
    char key[1024];
    char value[1024];
    fp = fopen(file, "r");
    if(NULL==fp)
    {
        return NUMBER_FALSE;
    }
    int i = 0;
    while(!feof(fp))
    {
        *s = 0;

        fscanf( fp, "%100[^\n#]s", s );
        fscanf( fp, "%*[^\n]s" );
        fgetc( fp );			// Skip newline
        if( strchr( s, '=' ) == NULL )
            continue;		// Probably empty?
        sscanf( s, "%[^= \t]s", key );
        for(i = 0; s[i]; i ++ )
        {
            if( s[i] == '=' )
            {
                for( i ++; isspace( (int) s[i] ) && s[i]; i ++ );
                break;
            }
        }
        strcpy( value, &s[i] );
        for( i = strlen( value ) - 1; isspace( (int) value[i] ); i -- )
        {
            value[i] = 0;
        }

        get_config_string( result, LocalEthernet );
        get_config_string( result, LocalGateway );
        get_config_string( result, FloatIP );
        get_config_string( result, FloatGateway );
        get_config_string( result, FloatNetmask );
        get_config_string( result, RemoteIP );

        get_config_number( result, HeartbeatPort );
        get_config_number( result, ReconnectInterval );
    }

    fclose(fp);

    return result;
}
