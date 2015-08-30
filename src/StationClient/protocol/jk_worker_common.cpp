#include "jk_worker_common.h"

int jk_command_check(jk_command_t *command, UCHAR cmd, UCHAR fnn)
{
    int result = 1;
    result &= (2<=command->data_size);
    result &= (cmd==((UCHAR)(*command->data)&0x7F));
    result &= (fnn==(UCHAR)(*(command->data+1)));
    return result;
}

int jk_command_check_cmd(jk_command_t *command, UCHAR cmd)
{
    int result = 1;
    UCHAR c = (UCHAR)(*command->data)&0x7F;
    result &= (2<=command->data_size);
    result &= (cmd==c);
    return result;
}

int jk_command_check_fnn(jk_command_t *command, UCHAR fnn)
{
    int result = 1;
    result &= (2<=command->data_size);
    result &= (fnn==(UCHAR)(*(command->data+1)));
    return result;
}

int jk_command_check_back(jk_command_t *command, UCHAR cmd)
{
    int result = 1;
    result &= (2<=command->data_size);
    result &= ((cmd|PRL_CT_BACK)==(UCHAR)(*command->data));
    return result;
}


int jk_client_command_valid(stationclient_t *client, jk_command_t *command)
{
    int result = 1;
    result &= (command->target_address==client->station_no);
    result &= (command->source_address==client->server_no);
    return result;
}


/*!
 * \brief jk_command_check_data_struct  检查指令中数据结构
 * \param command  指令
 * \return 指令中数据结构使用设备主编号返回1，否则返回0
 * 本规约数据结构数值，参见“1.2协议约定 C.”
 * 本规约数据结构使用设备主编号，不使用设备对照表，参见“1.2协议约定 A.”
 */
int jk_command_check_data_struct(jk_command_t *command)
{
    UCHAR struct_type = (UCHAR)(*(command->data+2));
    return (PRL_STRUCT_NAME==struct_type);
}
