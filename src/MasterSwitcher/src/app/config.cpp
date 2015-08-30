#include "config.h"

#include "sys/platform.h"

#define _STR( s ) #s
#define STR( s ) _STR(s)
#define get_config_string( name )				\
    if( strcmp( key, #name ) == 0 )				\
    {							\
        strcpy( self.name, value );			\
    }
#define get_config_number( name )				\
    if( strcmp( key, #name ) == 0 )				\
    {							\
        sscanf( value, "%i", &self.name );		\
    }

static bool init = false;
config_t self;

void config_init()
{
    memset(&self, 0, sizeof(config_t));

//    self.rt_id = 5678;
//    strcpy(self.wf_server_ip, "127.0.0.1");
//    self.wf_server_port = 5555;
//    self.client_enable = 0;
//    self.client_count = 0;
//    strcpy(self.default_client_server_ip, "127.0.0.1");
//    self.default_client_server_port = 9803;
//    self.default_client_server_id = 1;
//    self.server_enable = 0;
//    self.server_port = 9801;

}

int config_load()
{
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

        get_config_string( FloatIP );
        get_config_string( FloatGateway );
        get_config_string( RemoteIP );

        get_config_number( HeartbeatPort );
        get_config_number( ReconnectInterval );
    }

    fclose(fp);

    return NUMBER_TRUE;
}

config_t *config_instance()
{
    if(!init)
    {
        config_init();
        config_load();
        init = true;
    }

    return &self;
}
