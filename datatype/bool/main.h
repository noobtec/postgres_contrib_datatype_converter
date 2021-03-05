#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

postgres_datatype_converter_type_t pdc_bool = {
	.oid = 16
};

#define PDC_BOOL (&pdc_bool)

#define pg_strncasecmp strncasecmp

bool
pdc_bool__parse_bool_with_len(const char *value, size_t len, bool *result)
{
 switch (*value)
 {
     case 't':
     case 'T':
         if (pg_strncasecmp(value, "true", len) == 0)
         {
             if (result)
                 *result = true;
             return true;
         }
         break;
     case 'f':
     case 'F':
         if (pg_strncasecmp(value, "false", len) == 0)
         {
             if (result)
                 *result = false;
             return true;
         }
         break;
     case 'y':
     case 'Y':
         if (pg_strncasecmp(value, "yes", len) == 0)
         {
             if (result)
                 *result = true;
             return true;
         }
         break;
     case 'n':
     case 'N':
         if (pg_strncasecmp(value, "no", len) == 0)
         {
             if (result)
                 *result = false;
             return true;
         }
         break;
     case 'o':
     case 'O':
         /* 'o' is not unique enough */
         if (pg_strncasecmp(value, "on", (len > 2 ? len : 2)) == 0)
         {
             if (result)
                 *result = true;
             return true;
         }
         else if (pg_strncasecmp(value, "off", (len > 2 ? len : 2)) == 0)
         {
             if (result)
                 *result = false;
             return true;
         }
         break;
     case '1':
         if (len == 1)
         {
             if (result)
                 *result = true;
             return true;
         }
         break;
     case '0':
         if (len == 1)
         {
             if (result)
                 *result = false;
             return true;
         }
         break;
     default:
         break;
 }

 if (result)
     *result = false;        /* suppress compiler warning */
 return false;
}

#undef pg_strncasecmp

int pdc_bool__from_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_entry_t *entry
	,postgres_datatype_converter_type_t *type
	,const char *in
	,size_t in_len
	,void **out
	,size_t *out_len
	,bool *inplace
){
	int ret = -1;
	bool result;
	
	*out_len = sizeof(uint8_t);

	if(pdc_bool__parse_bool_with_len(
		 in
		,in_len
		,&result
	)){
		if(*out_len >= sizeof(uint8_t)){
			*((uint8_t*)*out) = result;
			*inplace = true;
			ret = 0;
		}else{
			uint8_t *tmp = *out = malloc(sizeof(uint8_t));
			if(tmp){
				*tmp = result;
				*inplace = false;
				ret = 0;
			}
		}
	}

	return ret;
}

int pdc_bool__to_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_entry_t *entry
	,postgres_datatype_converter_type_t *type
	,void *in
	,size_t in_len
	,char **out
	,size_t *out_len
	,bool *inplace
){
	int ret = -1;
	char *tmp;
	
	*inplace = true;
	*out_len = 2;
	
	if(in_len >= sizeof(uint8_t)){
		if(*out_len < 2){
			tmp = *out = malloc(2);
			*inplace = false;
		}else{
			tmp = *out;
		}

		if(tmp){
			tmp[0] = *((uint8_t*)in) ? '1' : '0';
			tmp[1] = 0;
			
			ret = 0;
		}
	}

	return ret;
}
