#include "psm_file.h"

int psm_file_load_kbq(char *file_name, TKBQRecord **record, int &record_count)
{
    FILE *fp;

    fp = fopen(file_name, "r");
    if(NULL==fp)
    {
        return NUMBER_FALSE;
    }
    psm_file_header_fixed_t header;
    //读取文件头
    {
        int size = sizeof(header);
        int count = fread(&header, size, 1, fp);
        if(1!=count)
        {
            return NUMBER_FALSE;
        }
    }
    TKBQRecord *r = NULL;
    //读取KBQ数据
    {
        int count = header.recoredCount;
        if(0==count)
        {
            return NUMBER_TRUE;
        }
        int size = sizeof(TKBQRecord);
        r = (TKBQRecord *)malloc(count*size);
        if(NULL==r)
        {
            return NUMBER_FALSE;
        }
        int ret = fread(r, size, count, fp);
        if(ret!=count)
        {
            return NUMBER_FALSE;
        }
        *record = r;
        record_count = count;
    }
    return NUMBER_TRUE;
}


int psm_file_load_yx(char *file_name, psm_file_yx_t **record, int &record_count)
{
    FILE *fp;

    fp = fopen(file_name, "r");
    if(NULL==fp)
    {
        return NUMBER_FALSE;
    }
    psm_file_header_fixed_t header;
    //读取文件头
    {
        int size = sizeof(header);
        int count = fread(&header, size, 1, fp);
        if(1!=count)
        {
            return NUMBER_FALSE;
        }
    }
    psm_file_yx_t *r = NULL;
    //读取遥信数据
    {
        int count = header.recoredCount;
        if(0==count)
        {
            return NUMBER_TRUE;
        }
        int size = sizeof(psm_file_yx_t);
        r = (psm_file_yx_t *)malloc(count*size);
        if(NULL==r)
        {
            return NUMBER_FALSE;
        }
        int ret = fread(r, size, count, fp);
        if(ret!=count)
        {
            return NUMBER_FALSE;
        }
        *record = r;
        record_count = count;
    }
    return NUMBER_TRUE;

}


int psm_file_load_yk(char *file_name, TYKRecord **record, int &record_count)
{
    FILE *fp;

    fp = fopen(file_name, "r");
    if(NULL==fp)
    {
        return NUMBER_FALSE;
    }
    psm_file_header_fixed_t header;
    //读取文件头
    {
        int size = sizeof(header);
        int count = fread(&header, size, 1, fp);
        if(1!=count)
        {
            return NUMBER_FALSE;
        }
    }
    TYKRecord *r = NULL;
    //读取遥信数据
    {
        int count = header.recoredCount;
        if(0==count)
        {
            return NUMBER_TRUE;
        }
        int size = sizeof(TYKRecord);
        r = (TYKRecord *)malloc(count*size);
        if(NULL==r)
        {
            return NUMBER_FALSE;
        }
        int ret = fread(r, size, count, fp);
        if(ret!=count)
        {
            return NUMBER_FALSE;
        }
        *record = r;
        record_count = count;
    }
    return NUMBER_TRUE;

}
