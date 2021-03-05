#define _GNU_SOURCE

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

typedef uint32_t Oid;
typedef _Bool bool;

typedef struct _postgres_datatype_converter_t{
	char *error;
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
	 oid_                              \
	,from_text_                        \
	,to_text_                          \
	,init_                             \
	,deinit_                           \
	,data_                             \
)                                          \
[oid_] = {                                 \
	 .oid = (oid_)                     \
	,.from_text = (from_text_)         \
	,.to_text = (to_text_)             \
	,.init = (init_)                   \
	,.deinit = (deinit_)               \
	,.data = (data_)                   \
},

#include "main.h"

postgres_datatype_converter_entry_t postgres_datatype_converter_entry[] = {
#include "entry.h"
};

void postgres_datatype_converter__set_last_error(
	 postgres_datatype_converter_t *ctx
	,char *format
	,...
){
	va_list args;
  	va_start(args,format);
	
	if(ctx->error){
		free(ctx->error);
	}
	
	if(vasprintf(&ctx->error,format,args) == -1){
		ctx->error = 0;
	}
	
	va_end(args);
}

char *postgres_datatype_converter__get_last_error(
	 postgres_datatype_converter_t *ctx
){
	return ctx->error ? ctx->error : "Unknown Error";
}

static inline size_t postgres_datatype_converter__get_length(
	 postgres_datatype_converter_t *ctx
){
	return sizeof(postgres_datatype_converter_entry)
	     / sizeof(postgres_datatype_converter_entry[0]);
}

bool postgres_datatype_converter__from_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_type_t *type
	,const char *in
	,size_t in_len
	,void **out
	,size_t *out_len
	,bool *inplace
){
	postgres_datatype_converter_entry_t *entry;
	
	return type->oid < postgres_datatype_converter__get_length(ctx)
	    && (entry = &postgres_datatype_converter_entry[type->oid])->from_text
	    && !entry->from_text(
			 ctx
			,entry
			,type
			,in
			,in_len
			,out
			,out_len
			,inplace
		)
	;
}

bool postgres_datatype_converter__to_text(
	 postgres_datatype_converter_t *ctx
	,postgres_datatype_converter_type_t *type
	,void *in
	,size_t in_len
	,char **out
	,size_t *out_len
	,bool *inplace
){
	postgres_datatype_converter_entry_t *entry;
	return type->oid < postgres_datatype_converter__get_length(ctx)
	    && (entry = &postgres_datatype_converter_entry[type->oid])->to_text
	    && !entry->to_text(
			 ctx
			,entry
			,type
			,in
			,in_len
			,out
			,out_len
			,inplace
		)
	;
}

bool postgres_datatype_converter__init(
	 postgres_datatype_converter_t *ctx
){
	postgres_datatype_converter_entry_t *entry;
	bool ret = true;
	size_t l = 0;
	
	ctx->error = 0;

	for(;ret && l < postgres_datatype_converter__get_length(ctx);l++){
		entry = &postgres_datatype_converter_entry[l];
 	 	if(entry->init){
			ret = !entry->init(
				 ctx
				,entry
			);
	 	}
	}

	if(!ret && l > 0){
		while(--l >= 0){
			if(entry->deinit){
				 entry->deinit(
					 ctx
					,entry
				 );
			}
		}
	}

	return ret;
}

void postgres_datatype_converter__deinit(
	 postgres_datatype_converter_t *ctx
){
	postgres_datatype_converter_entry_t *entry;
	for(size_t l=0;l < postgres_datatype_converter__get_length(ctx);l++){
		entry = &postgres_datatype_converter_entry[l];
 	 	if(entry->deinit){
 	 	 	 entry->deinit(
		 	 	 ctx
		 	 	,entry
 	 	 	 );
	 	}
	}
	
	if(ctx->error){
		free(ctx->error);
	}
}

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


	if(postgres_datatype_converter__from_text(
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
		if(postgres_datatype_converter__to_text(
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
