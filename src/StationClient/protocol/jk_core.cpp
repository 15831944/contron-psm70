#include "jk_core.h"

#include "jk_crc.h"


/*!
 * \brief jk_find_frame     查找集控数据帧
 * \param buffer            帧数据区
 * \param len               帧数据区长度
 * \param pos               查找起始位置
 * \param frame_pos         数据帧在帧数据区的起始位置
 * \param frame_len         数据帧长度
 * \return                  查找到数据帧返回1，否则返回0
 * \note                    1.如果查找到数据帧，移动pos到数据帧后；
 *                          2.此处不处理CRC校验不正确的情况，协议中规定调用者需要发送nack告知数据错误
 */
int jk_find_frame(char *buffer, int len, int &pos, int &frame_pos, int &frame_len)
{
    if(pos>=len)
    {
        return NUMBER_FALSE;
    }
    UCHAR header[] = {0xa5, 0x5a, 0xa5, 0x5a};
    int header_size = sizeof(header);
    int newsize = len;
    int frame_min_len = 18;  //数据帧除数据正文的长度(包括命令码和功能码)，数据帧最小长度
    int crc_size = 2;
    UCHAR *p = (UCHAR *)(buffer+pos);
    while(pos<newsize)
    {
        bool hasmore = (newsize-pos)>=frame_min_len;
        if(hasmore)
        {
            bool foundheader = (((*(p))==header[0])
                                &&((*(p+1))==header[1])
                                &&((*(p+2))==header[2])
                                &&((*(p+3))==header[3])
                                );
            if(foundheader)
            {
                frame_len = jk_get_ushort((char *)(p+header_size));
                frame_len += frame_min_len;  //重复计算了命令码和功能码长度，下面减去
                frame_len -= crc_size;
                bool found = (frame_len <= (newsize-pos));
                if(found)
                {
                    frame_pos = pos;
                    pos += frame_len;
                    p += frame_len;

                    return NUMBER_TRUE;

//                    USHORT crc = jk_get_ushort((char *)(p+frame_len-crc_size));
//                    USHORT calc_crc = jk_crc_32((char *)(p+header_size), frame_len-header_size-crc_size);  //crc校验不包括同步头和校验码
//                    found = (crc==calc_crc);
//                    if(found)
//                    {
//                        frame_pos = pos;
//                        pos += frame_len;
//                        p += frame_len;
//                    }
//                    else
//                    {
//                        //报文检验不正确
//                        pos++;
//                        p++;
//                    }
                }
                else
                {
                    //报文长度不够
                    break;
                }//found
            }
            else
            {
                //未找到报文头
                pos++;
                p++;
            }//foundheader
        }
        else
        {
            //未找到报文头
            break;
        }//hasmore
    }//while

    return NUMBER_FALSE;
}

/*!
 * \brief jk_get_ushort     从数据区中取两字节中存储的数值
 * \param data              数据区
 * \return                  数值
 * \note                    高前低后，调用前保证访问数据区两字节不会越界
 */
USHORT jk_get_ushort(const char *data)
{
    UCHAR hi = *data;
    UCHAR lo = *(data+1);
    USHORT result = (hi<<8)|lo;
    return result;
}



void jk_set_ushort(char **data, USHORT value)
{
    UCHAR *p = (UCHAR *)(*data);
    UCHAR hi = (UCHAR)(value>>8);
    UCHAR lo = (UCHAR)value;
    (*p) = hi;
    *(p+1) = lo;
    p += 2;
    (*data) = (char *)p;
}


void jk_set_uchar(char **data, UCHAR value)
{
    UCHAR *p = (UCHAR *)(*data);
    (*p) = value;
    p++;
    (*data) = (char *)p;

}


void jk_set_buffer(char **data, char *buffer, int size)
{
    if(NULL==buffer || 0==size)
    {
        return;
    }
    UCHAR *p = (UCHAR *)(*data);
    memcpy(p, buffer, size);
    p += size;
    (*data) = (char *)p;
}
