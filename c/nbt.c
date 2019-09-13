#define _NBT_C_BODY
#include "nbt.h"
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include <zlib.h>
#include <stdarg.h>
struct NBTTag{
    char *name;
    NBTTagType type;
};
#define reportError(err,detail,...) {lasterr=err;snprintf(errdetail,999, #err ": " detail "@%s:%d",##__VA_ARGS__,__FILE__,__LINE__);}
#define crash() fprintf(stderr,"crash() called. %s() at %s,%d\n",__func__,__FILE__,__LINE__);abort()
#define nonnull(a,rval,msg...) {if(!a){fprintf(stderr,msg);reportError(NBT_NULLPTR,msg);return rval;}}
#define warn(a,b...) fprintf(stderr,a"\n",##b)
static NBTErr lasterr=NBT_NOERR;
static char errdetail[1000]="No error";
static NBTErr writeIN(const void *I,size_t N,FILE *stream){
    char *tmp=malloc(N);
    const char *v=I;
    int t=1;
    if(((int)(*((char *)&t)))==t){//判断小端
        for(int i=N-1,j=0;i>=0;i--,j++){
            tmp[j]=v[i];
        }
        int size=fwrite(tmp,1,N,stream);
        if(size!=N){
            reportError(NBT_IOERR,"Cannot write to stream");
            free(tmp);
            return NBT_IOERR;
        }
    }else{
        int size=fwrite(I,1,N,stream);
        if(size!=N){
            reportError(NBT_IOERR,"Cannot write to stream");
            free(tmp);
            return NBT_IOERR;
        }
    }
    free(tmp);
    return NBT_NOERR;
}
#define DNwrite(data,stream) ({typeof(data) _dt_ =(data);writeIN(&_dt_,sizeof(data),stream);})
static NBTErr fnread(char *buffer,size_t size,FILE *stream){
    size_t readed=0;
    while(readed<size){
        readed+=fread(buffer+readed,1,size-readed,stream);
        if(ferror(stream)){
            reportError(NBT_IOERR,"IO error %d",ferror(stream));
            return NBT_IOERR;
        }
        if(feof(stream)){
            reportError(NBT_IOERR,"EOF reach");
            return NBT_IOERR;
        }
    }
    return NBT_NOERR;
}
static NBTErr readIN(void *I,size_t N,FILE *stream){
    char *tmp=malloc(N);
    NBTErr err=fnread(tmp,N,stream);
    if(err){
        free(tmp);
        return err;
    }
    char *v=I;
    int t=1;
    if(((int)(*((char *)&t)))==t){
        for(int i=N-1,j=0;i>=0;i--,j++){
            v[j]=tmp[i];
        }
    }else{
        for(int i=0;i<N;i++){
            v[i]=tmp[i];
        }
    }
    free(tmp);
    return NBT_NOERR;
}
#define DNread(data,stream) readIN(&data,sizeof(data),stream)
static NBTErr writeS(const char *s,FILE *stream){
    int len=strlen(s);
    if(len>32767){
        warn("The String `%s\' is too long.",s);
        reportError(NBT_ILLEGAL,"String `%s' is too long",s);
        return NBT_ILLEGAL;
    }
    DNwrite((short)len,stream);
    int size=fwrite(s,1,len,stream);
    if(size!=len){
        reportError(NBT_IOERR,"Cannot write String `%s' to stream",s);
        return NBT_IOERR;
    }
    return NBT_NOERR;
}
#define DSwrite writeS
static char *readS(int *count,FILE *stream){
    short len;
    NBTErr err=DNread(len,stream);
    if(err){
        return NULL;
    }
    if(len<0){
        reportError(NBT_IOERR,"Cannot read String because length<0");
        return NULL;
    }
    char *s=malloc(len+1);
    err=fnread(s,len,stream);
    if(err){
        return NULL;
    }
    s[len]=0;
    *count=len;
    return s;
}
#define DSread readS
static char *copystr(const char *str){
    char *s=malloc(strlen(str)+1);
    strcpy(s,str);
    return s;
}
static void *copymem(const void *buf,size_t size){
    void *r=malloc(size);
    memcpy(r,buf,size);
    return r;
}
static int hashstr(const void *s){
    int hash=0;
    const char *str=s;
    for(;*str;str++){
        hash+=(*str)*(*str);
    }
    return hash;
}
static int equalsstr(const void *a,const void *b){
    return !strcmp(a,b);
}
#define nalloc(type) malloc(sizeof(type))
typedef struct bytet_t{
    char *name;
    NBTTagType type;
    byte b;
}TagByte ;
TagByte *NBT_createByteTag(const char *name,byte bt){
    nonnull(name,NULL,"NBT_createByteTag(): name must not be null.");
    TagByte *b=nalloc(TagByte);
    b->name=copystr(name);
    b->type=1;
    b->b=bt;
    return b;
}
int NBT_isByte(NBTTag *tag){
    nonnull(tag,0,"NBT_isByte(): tag must not be null.");
    return tag->type==1;
}
byte NBT_getByte(TagByte *tag){
    return tag->b;
}
void NBT_setByte(TagByte *tag,byte b){
    tag->b=b;
}
NBTErr NBT_writeByte(TagByte *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeByte(): stream nust not be null.");
    byte b=tag->b;
    return DNwrite(b,stream);
}
TagByte *NBT_readByte(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readByte(): name must not be null.");
    nonnull(stream,NULL,"NBT_readByte(): stream must not be null.");
    byte b;
    NBTErr err=DNread(b,stream);
    if(err){
        return NULL;
    }
    return NBT_createByteTag(name,b);
}
TagByte *NBT_copyByte(TagByte *tag){
    return NBT_createByteTag(tag->name,tag->b);
}
void NBT_destroyByte(TagByte *tag){
    free(tag->name);
    free(tag);
}

typedef struct shortt_t{
    char *name;
    NBTTagType type;
    short s;
}TagShort;
TagShort *NBT_createShortTag(const char *name,short s){
    nonnull(name,NULL,"NBT_createShortTag(): name must not be null.");
    TagShort *r=nalloc(TagShort);
    r->name=copystr(name);
    r->type=2;
    r->s=s;
    return r;
}
int NBT_isShort(NBTTag *tag){
    nonnull(tag,0,"NBT_isShort(): tag must not be null.");
    return tag->type==2;
}
short NBT_getShort(TagShort *tag){
    return tag->s;
}
void NBT_setShort(TagShort *tag,short s){
    tag->s=s;
}
NBTErr NBT_writeShort(TagShort *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeShort(): stream must not be null.");
    short b=tag->s;
    return DNwrite(b,stream);
}
TagShort *NBT_readShort(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readShort(): name must not be null.");
    nonnull(stream,NULL,"NBT_readShort(): stream must not be null.");
    short b;
    DNread(b,stream);
    return NBT_createShortTag(name,b);
}
TagShort *NBT_copyShort(TagShort *tag){
    return NBT_createShortTag(tag->name,tag->s);
}
void NBT_destroyShort(TagShort *tag){
    free(tag->name);
    free(tag);
}
typedef struct intt_t{
    char *name;
    NBTTagType type;
    int i;
}TagInt;
TagInt *NBT_createIntTag(const char *name,int i){
    nonnull(name,NULL,"NBT_createIntTag(): name must not be null.");
    TagInt *r=nalloc(TagInt);
    r->name=copystr(name);
    r->type=3;
    r->i=i;
    return r;
}
int NBT_isInt(NBTTag *tag){
    nonnull(tag,0,"NBT_isInt(): tag must not be null.");
    return tag->type==3;
}
int NBT_getInt(TagInt *tag){
    return tag->i;
}
void NBT_setInt(TagInt *tag,int i){
    tag->i=i;
}
NBTErr NBT_writeInt(TagInt *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeInt(): stream must not be null.");
    int i=tag->i;
    return DNwrite(i,stream);
}
TagInt *NBT_readInt(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readInt(): name must not be null.");
    nonnull(stream,NULL,"NBT_readInt(): stream must mot be null.");
    int i;
    NBTErr err=DNread(i,stream);
    if(err){
        return NULL;
    }
    return NBT_createIntTag(name,i);
}
TagInt *NBT_copyInt(TagInt *tag){
    return NBT_createIntTag(tag->name,tag->i);
}
void NBT_destroyInt(TagInt *tag){
    free(tag->name);
    free(tag);
}
typedef struct longt_t{
    char *name;
    NBTTagType type;
    long long l;
}TagLong;
TagLong *NBT_createLongTag(const char *name,long long l){
    nonnull(name,NULL,"NBT_createLongTag(): name must not be null.");
    TagLong *r=nalloc(TagLong);
    r->name=copystr(name);
    r->type=4;
    r->l=l;
    return r;
}
int NBT_isLong(NBTTag *tag){
    nonnull(tag,0,"NBT_isLong(): tag must not be null.");
    return tag->type==4;
}
long long NBT_getLong(TagLong *tag){
    return tag->l;
}
void NBT_setLong(TagLong *tag,long long l){
    tag->l=l;
}
NBTErr NBT_writeLong(TagLong *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeLong(): tag must not be null.");
    long long l=tag->l;
    return DNwrite(l,stream);
}
TagLong *NBT_readLong(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readLong(): name must not be null.");
    nonnull(stream,NULL,"NBT_readLong(): stream must not be null.");
    long long l;
    NBTErr err=DNread(l,stream);
    if(err){
        return NULL;
    }
    return NBT_createLongTag(name,l);
}
TagLong *NBT_copyLong(TagLong *tag){
    return NBT_createLongTag(tag->name,tag->l);
}
void NBT_destroyLong(TagLong *tag){
    free(tag->name);
    free(tag);
}
typedef struct floatt_t{
    char *name;
    NBTTagType type;
    float f;
}TagFloat;
TagFloat *NBT_createFloatTag(const char *name,float f){
    nonnull(name,NULL,"NBT_createFloatTag(): name must not be null.");
    TagFloat *r=nalloc(TagFloat);
    r->name=copystr(name);
    r->type=5;
    r->f=f;
    return r;
}
int NBT_isFloat(NBTTag *tag){
    nonnull(tag,0,"NBT_isFloat(): tag must not be null.");
    return tag->type==5;
}
float NBT_getFloat(TagFloat *tag){
    return tag->f;
}
void NBT_setFloat(TagFloat *tag,float f){
    tag->f=f;
}
NBTErr NBT_writeFloat(TagFloat *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeFloat(): stream must not be null.");
    float f=tag->f;
    return DNwrite(f,stream);
}
TagFloat *NBT_readFloat(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readFloat(): name must not be null.");
    nonnull(stream,NULL,"NBT_readFloat(): stream must not be null.");
    float f;
    NBTErr err=DNread(f,stream);
    if(err){
        return NULL;
    }
    return NBT_createFloatTag(name,f);
}
TagFloat *NBT_copyFloat(TagFloat *tag){
    return NBT_createFloatTag(tag->name,tag->f);
}
void NBT_destroyFloat(TagFloat *tag){
    free(tag->name);
    free(tag);
}
typedef struct doublet_t{
    char *name;
    NBTTagType type;
    double d;
}TagDouble;
TagDouble *NBT_createDoubleTag(const char *name,double d){
    TagDouble *r=nalloc(TagDouble);
    r->name=copystr(name);
    r->type=6;
    r->d=d;
    return r;
}
int NBT_isDouble(NBTTag *tag){
    nonnull(tag,0,"NBT_isDouble(): tag must notbe null.");
    return tag->type==6;
}
double NBT_getDouble(TagDouble *tag){
    return tag->d;
}
void NBT_setDouble(TagDouble *tag,double d){
    tag->d=d;
}
NBTErr NBT_writeDouble(TagDouble *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeDouble(): stream must not be null.");
    double d=tag->d;
    return DNwrite(d,stream);
}
TagDouble *NBT_readDouble(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readDouble(): name must not be null.");
    nonnull(stream,NULL,"NBT_readDouble(): stream must not be null.");
    double d;
    NBTErr err=DNread(d,stream);
    if(err){
        return NULL;
    }
    return NBT_createDoubleTag(name,d);
}
TagDouble *NBT_copyDouble(TagDouble *tag){
    return NBT_createDoubleTag(tag->name,tag->d);
}
void NBT_destroyDouble(TagDouble *tag){
    free(tag->name);
    free(tag);
}
typedef struct bytarrt_t{
    char *name;
    NBTTagType type;
    int size;
    byte *arr;
}TagByteArray;
TagByteArray *NBT_createByteArrayTag(const char *name,int count,const byte *array){
    nonnull(name,NULL,"NBT_createByteArrayTag(): name must not be null.");
    if(count>0){
        nonnull(array,NULL,"NBT_createByteArrayTag(): array must not be null when count != 0.");
    }
    TagByteArray *r=nalloc(TagByteArray);
    r->name=copystr(name);
    r->type=7;
    r->size=count;
    if(count>0){
        r->arr=copymem(array,sizeof(byte)*count);
    }else{
        r->arr=NULL;
    }
    return r;
}
int NBT_isByteArray(NBTTag *tag){
    nonnull(tag,0,"NBT_isByteArray(): tag must not be null.");
    return tag->type==7;
}
byte *NBT_getByteArray(TagByteArray *tag,int *count){
    if(tag->size){
        *count=tag->size;
        return copymem(tag->arr,sizeof(byte)*tag->size);
    }else{
        *count=0;
        return NULL;
    }
}
void NBT_setByteArray(TagByteArray *tag,int count,const byte *array){
    if(tag->arr)
        free(tag->arr);
    if(count){
        tag->arr=copymem(array,count);
    }else{
        tag->arr=NULL;
    }
    tag->size=count;
}
byte NBT_getByteArrayIndex(TagByteArray *tag,int index){
    if(index>tag->size){
    	fprintf(stderr,"NBT_getByteArrayIndex(): the index %d is larger than the size %d.\n",index,tag->size);
        crash();
    }
    return tag->arr[index];
}
int NBT_getByteArraySize(TagByteArray *tag){
    return tag->size;
}
NBTErr NBT_writeByteArray(TagByteArray *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeByteArray(): stream must not be null.");
    int size=tag->size;
    byte *arr=tag->arr;
    NBTErr err=DNwrite(size,stream);
    if(err){
        return err;
    }
    for(int i=0;i<size;i++){
        if(err=DNwrite(arr[i],stream)){
            return err;
        }
    }
    return NBT_NOERR;
}
TagByteArray *NBT_readByteArray(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readByteArray(): name must not be null.");
    nonnull(stream,NULL,"NBT_readByteArray(): stream must not be null.");
    int size;
    NBTErr err=DNread(size,stream);
    if(err){
        return NULL;
    }
    byte *buf;
    if(size>0)
        buf=malloc(sizeof(byte)*size);
    else
        buf=NULL;
    for(int i=0;i<size;i++){
        if(err=DNread(buf[i],stream)){
            free(buf);
            return NULL;
        }
    }
    TagByteArray *tag=nalloc(TagByteArray);
    tag->name=copystr(name);
    tag->type=7;
    tag->size=size;
    tag->arr=buf;
    return tag;
}
TagByteArray *NBT_copyByteArray(TagByteArray *tag){
    TagByteArray *t=(TagByteArray *)tag;
    return NBT_createByteArrayTag(t->name,t->size,t->arr);
}
void NBT_destroyByteArray(TagByteArray *tag){
    free(tag->name);
    if(tag->size>0){
        free(tag->arr);
    }
    free(tag);
}
typedef struct strt_t{
    char *name;
    NBTTagType type;
    short len;
    char *str;
}TagString;
TagString *NBT_createStringTag(const char *name,const char *val){
    nonnull(name,NULL,"NBT_createStringTag(): name must not be null.");
    nonnull(val,NULL,"NBT_createStringTag(): val must not be null.");
    int len=strlen(val);
    if(len>32767){
        reportError(NBT_ILLEGAL,"NBT_createStringTag(): val `%s' too long",val);
        return NULL;
    }
    TagString *tag=nalloc(TagString);
    tag->name=copystr(name);
    tag->type=8;
    tag->len=len;
    tag->str=copystr(val);
    return tag;
}
int NBT_isString(NBTTag *tag){
    nonnull(tag,0,"NBT_isString(): tag must not be null.");
    return tag->type==8;
}
char *NBT_getString(TagString *tag,int *count){
    *count=tag->len;
    return copystr(tag->str);
}
void NBT_setString(TagString *tag,const char *str){
    tag->len=strlen(str);
    free(tag->str);
    tag->str=copystr(str);
}
int NBT_getStringSize(TagString *tag){
    return tag->len;
}
NBTErr NBT_writeString(TagString *tag,FILE *stream){
    return DSwrite(tag->str,stream);
}
TagString *NBT_readString(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readString(): name must not be null.");
    nonnull(stream,NULL,"NBT_readString(): stream must not be null.");
    int x;
    char *n=DSread(&x,stream);
    if(!n){
        return NULL;
    }
    TagString *tag=nalloc(TagString);
    tag->name=copystr(name);
    tag->type=8;
    tag->len=x;
    tag->str=n;
    return tag;
}
TagString *NBT_copyString(TagString *tag){
    TagString *r=nalloc(TagString);
    r->name=copystr(tag->name);
    r->type=8;
    r->len=tag->len;
    r->str=copymem(tag->str,tag->len+1);
    return r;
}
void NBT_destroyString(TagString *tag){
    free(tag->str);
    free(tag->name);
    free(tag);
}
typedef struct listt_t{
    char *name;
    NBTTagType type;
    NBTTagType list_type;
    int size;
    NBTTag **arr;
}TagList;
TagList *NBT_createListTag(const char *name,int count,NBTTag **array){
    nonnull(name,NULL,"NBT_createListTag(): name must not be null.");
    if(count>0){
        nonnull(array,NULL,"NBT_createListTag(): array must not be null when count>0.");
    }
    TagList *tag=nalloc(TagList);
    tag->name=copystr(name);
    tag->type=9;
    tag->size=count;
    if(count>0){
        tag->arr=malloc(sizeof(NBTTag *)*count);
        for(int i=0;i<count;i++){
            if(*(array[i]->name)!=0){
                warn("NBT_createListTag(): name\"%s\" should be \"\".",array[i]->name);
                reportError(NBT_ILLEGAL,"NBT_createListTag(): Illegal name");
                for(int j=0;j<i;j++)
                    free(tag->arr[j]);
                free(tag->arr);
                free(tag->name);
                free(tag);
                return NULL;
            }
            tag->arr[i]=NBT_copy(array[i]);
        }
        tag->list_type=array[0]->type;
    }else{
        tag->arr=0;
        tag->list_type=0;
    }
    return tag;
}
int NBT_isList(NBTTag *tag){
    nonnull(tag,0,"NBT_isList(): tag must not be null.");
    return tag->type==9;
}
NBTTag **NBT_getList(TagList *tag,int *count){
    if(tag->size>0){
        NBTTag **r=malloc(sizeof(NBTTag *)*tag->size);
        for(int i=0;i<tag->size;i++){
            r[i]=NBT_copy(tag->arr[i]);
        }
        *count=tag->size;
        return r;
    }else{
        *count=0;
        return NULL;
    }
}
void NBT_setList(TagList *tag,int count,NBTTag **array){
    if(tag->size){
        for(int i=0;i<tag->size;i++){
            NBT_destroy(tag->arr[i]);
        }
        free(tag->arr);
    }
    tag->size=count;
    if(count>0){
        tag->arr=malloc(sizeof(NBTTag *)*count);
        for(int i=0;i<count;i++){
            tag->arr[i]=NBT_copy(array[i]);
        }
        tag->list_type=array[0]->type;
    }else{
        tag->arr=0;
        tag->list_type=0;
    }
}
NBTTag *NBT_getListIndex(TagList *tag,int index){
    if(index>tag->size){
    	fprintf(stderr,"NBT_getListIndex(): the index %d is larger than the size %d.\n",index,tag->size);
        crash();
    }
    return NBT_copy(tag->arr[index]);
}
int NBT_getListSize(TagList *tag){
    return tag->size;
}
NBTTagType NBT_getListType(TagList *tag){
    return tag->list_type;
}
NBTErr NBT_writeList(TagList *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeList(): stream must not be null.");
    byte list_type=tag->list_type;
    NBTErr err=DNwrite(list_type,stream);
    if(err){
        return err;
    }
    int size=tag->size;
    if(err=DNwrite(size,stream)){
        return err;
    }
    for(int i=0;i<size;i++){
        if(err=NBT_writeBody(tag->arr[i],stream)){
            return err;
        }
    }
    return NBT_NOERR;
}
TagList *NBT_readList(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readList(): name must not be null.");
    nonnull(stream,NULL,"NBT_readList(): stream must not be null.")
    byte list_type;
    NBTErr err=DNread(list_type,stream);
    if(err){
        return NULL;
    }
    int size;
    if(err=DNread(size,stream)){
        return NULL;
    }
    NBTTag **buf=malloc(sizeof(NBTTag *)*size);
    for(int i=0;i<size;i++){
        buf[i]=NBT_readBody("",list_type,stream);
        if(!buf[i]){
            for(int j=0;j<i;j++){
                NBT_destroy(buf[i]);
            }
            return NULL;
        }
    }
    TagList *tag=nalloc(TagList);
    tag->name=copystr(name);
    tag->type=9;
    tag->size=size;
    tag->arr=buf;
    return tag;
}
TagList *NBT_copyList(TagList *tag){
    return NBT_createListTag(tag->name,tag->size,tag->arr);
}
void NBT_destroyList(TagList *tag){
    free(tag->name);
    for(int i=0;i<tag->size;i++){
        NBT_destroy(tag->arr[i]);
    }
    free(tag->arr);
    free(tag);
}
typedef struct compoundt_t{
    char *name;
    NBTTagType type;
    HashMap(char *,NBTTag *) *map;
    int size;
}TagCompound;
TagCompound *NBT_createCompoundTag(const char *name,int size,NBTTag **array){
    nonnull(name,NULL,"NBT_createCompoundTag(): name must not be null.");
    TagCompound *tag=nalloc(TagCompound);
    tag->name=copystr(name);
    tag->type=10;
    HashMap *map=tag->map=HashMap_new((int (*)(const void *))hashstr,(int (*)(const void *,const void *))equalsstr,
            (void *(*)(const void *))copystr,free,
            (void *(*)(const void *))NBT_copy,(void (*)(void *))NBT_destroy);
    for(int i=0;i<size;i++){
        if(!HashMap_contains(map,array[i]->name)){
            HashMap_put(map,array[i]->name,array[i]);
        }else{
            reportError(NBT_ILLEGAL,"NBT_createCompoundTag(): Members have same name");
            free(tag->name);
            free(tag);
            return NULL;
        }
    }
    tag->size=size;
    return tag;
}
int NBT_isCompound(NBTTag *tag){
    nonnull(tag,0,"NBT_isCompound(): tag must not be null.");
    return tag->type==10;
}
HashMap *NBT_getCompound(TagCompound *tag){
    return HashMap_copy(tag->map);
}
void NBT_setCompound(TagCompound *tag,HashMap *map){
    HashMap_destroy(tag->map);
    tag->map=HashMap_copy(map);
    tag->size=HashMap_size(map);
}
NBTTag *NBT_getCompoundIndex(TagCompound *tag,const char *index){
    return HashMap_get(tag->map,index);
}
void NBT_setCompoundIndex(TagCompound *tag,NBTTag *ele){
    HashMap_put(tag->map,ele->name,ele);
    tag->size=HashMap_size(tag->map);
}
void NBT_removeCompoundIndex(TagCompound *tag,const char *index){
    HashMap_remove(tag->map,index);
    tag->size=HashMap_size(tag->map);
}
void NBT_setCompoundByte(TagCompound *tag,const char *index,byte value){
    TagByte *insert=NBT_createByteTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundShort(TagCompound *tag,const char *index,short value){
    TagShort *insert=NBT_createShortTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundInt(TagCompound *tag,const char *index,int value){
    TagInt *insert=NBT_createIntTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundLong(TagCompound *tag,const char *index,long long value){
    TagLong *insert=NBT_createLongTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundFloat(TagCompound *tag,const char *index,float value){
    TagFloat *insert=NBT_createFloatTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundDouble(TagCompound *tag,const char *index,double value){
    TagDouble *insert=NBT_createDoubleTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundString(TagCompound *tag,const char *index,const char *value){
    TagString *insert=NBT_createStringTag(index,value);
    HashMap_put(tag->map,index,insert);
    NBT_destroy((NBTTag *)insert);
}
void NBT_setCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,...){
    va_list ap;
    va_start(ap,interface);
    NBT_vsetCompoundInterface(tag,index,object,interface,ap);
    va_end(ap);
}
void NBT_vsetCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,va_list options){
    NBTTag *tag_obj=NBT_allocTagByInterface(interface);
    NBT_vserialize(object,tag_obj,interface,options);
    NBT_setName(tag_obj,index);
    NBT_setCompoundIndex(tag,tag_obj);
    NBT_destroy(tag_obj);
}
/*若无法取到值，则返回fallback,字符串返回复制后的值*/
byte NBT_getCompoundByte(TagCompound *tag,const char *index,byte fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_BYTE(rval)){
        fallback=NBT_getByte((TagByte *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
short NBT_getCompoundShort(TagCompound *tag,const char *index,short fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_SHORT(rval)){
        fallback=NBT_getShort((TagShort *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
int NBT_getCompoundInt(TagCompound *tag,const char *index,int fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_INT(rval)){
        fallback=NBT_getInt((TagInt *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
long long NBT_getCompoundLong(TagCompound *tag,const char *index,long long fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_LONG(rval)){
        fallback=NBT_getLong((TagLong *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
float NBT_getCompoundFloat(TagCompound *tag,const char *index,float fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_FLOAT(rval)){
        fallback=NBT_getFloat((TagFloat *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
double NBT_getCompoundDouble(TagCompound *tag,const char *index,double fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_DOUBLE(rval)){
        fallback=NBT_getDouble((TagDouble *)rval);
    }
    if(rval)
        NBT_destroy(rval);
    return fallback;
}
char *NBT_getCompoundString(TagCompound *tag,const char *index,int *count,const char *fallback){
    NBTTag *rval=HashMap_get(tag->map,index);
    if(NBT_STRING(rval)){
        fallback=NBT_getString((TagString *)rval,count);
    }else{
        if(count){
            *count=strlen(fallback);
        }
        if(fallback)
            fallback=copystr(fallback);
    }
    if(rval)
        NBT_destroy(rval);
    return (char *)fallback;
}
void NBT_getCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,...){
    va_list ap;
    va_start(ap,interface);
    NBT_vgetCompoundInterface(tag,index,object,interface,ap);
    va_end(ap);
}
void NBT_vgetCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,va_list options){
    NBTTag *obj_tag=NBT_getCompoundIndex(tag,index);
    if(obj_tag->type==interface->nbtType){
        NBT_vdeserialize(object,obj_tag,interface,options);
    }
    NBT_destroy(obj_tag);
}
void NBT_clearCompoundMap(TagCompound *tag){
    HashMap_destroy(tag->map);
    tag->map=HashMap_new((int (*)(const void *))hashstr,(int (*)(const void *,const void *))equalsstr,
            (void *(*)(const void *))copystr,free,
            (void *(*)(const void *))NBT_copy,(void (*)(void *))NBT_destroy);
    tag->size=0;
}
int NBT_getCompoundSize(TagCompound *tag){
    return tag->size;
}
char **NBT_getCompoundKeys(TagCompound *tag,int *count){
    return (char **)HashMap_keys(tag->map,count);
}
NBTTagType NBT_getCompoundIndexType(TagCompound *tag,const char *index){
    NBTTag *t=HashMap_get(tag->map,index);
    if(!t){
        return TAG_END;
    }
    NBTTagType r=t->type;
    NBT_destroy(t);
    return r;
}
NBTErr NBT_writeCompound(TagCompound *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeCompound(): stream must not be null.");
    HashMap *map=tag->map;
    int count;
    char **k=(char **)HashMap_keys(map,&count);
    for(int i=0;i<count;i++){
        NBTTag *v=HashMap_get(map,k[i]);
        NBTErr err=NBT_write(v,stream);
        if(err){
            for(int j=i;j<count;j++){
                free(k[j]);
            }
            free(k);
            return err;
        }
        NBT_destroy(v);
        free(k[i]);
    }
    NBTErr err=DNwrite((byte)0,stream);
    free(k);
    return err;
}
TagCompound *NBT_readCompound(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readCompound(): name must not be null.")
    nonnull(stream,NULL,"NBT_readCompound(): stream must not be null.")
    HashMap *map=HashMap_new((int (*)(const void *))hashstr,(int (*)(const void *,const void *))equalsstr,
            (void *(*)(const void *))copystr,free,
            (void *(*)(const void *))NBT_copy,(void (*)(void *))NBT_destroy);
    while(1){
        NBTTag *v=NBT_read(stream);
        if(v){
            HashMap_put(map,v->name,v);
            NBT_destroy(v);
        }else{
            break;
        }
    }
    TagCompound *tag=nalloc(TagCompound);
    tag->name=copystr(name);
    tag->type=10;
    char **k=(char **)HashMap_keys(map,&tag->size);
    for(int i=0;i<tag->size;i++){
        free(k[i]);
    }
    free(k);
    tag->map=map;
    return tag;
}
TagCompound *NBT_copyCompound(TagCompound *tag){
    TagCompound *r=nalloc(TagCompound);
    r->name=copystr(tag->name);
    r->type=10;
    r->map=HashMap_copy(tag->map);
    r->size=tag->size;
    return r;
}
void NBT_destroyCompound(TagCompound *tag){
    HashMap_destroy(tag->map);
    free(tag->name);
    free(tag);
}
typedef struct intarrt_t{
    char *name;
    NBTTagType type;
    int size;
    int *arr;
}TagIntArray;
TagIntArray *NBT_createIntArrayTag(const char *name,int count,const int *array){
    nonnull(name,NULL,"NBT_createIntArrayTag(): name must not be null.");
    if(count>0){
        nonnull(array,NULL,"NBT_createIntArrayTag(): array must not be null when count != 0.");
    }
    TagIntArray *r=nalloc(TagIntArray);
    r->name=copystr(name);
    r->type=11;
    r->size=count;
    if(count>0){
        r->arr=copymem(array,sizeof(int)*count);
    }else{
        r->arr=NULL;
    }
    return r;
}
int NBT_isIntArray(NBTTag *tag){
    nonnull(tag,0,"NBT_isIntArray(): tag must not be null.");
    return tag->type==11;
}
int *NBT_getIntArray(TagIntArray *tag,int *count){
    if(tag->size){
        *count=tag->size;
        return copymem(tag->arr,sizeof(int)*tag->size);
    }else{
        *count=0;
        return NULL;
    }
}
void NBT_setIntArray(TagIntArray *tag,int count,const int *array){
    if(tag->arr)
        free(tag->arr);
    if(count){
        tag->arr=copymem(array,count*sizeof(int));
    }else{
        tag->arr=NULL;
    }
    tag->size=count;
}
int NBT_getIntArrayIndex(TagIntArray *tag,int index){
    if(index>tag->size){
    	fprintf(stderr,"NBT_getIntArrayIndex(): the index %d is larger than the size %d.",index,tag->size);
        crash();
    }
    return tag->arr[index];
}
int NBT_getIntArraySize(TagIntArray *tag){
    return tag->size;
}
NBTErr NBT_writeIntArray(TagIntArray *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeIntArray(): stream must not be null.");
    int size=tag->size;
    int *arr=tag->arr;
    NBTErr err=DNwrite(size,stream);
    if(err){
        return err;
    }
    for(int i=0;i<size;i++){
        if(err=DNwrite(arr[i],stream)){
            return err;
        }
    }
    return NBT_NOERR;
}
TagIntArray *NBT_readIntArray(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readIntArray(): name must not be null.");
    nonnull(stream,NULL,"NBT_readIntArray(): stream must not be null.");
    int size;
    NBTErr err=DNread(size,stream);
    if(err){
        return NULL;
    }
    int *buf;
    if(size>0)
        buf=malloc(sizeof(int)*size);
    else
        buf=NULL;
    for(int i=0;i<size;i++){
        if(err=DNread(buf[i],stream)){
            free(buf);
            return NULL;
        }
    }
    TagIntArray *tag=nalloc(TagIntArray);
    tag->name=copystr(name);
    tag->type=11;
    tag->size=size;
    tag->arr=buf;
    return tag;
}
TagIntArray *NBT_copyIntArray(TagIntArray *tag){
    return NBT_createIntArrayTag(tag->name,tag->size,tag->arr);
}
void NBT_destroyIntArray(TagIntArray *tag){
    free(tag->name);
    if(tag->size>0){
        free(tag->arr);
    }
    free(tag);
}
typedef struct lonarrt_t{
    char *name;
    NBTTagType type;
    int size;
    long long *arr;
}TagLongArray;
TagLongArray *NBT_createLongArrayTag(const char *name,int count,const long long *array){
    nonnull(name,NULL,"NBT_createLongArrayTag(): name must not be null.");
    if(count>0){
        nonnull(array,NULL,"NBT_createLongArrayTag(): array must not be null when count != 0.");
    }
    TagLongArray *r=nalloc(TagLongArray);
    r->name=copystr(name);
    r->type=12;
    r->size=count;
    if(count>0){
        r->arr=copymem(array,sizeof(long long)*count);
    }else{
        r->arr=NULL;
    }
    return r;
}
int NBT_isLongArray(NBTTag *tag){
    nonnull(tag,0,"NBT_isLongArray(): tag must not be null.");
    return tag->type==12;
}
long long *NBT_getLongArray(TagLongArray *tag,int *count){
    if(tag->size){
        *count=tag->size;
        return copymem(tag->arr,sizeof(long long)*tag->size);
    }else{
        *count=0;
        return NULL;
    }
}
void NBT_setLongArray(TagLongArray *tag,int count,const long long *array){
    if(tag->arr)
        free(tag->arr);
    if(count){
        tag->arr=copymem(array,count*sizeof(long long));
    }else{
        tag->arr=NULL;
    }
    tag->size=count;
}
long long NBT_getLongArrayIndex(TagLongArray *tag,int index){
    if(index>tag->size){
    	fprintf(stderr,"NBT_getLongArrayIndex(): the index %d is larger than the size %d.",index,tag->size);
        crash();
    }
    return tag->arr[index];
}
int NBT_getLongArraySize(TagLongArray *tag){
    return tag->size;
}
NBTErr NBT_writeLongArray(TagLongArray *tag,FILE *stream){
    nonnull(stream,NBT_NULLPTR,"NBT_writeLongArray(): stream must not be null.");
    int size=tag->size;
    long long *arr=tag->arr;
    NBTErr err=DNwrite(size,stream);
    if(err){
        return err;
    }
    for(int i=0;i<size;i++){
        if(err=DNwrite(arr[i],stream)){
            return err;
        }
    }
    return NBT_NOERR;
}
TagLongArray *NBT_readLongArray(const char *name,FILE *stream){
    nonnull(name,NULL,"NBT_readLongArray(): name must not be null.");
    nonnull(stream,NULL,"NBT_readLongArray(): stream must not be null.");
    int size;
    NBTErr err=DNread(size,stream);
    if(err){
        return NULL;
    }
    long long *buf;
    if(size>0)
        buf=malloc(sizeof(long long)*size);
    else
        buf=NULL;
    for(int i=0;i<size;i++){
        if(err=DNread(buf[i],stream)){
            return NULL;
        }
    }
    TagLongArray *tag=nalloc(TagLongArray);
    tag->name=copystr(name);
    tag->type=12;
    tag->size=size;
    tag->arr=buf;
    return tag;
}
TagLongArray *NBT_copyLongArray(TagLongArray *tag){
    return NBT_createLongArrayTag(tag->name,tag->size,tag->arr);
}
void NBT_destroyLongArray(TagLongArray *tag){
    free(tag->name);
    if(tag->size>0){
        free(tag->arr);
    }
    free(tag);
}

NBTTagType NBT_getType(NBTTag *tag){
    nonnull(tag,TAG_END,"NBT_getType(): tag must not be null.");
    return tag->type;
}
NBTErr NBT_write(NBTTag *tag,FILE *stream){
    if(tag){
        NBTErr err=NBT_writeHead(tag,stream);
        return err ? err:NBT_writeBody(tag,stream);
    }else{
        return DNwrite((byte)0,stream);
    }
}
NBTErr NBT_writeHead(NBTTag *tag,FILE *stream){
    if(tag==NULL){
        return DNwrite((byte)0,stream);
    }else{
        NBTErr err=DNwrite((byte)tag->type,stream);
        return err ? err:DSwrite(tag->name,stream);
    }
}
NBTErr NBT_writeBody(NBTTag *tag,FILE *stream){
#define tp(id,name) case id:\
    return NBT_write##name((void *)tag,stream);\
    break
    switch(tag->type){
        tp(1,Byte);
        tp(2,Short);
        tp(3,Int);
        tp(4,Long);
        tp(5,Float);
        tp(6,Double);
        tp(7,ByteArray);
        tp(8,String);
        tp(9,List);
        tp(10,Compound);
        tp(11,IntArray);
        tp(12,LongArray);
        default:
            reportError(NBT_ILLEGAL,"NBT_writeBody(): %p is not a nbt tag",(void *)tag);
            return NBT_ILLEGAL;
    }
#undef tp
}
NBTTag *NBT_read(FILE *stream){
    char *name;
    NBTTagType type;
    name=NBT_readHead(&type,stream);
    if(name&&type){
        return (NBTTag *)NBT_readBody(name,type,stream);
    }else{
        return NULL;
    }
}
char *NBT_readHead(NBTTagType *type,FILE *stream){
    byte t;
    NBTErr err=DNread(t,stream);
    if(err){
        return NULL;
    }
    if(t==0){
    	*type=0;
    	return NULL;
    }
    *type=t;
    int count;
    return DSread(&count,stream);
}
NBTTag *NBT_readBody(const char *name,NBTTagType type,FILE *stream){
#define tp(t,n) case t: return (NBTTag *)NBT_read##n(name,stream)
    switch(type){
        tp(1,Byte);
        tp(2,Short);
        tp(3,Int);
        tp(4,Long);
        tp(5,Float);
        tp(6,Double);
        tp(7,ByteArray);
        tp(8,String);
        tp(9,List);
        tp(10,Compound);
        tp(11,IntArray);
        tp(12,LongArray);
        default:return NULL;
    }
#undef tp
}
NBTTag *NBT_copy(NBTTag *tag){
#define tp(t,name) case t: return (NBTTag *)NBT_copy##name((void *)tag)
    switch(tag->type){
        tp(1,Byte);
        tp(2,Short);
        tp(3,Int);
        tp(4,Long);
        tp(5,Float);
        tp(6,Double);
        tp(7,ByteArray);
        tp(8,String);
        tp(9,List);
        tp(10,Compound);
        tp(11,IntArray);
        tp(12,LongArray);
        default:return NULL;
    }
#undef tp
}
char *NBT_getName(NBTTag *tag){
    if(tag==NULL){
        return NULL;
    }else{
        return copystr(tag->name);
    }
}
void NBT_setName(NBTTag *tag,const char *name){
    free(tag->name);
    tag->name=copystr(name);
}
void NBT_destroy(NBTTag *tag){
#define tp(t,name) case t: NBT_destroy##name((void *)tag);break
    switch(tag->type){
        tp(1,Byte);
        tp(2,Short);
        tp(3,Int);
        tp(4,Long);
        tp(5,Float);
        tp(6,Double);
        tp(7,ByteArray);
        tp(8,String);
        tp(9,List);
        tp(10,Compound);
        tp(11,IntArray);
        tp(12,LongArray);
        default:return;
    }
#undef tp
}
void NBT_releaseArray(NBTTag **tags,int count){
	for(int i=0;i<count;i++){
		NBT_destroy(tags[i]);
	}
	free(tags);
}
void *__n_class_cast_to_type(void *obj,int type){
	if(!obj){
		return NULL;
	}
	if(((NBTTag *)obj)->type==type){
		return obj;
	}else{
		return NULL;
	}
}
static void printJSONStr(FILE *stream,char *c,int len){
    fputc('"',stream);
    for(int i=0;i<len;i++){
        char o=c[i];
        switch(o){
            case '\n':
                fprintf(stream,"\\n");
                break;
            case '\r':
                fprintf(stream,"\\r");
                break;
            case '\b':
                fprintf(stream,"\\b");
                break;
            case '\\':
                fprintf(stream,"\\\\");
                break;
            case '\t':
                fprintf(stream,"\\t");
                break;
            case '\'':
                fprintf(stream,"\\\'");
                break;
            case '"':
                fprintf(stream,"\\\"");
                break;
            default:
                fputc(o,stream);
        }
    }
    fputc('"',stream);
}
static void printStr(FILE *stream,char *c,int len){
    if(len==0){
        fprintf(stream,"\"\"");
        return;
    }
    for(int i=0;i<len;i++){
        int o=c[i];
        if(!((o>='a'&&o<='z')||(o>='A'&&o<='Z')||(o>='0'&&o<='9'))){
            printJSONStr(stream,c,len);
            return;
        }
    }
    if(c[0]>='0'&&c[0]<='9'){
        printJSONStr(stream,c,len);
        return;
    }else{
        fprintf(stream,"%s",c);
    }
}
static void printTab(FILE *stream,int deep){
    for(int i=0;i<deep;i++){
        fputc('\t',stream);
    }
}
static void printDouble(FILE *stream,double d){
    char out[1000];
    snprintf(out,999,"%.16lf",d);
    int i,finddot;
    for(i=0;out[i]&&(i<=16||(!finddot));i++){
        if(out[i]=='.'){
            finddot=1;
        }
    }
    out[i]=0;
    if(out[i-1]>'5'){
        int update=i-1;
        while(update!=0){
            int pre=update>1&&out[update-1]=='.' ? update-2 : update-1;
            out[update]='0';
            out[pre]++;
            if(out[pre]>'9'){
                update=pre;
            }else{
                break;
            }
        }
    }
    int lasteff=0;
    for(i=0;out[i];i++){
        if(out[i]!='0'||(i>0&&out[i-1]=='.')){
            lasteff=i;
        }
    }
    out[lasteff+1]=0;
    fprintf(stream,"%s",out);
}
static void printFloat(FILE *stream,float d){
    char out[1000];
    snprintf(out,999,"%.8f",d);
    int i,finddot;
    for(i=0;out[i]&&(i<=8||(!finddot));i++){
        if(out[i]=='.'){
            finddot=1;
        }
    }
    out[i]=0;
    if(out[i-1]>'5'){
        int update=i-1;
        while(update!=0){
            int pre=update>1&&out[update-1]=='.' ? update-2 : update-1;
            out[update]='0';
            out[pre]++;
            if(out[pre]>'9'){
                update=pre;
            }else{
                break;
            }
        }
    }
    int lasteff=0;
    for(i=0;out[i];i++){
        if(out[i]!='0'||(i>0&&out[i-1]=='.')){
            lasteff=i;
        }
    }
    out[lasteff+1]=0;
    fprintf(stream,"%s",out);
}
static void NBT_print_(NBTTag *tag,FILE *stream,int deep){
    int len,type,needl;
    char *c,**keys;
    struct compoundt_t *cpd;
    switch(tag->type){
        case TAG_BYTE:
            fprintf(stream,"%db",(int)(((struct bytet_t *)tag)->b));
            break;
        case TAG_SHORT:
            fprintf(stream,"%ds",(int)(((struct shortt_t *)tag)->s));
            break;
        case TAG_INT:
            fprintf(stream,"%d",(((struct intt_t *)tag)->i));
            break;
        case TAG_LONG:
            fprintf(stream,"%lldL",(long long)(((struct longt_t *)tag)->l));
            break;
        case TAG_FLOAT:
            printFloat(stream,(((struct floatt_t *)tag)->f));
            fputc('f',stream);
            break;
        case TAG_DOUBLE:
            printDouble(stream,(((struct doublet_t *)tag)->d));
            break;
        case TAG_BYTEARRAY:
            fprintf(stream,"[");
            len=NBT_getByteArraySize(NBT_BYTEARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    byte b=NBT_getByteArrayIndex(NBT_BYTEARRAY(tag),i);
                    fprintf(stream,"%db, ",(int)b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%db]",(int)NBT_getByteArrayIndex(NBT_BYTEARRAY(tag),len-1));
            }
            break;
        case TAG_STRING:
            c=NBT_STRING(tag)->str;
            len=NBT_STRING(tag)->len;
            printStr(stream,c,len);
            break;
        case TAG_LIST:
            type=NBT_LIST(tag)->list_type;
            needl= type==TAG_INT || type==TAG_LONG || type==TAG_BYTE;
            fprintf(stream,"[\n");
            printTab(stream,deep+1);
            len=NBT_getListSize(NBT_LIST(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    NBTTag *b=NBT_getListIndex(NBT_LIST(tag),i);
                    NBT_print_(b,stream,deep+1);
                    if((!(needl||type==TAG_SHORT||type==TAG_FLOAT||type==TAG_DOUBLE))||(i+1)%8==0){
                        fprintf(stream,",\n");
                        printTab(stream,deep+1);
                    }else{
                        fprintf(stream,", ");
                    }
                    NBT_destroy(b);
                }
                NBTTag *n=NBT_getListIndex(NBT_LIST(tag),len-1);
                NBT_print_(n,stream,deep+1);
                NBT_destroy(n);
            }
            printf("\n");
            printTab(stream,deep);
            if(needl){
                fprintf(stream,"] L");
            }else{
                fprintf(stream,"]");
            }
            break;
        case TAG_COMPOUND:
            fprintf(stream,"{");
            cpd=NBT_COMPOUND(tag);
            keys=NBT_getCompoundKeys(cpd,&len);
            fprintf(stream,"\n");
            for(int i=0;i<len;i++){
                NBTTag *b=NBT_getCompoundIndex(cpd,keys[i]);
                printTab(stream,deep+1);
                printStr(stream,keys[i],strlen(keys[i]));
                printf(": ");
                NBT_print_(b,stream,deep+1);
                NBT_destroy(b);
                if(i<len-1)
                    printf(",\n");
                else
                    printf("\n");
                free(keys[i]);
            }
            free(keys);
            printTab(stream,deep);
            fprintf(stream,"}");
            break;
        case TAG_INTARRAY:
            fprintf(stream,"[");
            len=NBT_getIntArraySize(NBT_INTARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    int b=NBT_getIntArrayIndex(NBT_INTARRAY(tag),i);
                    fprintf(stream,"%d, ",b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%d]",NBT_getIntArrayIndex(NBT_INTARRAY(tag),len-1));
            }
            break;
        case TAG_LONGARRAY:
            fprintf(stream,"[");
            len=NBT_getLongArraySize(NBT_LONGARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    long long b=NBT_getLongArrayIndex(NBT_LONGARRAY(tag),i);
                    fprintf(stream,"%lldL, ",b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%lldL]",NBT_getLongArrayIndex(NBT_LONGARRAY(tag),len-1));
            }
            break;
        case TAG_END:
            break;
    }
}
void NBT_print(NBTTag *tag,FILE *stream){
    NBT_print_(tag,stream,0);
}
static void NBT_printJSON_(NBTTag *tag,FILE *stream,int deep){
    int len,type,needl;
    char *c,**keys;
    struct compoundt_t *cpd;
    switch(tag->type){
        case TAG_BYTE:
            fprintf(stream,"%d",(int)(((struct bytet_t *)tag)->b));
            break;
        case TAG_SHORT:
            fprintf(stream,"%d",(int)(((struct shortt_t *)tag)->s));
            break;
        case TAG_INT:
            fprintf(stream,"%d",(((struct intt_t *)tag)->i));
            break;
        case TAG_LONG:
            fprintf(stream,"%lld",(long long)(((struct longt_t *)tag)->l));
            break;
        case TAG_FLOAT:
            printFloat(stream,(((struct floatt_t *)tag)->f));
            break;
        case TAG_DOUBLE:
            printDouble(stream,(((struct doublet_t *)tag)->d));
            break;
        case TAG_BYTEARRAY:
            fprintf(stream,"[");
            len=NBT_getByteArraySize(NBT_BYTEARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    byte b=NBT_getByteArrayIndex(NBT_BYTEARRAY(tag),i);
                    fprintf(stream,"%d, ",(int)b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%d]",(int)NBT_getByteArrayIndex(NBT_BYTEARRAY(tag),len-1));
            }
            break;
        case TAG_STRING:
            c=NBT_STRING(tag)->str;
            len=NBT_STRING(tag)->len;
            printJSONStr(stream,c,len);
            break;
        case TAG_LIST:
            type=NBT_LIST(tag)->list_type;
            needl= type==TAG_INT || type==TAG_LONG || type==TAG_BYTE;
            len=NBT_getListSize(NBT_LIST(tag));
            if(len==0){
                fprintf(stream,"[]");
                return;
            }else{
                fprintf(stream,"[\n");
                printTab(stream,deep+1);
                for(int i=0;i<len-1;i++){
                    NBTTag *b=NBT_getListIndex(NBT_LIST(tag),i);
                    NBT_printJSON_(b,stream,deep+1);
                    if((!(needl||type==TAG_SHORT||type==TAG_FLOAT||type==TAG_DOUBLE))||(i+1)%8==0){
                        fprintf(stream,",\n");
                        printTab(stream,deep+1);
                    }else{
                        fprintf(stream,", ");
                    }
                    NBT_destroy(b);
                }
                NBTTag *n=NBT_getListIndex(NBT_LIST(tag),len-1);
                NBT_printJSON_(n,stream,deep+1);
                NBT_destroy(n);
            }
            printf("\n");
            printTab(stream,deep);
            fprintf(stream,"]");
            break;
        case TAG_COMPOUND:
            fprintf(stream,"{");
            cpd=NBT_COMPOUND(tag);
            keys=NBT_getCompoundKeys(cpd,&len);
            if(len==0){
                fprintf(stream,"}");
                return;
            }
            fprintf(stream,"\n");
            for(int i=0;i<len;i++){
                NBTTag *b=NBT_getCompoundIndex(cpd,keys[i]);
                printTab(stream,deep+1);
                printJSONStr(stream,keys[i],strlen(keys[i]));
                printf(": ");
                NBT_printJSON_(b,stream,deep+1);
                NBT_destroy(b);
                if(i<len-1)
                    printf(",\n");
                else
                    printf("\n");
                free(keys[i]);
            }
            free(keys);
            printTab(stream,deep);
            fprintf(stream,"}");
            break;
        case TAG_INTARRAY:
            fprintf(stream,"[");
            len=NBT_getIntArraySize(NBT_INTARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    int b=NBT_getIntArrayIndex(NBT_INTARRAY(tag),i);
                    fprintf(stream,"%d, ",b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%d]",NBT_getIntArrayIndex(NBT_INTARRAY(tag),len-1));
            }
            break;
        case TAG_LONGARRAY:
            fprintf(stream,"[");
            len=NBT_getLongArraySize(NBT_LONGARRAY(tag));
            if(len==0){
                fprintf(stream,"]");
            }else{
                for(int i=0;i<len-1;i++){
                    long long b=NBT_getLongArrayIndex(NBT_LONGARRAY(tag),i);
                    fprintf(stream,"%lld, ",b);
                    if((i+1)%8==0){
                        printf("\n");
                        printTab(stream,deep+1);
                    }
                }
                fprintf(stream,"%lld]",NBT_getLongArrayIndex(NBT_LONGARRAY(tag),len-1));
            }
            break;
        case TAG_END:
            break;
    }
}
void NBT_printJSON(NBTTag *tag,FILE *stream){
    NBT_printJSON_(tag,stream,0);
}
NBTErr NBT_getLastError(){
    return lasterr;
}
void NBT_getErrorDetail(char *str,int len){
    strncpy(str,errdetail,len-1);
    int slen=strlen(errdetail);
    len=len>slen?slen:len-1;
    str[len]=0;
}

/*copy from gz.c*/
#define ArrayList(type) struct {int size;int alloced_length;size_t element_size;type *array;}
#define ArrayList_new(type) ((void *)({ArrayList(type) *__list=malloc(sizeof(ArrayList(type)));__list->size=0;__list->alloced_length=4;__list->element_size=sizeof(type);__list->array=malloc(sizeof(type)*4);__list;}))
#define ArrayList_foreach(list,varname,todo) for(int __ij=0;__ij<(list)->size;__ij++){typeof((list)->array[0]) varname=(list)->array[i];todo}
#define ArrayList_index(list,in) list->array[in]
#define ArrayList_size(list) list->size
#define ArrayList_add(list,obj) (list)->size++;if((list)->size>(list)->alloced_length){(list)->alloced_length*=2;(list)->array=realloc((list)->array,(list)->alloced_length*(list)->element_size);}(list)->array[(list)->size-1]=obj
#define ArrayList_delete(list) free((list)->array);free(list)

TagCompound *NBT_readFromFile(char *path){
	char array[512];
	ArrayList(char) *list=ArrayList_new(char);
	gzFile stream=gzopen(path,"r");
    if(!stream){
        reportError(NBT_IOERR,"Cannot read file `%s'",path);
        return NULL;
    }
	int i=0;
	while((i=gzread(stream,array,512))>0){
		for(int j=0;j<i;j++){
			ArrayList_add(list,array[j]);
		}
	}
	FILE *fp=fmemopen(list->array,list->size,"r");
    if(!fp){
        reportError(NBT_IOERR,"Cannot create memstream");
        return NULL;
    }
	TagCompound *cpd=NBT_COMPOUND(NBT_read(fp));
	fclose(fp);
	gzclose(stream);
	ArrayList_delete(list);
	return cpd;
}
NBTErr NBT_writeToFile(TagCompound *cpd,char *path){
	char *bytes=NULL;
	size_t size=0;
	FILE *stream=open_memstream(&bytes,&size);
    if(!stream){
        reportError(NBT_IOERR,"Cannot open mem output stream");
        return NBT_IOERR;
    }
	NBTErr err=NBT_write(NBT(cpd),stream);
    if(err){
        return err;
    }
	fclose(stream);
	gzFile fp=gzopen(path,"w");
    if(!fp){
        reportError(NBT_IOERR,"Cannot open file `%s'",path);
        return NBT_IOERR;
    }
	gzwrite(fp,bytes,size);
	gzclose(fp);
	free(bytes);
    return NBT_NOERR;
}
typedef enum WordType{
    WORD_CPD_START,//{
    WORD_CPD_END,//}
    WORD_LIST_START,//[
    WORD_LIST_END,//]
    WORD_COMMA,//,
    WORD_COLON,//:
    WORD_JSON_STRING,//"something"
    WORD_STRING,//something
    WORD_NUMBER//123
}WordType;
typedef struct Word{
    WordType type;
    char *raw;//raw word
    long long offset;

    /*STRING and JSON_STRING only*/
    char *stringValue;

    /*NUMBER only*/
    NBTTagType numberType;
    union{
        byte b;
        short s;
        int i;
        long long l;
        float f;
        double d;
    } numberValue;
} Word;
static Word *readWord(FILE *in){
    ArrayList(char) *rawbuf=ArrayList_new(char);
    int c;
    do{
        c=fgetc(in);
        if(c<0){
            reportError(NBT_IOERR,"Cannot scan nbt: fgetc() failed");
            ArrayList_delete(rawbuf);
            return NULL;
        }
    }while(c==' '||c=='\n'||c=='\r'||c=='\t');
    if(c=='"'){
        ArrayList_add(rawbuf,'"');
        ArrayList(char) *strbuf=ArrayList_new(char);
        c=fgetc(in);
        while(c!='"'){
            if(c<0){
                reportError(NBT_IOERR,"Cannot scan nbt: fgetc() failed");
                ArrayList_delete(rawbuf);
                ArrayList_delete(strbuf);
                return NULL;
            }
            ArrayList_add(rawbuf,(char)c);
            if(c=='\\'){
                c=fgetc(in);
                if(c<0){
                    reportError(NBT_IOERR,"Cannot scan nbt: fgetc() failed");
                    ArrayList_delete(rawbuf);
                    ArrayList_delete(strbuf);
                    return NULL;
                }
                ArrayList_add(rawbuf,(char)c);
                switch(c){
                    case '\\':
                        ArrayList_add(strbuf,'\\');
                        break;
                    case 'b':
                        ArrayList_add(strbuf,'\b');
                        break;
                    case 'n':
                        ArrayList_add(strbuf,'\n');
                        break;
                    case 'r':
                        ArrayList_add(strbuf,'\r');
                        break;
                    case 't':
                        ArrayList_add(strbuf,'\t');
                        break;
                    case '"':
                        ArrayList_add(strbuf,'\"');
                        break;
                    case '\'':
                        ArrayList_add(strbuf,'\'');
                        break;
                    case 'a':
                        ArrayList_add(strbuf,'\a');
                        break;
                }
            }else{
                ArrayList_add(strbuf,(char)c);
            }
            c=fgetc(in);
        }
        ArrayList_add(rawbuf,'"');
        ArrayList_add(rawbuf,'\0');
        ArrayList_add(strbuf,'\0');
        Word *rval=nalloc(Word);
        rval->raw=rawbuf->array;
        rval->type=WORD_JSON_STRING;
        rval->stringValue=strbuf->array;
        rval->offset=ftell(in);
        free(rawbuf);
        free(strbuf);
        return rval;
    }else if((c>='a'&&c<='z')||(c>='A'&&c<='Z')){
        ArrayList(char) *strbuf=ArrayList_new(char);
        while((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')){
            ArrayList_add(rawbuf,(char)c);
            ArrayList_add(strbuf,(char)c);
            c=fgetc(in);
            if(c<0){
                reportError(NBT_IOERR,"Cannot scan nbt: fgetc() failed");
                ArrayList_delete(rawbuf);
                ArrayList_delete(strbuf);
                return NULL;
            }
        }
        ungetc(c,in);
        Word *rval=nalloc(Word);
        ArrayList_add(rawbuf,'\0');
        ArrayList_add(strbuf,'\0');
        rval->type=WORD_STRING;
        rval->raw=rawbuf->array;
        rval->stringValue=strbuf->array;
        rval->offset=ftell(in);
        free(rawbuf);
        free(strbuf);
        return rval;
    }else if((c>='0'&&c<='9')||c=='-'){
        int hasdot=0;
        while(c=='e'||c=='E'||c=='-'||c=='+'||c=='.'||(c>='0'&&c<='9')){
            ArrayList_add(rawbuf,(char)c);
            c=fgetc(in);
            if(c<0){
                reportError(NBT_IOERR,"Cannot scan nbt: fgetc() failed");
                ArrayList_delete(rawbuf);
                return NULL;
            }
            if(c=='.'&&!hasdot){
                hasdot=1;
            }else if(c=='.'){
                reportError(NBT_ILLEGAL,"Cannot scan nbt: float should not has 2 '.'");
                ArrayList_delete(rawbuf);
                return NULL;
            }
        }
        ArrayList_add(rawbuf,'\0');
        Word *rval=nalloc(Word);
        rval->type=WORD_NUMBER;
        rval->offset=ftell(in);
        int t;
        switch(c){
            case 'L':
            case 'l':
                rval->numberType=TAG_LONG;
                sscanf(rawbuf->array,"%lld",&(rval->numberValue.l));
                ArrayList_index(rawbuf,ArrayList_size(rawbuf)-1)=(char)c;
                ArrayList_add(rawbuf,'\0');
                break;
            case 's':
            case 'S':
                rval->numberType=TAG_SHORT;
                sscanf(rawbuf->array,"%d",&t);
                rval->numberValue.s=(short)t;
                ArrayList_index(rawbuf,ArrayList_size(rawbuf)-1)=(char)c;
                ArrayList_add(rawbuf,'\0');
                break;
            case 'b':
            case 'B':
                rval->numberType=TAG_BYTE;
                sscanf(rawbuf->array,"%d",&t);
                rval->numberValue.b=(byte)t;
                ArrayList_index(rawbuf,ArrayList_size(rawbuf)-1)=(char)c;
                ArrayList_add(rawbuf,'\0');
                break;
            case 'f':
            case 'F':
                rval->numberType=TAG_FLOAT;
                sscanf(rawbuf->array,"%f",&(rval->numberValue.f));
                ArrayList_index(rawbuf,ArrayList_size(rawbuf)-1)=(char)c;
                ArrayList_add(rawbuf,'\0');
                break;
            case 'd':
            case 'D':
                rval->numberType=TAG_DOUBLE;
                sscanf(rawbuf->array,"%lf",&(rval->numberValue.d));
                ArrayList_index(rawbuf,ArrayList_size(rawbuf)-1)=(char)c;
                ArrayList_add(rawbuf,'\0');
                break;
            default:
                if(hasdot){
                    rval->numberType=TAG_DOUBLE;
                    sscanf(rawbuf->array,"%lf",&(rval->numberValue.d));
                    ungetc(c,in);
                }else{
                    rval->numberType=TAG_INT;
                    sscanf(rawbuf->array,"%d",&(rval->numberValue.i));
                    ungetc(c,in);
                }
        }
        rval->raw=rawbuf->array;
        free(rawbuf);
        return rval;
    }else if(c=='{'){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_CPD_START;
        rval->raw=copystr("{");
        rval->offset=ftell(in);
        return rval;
    }else if(c=='}'){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_CPD_END;
        rval->raw=copystr("}");
        rval->offset=ftell(in);
        return rval;
    }else if(c==','){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_COMMA;
        rval->raw=copystr(",");
        rval->offset=ftell(in);
        return rval;
    }else if(c==':'){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_COLON;
        rval->raw=copystr(":");
        rval->offset=ftell(in);
        return rval;
    }else if(c=='['){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_LIST_START;
        rval->raw=copystr("]");
        rval->offset=ftell(in);
        return rval;
    }else if(c==']'){
        ArrayList_delete(rawbuf);
        Word *rval=nalloc(Word);
        rval->type=WORD_LIST_END;
        rval->raw=copystr("]");
        rval->offset=ftell(in);
        return rval;
    }else{
        ArrayList_delete(rawbuf);
        reportError(NBT_ILLEGAL,"Illegal NBT text format at %lld",(long long)ftell(in));
        return NULL;
    }
}
static void destroyWord(Word *w){
    if(w->type==WORD_STRING||w->type==WORD_JSON_STRING){
        free(w->stringValue);
    }
    free(w->raw);
    free(w);
}
typedef struct WordInputStream{
    FILE *readStream;
    Word *unget;
}WordInputStream;
static WordInputStream createWordIstream(FILE *in){
    return (WordInputStream){in,NULL};
}
static Word *getWord(WordInputStream *stream){
    if(stream->unget){
        Word *rval=stream->unget;
        stream->unget=NULL;
        return rval;
    }else{
        return readWord(stream->readStream);
    }
}
static void ungetWord(Word *word,WordInputStream *stream){
    stream->unget=word;
}
static NBTTag *readTag(WordInputStream *stream,char *name){
    Word *start=getWord(stream);
    if(!start){
        return NULL;
    }
    NBTTag *rval;
    ArrayList(NBTTag *) *tags;
    switch(start->type){
        case WORD_STRING:
        case WORD_JSON_STRING:
            rval=NBT(NBT_createStringTag(name,start->stringValue));
            destroyWord(start);
            break;
        case WORD_NUMBER:
            switch(start->numberType){
                case TAG_BYTE:
                    rval=NBT(NBT_createByteTag(name,start->numberValue.b));
                    break;
                case TAG_SHORT:
                    rval=NBT(NBT_createShortTag(name,start->numberValue.s));
                    break;
                case TAG_INT:
                    rval=NBT(NBT_createIntTag(name,start->numberValue.i));
                    break;
                case TAG_LONG:
                    rval=NBT(NBT_createLongTag(name,start->numberValue.l));
                    break;
                case TAG_FLOAT:
                    rval=NBT(NBT_createFloatTag(name,start->numberValue.f));
                    break;
                case TAG_DOUBLE:
                    rval=NBT(NBT_createDoubleTag(name,start->numberValue.d));
                    break;
                default://nerver happen, stop gcc from warning
                    break;
            }
            destroyWord(start);
            break;
        case WORD_LIST_START:
            tags=ArrayList_new(NBTTag *);
            while(start->type!=WORD_LIST_END){
                Word *w=getWord(stream);
                if(!w){
                    reportError(NBT_IOERR,"Cannot read word");
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    destroyWord(start);
                    ArrayList_delete(tags);
                }
                if(w->type==WORD_LIST_END){
                    destroyWord(start);
                    start=w;
                    continue;
                }else{
                    ungetWord(w,stream);
                }
                NBTTag *n=readTag(stream,"");
                if(!n){
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    ArrayList_delete(tags);
                    destroyWord(start);
                    return NULL;
                }
                ArrayList_add(tags,n);
                destroyWord(start);
                start=getWord(stream);
                if(!start){
                    reportError(NBT_IOERR,"Cannot read Word");
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    ArrayList_delete(tags);
                    return NULL;
                }
                if(start->type!=WORD_COMMA&&start->type!=WORD_LIST_END){
                    warn("Use `,' in list");
                    reportError(NBT_IOERR,"Use `,' in list @offset=%lld",start->offset);
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    ArrayList_delete(tags);
                    destroyWord(start);
                    return NULL;
                }
            }
            int isList=0;
            destroyWord(start);
            start=getWord(stream);
            if(!start){
                reportError(NBT_IOERR,"Cannot read Word");
                for(int i=0;i<ArrayList_size(tags);i++){
                    NBT_destroy(ArrayList_index(tags,i));
                }
                ArrayList_delete(tags);
                 return NULL;
            }
            if(start->type==WORD_STRING&&!strcmp(start->stringValue,"L")){
                isList=1;
                destroyWord(start);
            }else{
                ungetWord(start,stream);
            }
            if(ArrayList_size(tags)==0||(!(NBT_isByte(ArrayList_index(tags,0))||
                NBT_isInt(ArrayList_index(tags,0))||NBT_isLong(ArrayList_index(tags,0))))){
                isList=1;
            }
            if(isList){
                rval=NBT(NBT_createListTag(name,ArrayList_size(tags),tags->array));
            }else if(NBT_isByte(ArrayList_index(tags,0))){
                byte b[ArrayList_size(tags)];
                for(int i=0;i<ArrayList_size(tags);i++){
                    b[i]=NBT_getByte(NBT_BYTE(ArrayList_index(tags,i)));
                }
                rval=NBT(NBT_createByteArrayTag(name,ArrayList_size(tags),b));
            }else if(NBT_isInt(ArrayList_index(tags,0))){
                int b[ArrayList_size(tags)];
                for(int i=0;i<ArrayList_size(tags);i++){
                    b[i]=NBT_getInt(NBT_INT(ArrayList_index(tags,i)));
                }
                rval=NBT(NBT_createIntArrayTag(name,ArrayList_size(tags),b));
            }else if(NBT_isLong(ArrayList_index(tags,0))){
                long long b[ArrayList_size(tags)];
                for(int i=0;i<ArrayList_size(tags);i++){
                    b[i]=NBT_getLong(NBT_LONG(ArrayList_index(tags,i)));
                }
                rval=NBT(NBT_createLongArrayTag(name,ArrayList_size(tags),b));
            }
            for(int i=0;i<ArrayList_size(tags);i++){
                NBT_destroy(ArrayList_index(tags,i));
            }
            ArrayList_delete(tags);
            break;
        case WORD_CPD_START:
            tags=ArrayList_new(NBTTag *);
            while(start->type!=WORD_CPD_END){
                Word *key=getWord(stream);
                if(!key){
                    reportError(NBT_IOERR,"Cannot read word");
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    destroyWord(start);
                    ArrayList_delete(tags);
                }
                if(key->type==WORD_CPD_END){
                    destroyWord(start);
                    start=key;
                    continue;
                }
                Word *colon=getWord(stream);
                if((!key)||(!colon)){
                    destroyWord(start);
                    if(key)
                        destroyWord(key);
                    if(colon)
                        destroyWord(colon);
                    return NULL;
                }
                if((!(key->type==WORD_STRING||key->type==WORD_NUMBER||key->type==WORD_JSON_STRING))||
                    (colon->type!=WORD_COLON)){
                    destroyWord(start);
                    destroyWord(key);
                    destroyWord(colon);
                    reportError(NBT_ILLEGAL,"Wrong NBT text");
                    return NULL;
                }
                NBTTag *n=readTag(stream,key->type==WORD_JSON_STRING?key->stringValue:key->raw);
                destroyWord(colon);
                destroyWord(key);
                if(!n){
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    ArrayList_delete(tags);
                    destroyWord(start);
                    return NULL;
                }
                ArrayList_add(tags,n);
                destroyWord(start);
                start=getWord(stream);
                if(!start){
                    reportError(NBT_IOERR,"Cannot read Word");
                    for(int i=0;i<ArrayList_size(tags);i++){
                        NBT_destroy(ArrayList_index(tags,i));
                    }
                    ArrayList_delete(tags);
                    return NULL;
                }
                if(start->type!=WORD_COMMA&&start->type!=WORD_CPD_END){
                    warn("Use `,' in Compound");
                }
            }
            rval=NBT(NBT_createCompoundTag(name,ArrayList_size(tags),tags->array));
            for(int i=0;i<ArrayList_size(tags);i++){
                NBT_destroy(ArrayList_index(tags,i));
            }
            ArrayList_delete(tags);
            destroyWord(start);
            break;
        default:
            reportError(NBT_ILLEGAL,"unexpected word @%lld",start->offset);
            destroyWord(start);
            return NULL;
    }
    return rval;
}
NBTTag *NBT_scan(FILE *in){
    WordInputStream stream=createWordIstream(in);
    return readTag(&stream,"");
}
void NBT_vserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,va_list options){
    if(interface->nbtType!=tag->type)
        abort();
    interface->toNBT(object,tag,interface,options);
}
void NBT_vdeserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,va_list options){
    if(interface->nbtType!=tag->type)
        abort();
    interface->fromNBT(object,tag,interface,options);
}
void NBT_serialize(void *object,NBTTag *tag,NBTBaseInterface *interface,...){
    va_list ap;
    va_start(ap,interface);
    NBT_vserialize(object,tag,interface,ap);
    va_end(ap);
}
void NBT_deserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,...){
    va_list ap;
    va_start(ap,interface);
    NBT_vdeserialize(object,tag,interface,ap);
    va_end(ap);
}
NBTTag *NBT_allocTagByInterface(NBTBaseInterface *interface){
    switch(interface->nbtType){
        case TAG_END:
            return NULL;
        case TAG_BYTE:
            return (NBTTag *)NBT_createByteTag("",0);
        case TAG_SHORT:
            return (NBTTag *)NBT_createShortTag("",0);
        case TAG_INT:
            return (NBTTag *)NBT_createIntTag("",0);
        case TAG_LONG:
            return (NBTTag *)NBT_createLongTag("",0);
        case TAG_FLOAT:
            return (NBTTag *)NBT_createFloatTag("",0);
        case TAG_DOUBLE:
            return (NBTTag *)NBT_createDoubleTag("",0);
        case TAG_BYTEARRAY:
            return (NBTTag *)NBT_createByteArrayTag("",0,NULL);
        case TAG_STRING:
            return (NBTTag *)NBT_createStringTag("","");
        case TAG_LIST:
            return (NBTTag *)NBT_createListTag("",0,NULL);
        case TAG_COMPOUND:
            return (NBTTag *)NBT_createCompoundTag("",0,NULL);
        case TAG_INTARRAY:
            return (NBTTag *)NBT_createIntArrayTag("",0,NULL);
        case TAG_LONGARRAY:
            return (NBTTag *)NBT_createLongArrayTag("",0,NULL);
    }
    abort();//control never reaches here because NBTTagType is enum
}
static void UUID_TO_NBT(long long uuid_in[2],TagCompound *cpd,...){//varargs are ignored
    NBT_setCompoundLong(cpd,"UUIDMost",uuid_in[0]);
    NBT_setCompoundLong(cpd,"UUIDLeast",uuid_in[1]);
}
static void UUID_FROM_NBT(long long uuid_in[2],TagCompound *cpd,...){
    NBTTag *most=NBT_getCompoundIndex(cpd,"UUIDMost");
    NBTTag *least=NBT_getCompoundIndex(cpd,"UUIDLeast");
    if(NBT_LONG(most)&&NBT_LONG(least)){
        uuid_in[0]=((TagLong *)most)->l;
        uuid_in[1]=((TagLong *)least)->l;
    }
    if(most)
        NBT_destroy(most);
    if(least)
        NBT_destroy(least);
}
NBTBaseInterface *NBT_BASE_UUID=&((NBTBaseInterface){
    .nbtType=TAG_COMPOUND,
    .toNBT=(void *)UUID_TO_NBT,
    .fromNBT=(void *)UUID_FROM_NBT
});
static void BOOL_TO_NBT(_Bool *bool_in,TagByte *byt,...){
    NBT_setByte(byt,(char)*bool_in);
}
static void BOOL_FROM_NBT(_Bool *bool_in,TagByte *byt,...){
    *bool_in=NBT_getByte(byt);
}
NBTBaseInterface *NBT_BASE_BOOL=&((NBTBaseInterface){
    .nbtType=TAG_BYTE,
    .toNBT=(void *)BOOL_TO_NBT,
    .fromNBT=(void *)BOOL_FROM_NBT
});
static void ENUM_TO_NBT(int *enum_in,TagString *str,NBTBaseInterface *interface,va_list options){
    char **enum_str=va_arg(options,char**);
    NBT_setString(str,enum_str[*enum_in]);
}
static void ENUM_FROM_NBT(int *enum_in,TagString *str,NBTBaseInterface *interface,va_list options){
    char **enum_str=va_arg(options,char**);
    int len;
    char *cstr=NBT_getString(str,&len);
    for(int i=0;enum_str[i];i++){
        if(!strcmp(enum_str[i],cstr)){
            *enum_in=i;
            break;
        }
    }
    free(cstr);
}
NBTBaseInterface *NBT_BASE_ENUM=&((NBTBaseInterface){
    .nbtType=TAG_STRING,
    .toNBT=(void *)ENUM_TO_NBT,
    .fromNBT=(void *)ENUM_FROM_NBT
});