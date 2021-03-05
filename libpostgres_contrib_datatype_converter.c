#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

typedef uint32_t Oid;
typedef _Bool bool;

typedef struct _postgres_datatype_converter_t{
	
}postgres_datatype_converter_t;

typedef struct _postgres_datatype_converter_type_t{
	Oid oid;
}postgres_datatype_converter_type_t;

typedef struct _postgres_datatype_converter_entry_t{
	Oid oid;
	int (*from_text)(
		 postgres_datatype_converter_t *ctx
		,struct _postgres_datatype_converter_entry_t *entry
		,postgres_datatype_converter_type_t *type
		,const char *in
		,size_t in_len
		,void **out
		,size_t *out_len
		,bool *inplace
	);
	int (*to_text)(
		 postgres_datatype_converter_t *ctx
		,struct _postgres_datatype_converter_entry_t *entry
		,postgres_datatype_converter_type_t *type
		,void *in
		,size_t in_len
		,char **out
		,size_t *out_len
		,bool *inplace
	);
	int (*init)(
		 postgres_datatype_converter_t *ctx
		,struct _postgres_datatype_converter_entry_t *entry
	);
	void (*deinit)(
		 postgres_datatype_converter_t *ctx
		,struct _postgres_datatype_converter_entry_t *entry
	);
	void *data;
}postgres_datatype_converter_entry_t;

#define POSTGRES_DATATYPE_CONVERTER_ENTRY( \
	 oid_                               \
	,from_text_                          \
	,to_text_                         \
	,init_                          \
	,deinit_                         \
	,data_                              \
)                                          \
[oid_] = { \
	 .oid = (oid_)                      \
	,.from_text = (from_text_)            \
	,.to_text = (to_text_)          \
	,.init = (init_)            \
	,.deinit = (deinit_)          \
	,.data = (data_)          \
},\

#include "main.h"

postgres_datatype_converter_entry_t postgres_datatype_converter_entry[] = {
#include "entry.h"
};

#define POSTGRES__ARRAY_LENGTH() (sizeof(x)/sizeof(x[0]))

int postgres_datatype_converter__from_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_type_t *type
	,const char *in
	,size_t in_len
	,void **out
	,size_t *out_len
	,bool *inplace
){
	return type->oid
	     < POSTGRES__ARRAY_LENGTH(postgres_datatype_converter_entry)
	    && postgres_datatype_converter_entry[type->oid].from_text
	     ? postgres_datatype_converter_entry[type->oid].from_text(
		 ctx
		,&postgres_datatype_converter_entry[type->oid]
		,type
		,in
		,in_len
		,out
		,out_len
		,inplace
	) : -2;
}

int postgres_datatype_converter__to_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_type_t *type
	,void *in
	,size_t in_len
	,char **out
	,size_t *out_len
	,bool *inplace
){
	return type->oid
	     < POSTGRES__ARRAY_LENGTH(postgres_datatype_converter_entry)
	    && postgres_datatype_converter_entry[type->oid].to_text
	     ? postgres_datatype_converter_entry[type->oid].to_text(
		 ctx
		,&postgres_datatype_converter_entry[type->oid]
		,type
		,in
		,in_len
		,out
		,out_len
		,inplace
	) : -2;
}

bool postgres_datatype_converter__init(
	 postgres_datatype_converter_t *ctx
){
	int ret = 0;
	size_t l = 0;

	for(;!ret && l < POSTGRES__ARRAY_LENGTH(postgres_datatype_converter_entry);l++){
 	 	if(postgres_datatype_converter_entry[l].init){
ret = postgres_datatype_converter_entry[l].init(
		 ctx
		,&postgres_datatype_converter_entry[l]
);
	 	}
	}

if(!ret && l > 0){
	while(--l >= 0){
 	 	if(postgres_datatype_converter_entry[l].deinit){
 	 	 	 ret = postgres_datatype_converter_entry[l].deinit(
		 	 	 ctx
		 	 	,&postgres_datatype_converter_entry[l]
 	 	 	 );
	 	}
	}
}

	return !ret;
}

void postgres_datatype_converter__deinit(
	 postgres_datatype_converter_t *ctx
){
	for(size_t l=0;l<POSTGRES__ARRAY_LENGTH(postgres_datatype_converter_entry);l++){
 	 	if(postgres_datatype_converter_entry[l].deinit){
 	 	 	 ret = postgres_datatype_converter_entry[l].deinit(
		 	 	 ctx
		 	 	,&postgres_datatype_converter_entry[l]
 	 	 	 );
	 	}
	}
}

#undef POSTGRES__ARRAY_LENGTH

/*
int main(){
postgres_datatype_converter_t ctx;

	const char *in = "true";
	size_t in_len = strlen(in);
	bool tmp;
	void *out = &tmp;
	size_t out_len = sizeof(bool);
	bool inplace;

if(postgres_datatype_converter__init(&ctx)){


	if(!postgres_datatype_converter__from_text(
		 &ctx
		,PDC_BOOL
		,in
		,in_len
		,&out
		,&out_len
		,&inplace
	)){
		char str_tmp[5];
		char *str_out = str_tmp;
		size_t str_out_len = sizeof(str_out)/sizeof(str_out[0]);
		printf("postgres__from_text(bool) = %d\n",tmp);
		if(!postgres_datatype_converter__to_text(
			 &ctx
			,PDC_BOOL
			,&tmp
			,sizeof(bool)
			,&str_out
			,&str_out_len
			,&inplace
		)){
			printf("postgres__to_text(bool) = %.*s\n",str_out_len,str_out);
			if(!inplace){
				free(str_out);
			}
		}else{
			puts("error");
		}
		if(!inplace){
			free(out);
		}
	}else{
		puts("error");
	}

postgres_datatype_converter__deinit(&ctx);
}
}
*/
