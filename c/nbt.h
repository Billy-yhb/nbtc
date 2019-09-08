#ifndef INCLUDE_NBT__
#define INCLUDE_NBT__
#include <stdio.h>
#ifdef __cplusplus
/*There is some problem in Cpp, so I use this to stop cpp*/
#error "The nbtc library is a c library (standard>=c11 for type-safety cast) and does not support c++"
#endif
#include "hashmap.h"
/*
 * 依据NBT 19133标准编写
 * 字节序:大端序
 * 
 * Conforming to: NBT19133
 * 		(Big-Endian)
 * */
#ifndef byte
#define byte char
#endif
typedef struct NBTTAG NBTTAG;
typedef enum TAGTYPE{
    TAG_END=0,
    TAG_BYTE=1,
    TAG_SHORT=2,
    TAG_INT=3,
    TAG_LONG=4,
    TAG_FLOAT=5,
    TAG_DOUBLE=6,
    TAG_BYTEARRAY=7,
    TAG_STRING=8,
    TAG_LIST=9,
    TAG_COMPOUND=10,
    TAG_INTARRAY=11,
    TAG_LONGARRAY=12
}NBTTAGType;
typedef enum NBTErr{
	NBT_NOERR,//无错  no error
	NBT_IOERR,//输入输出错误  IO error
	NBT_ILLEGAL,//传入的NBT对象错误，例如List含有不同类型  wrong NBT data eg. diffent tag type in a list
	NBT_NULLPTR//接受空指针  get a NULL pointer
}NBTErr;
typedef struct bytet_t TagByte;
typedef struct shortt_t TagShort;
typedef struct intt_t TagInt;
typedef struct longt_t TagLong;
typedef struct floatt_t TagFloat;
typedef struct doublet_t TagDouble;
typedef struct bytarrt_t TagByteArray;
typedef struct strt_t TagString;
typedef struct listt_t TagList;
typedef struct compoundt_t TagCompound;
typedef struct intarrt_t TagIntArray;
typedef struct lonarrt_t TagLongArray;

/* 创建一个TAG_BYTE的nbt节点
 * 参数
 *     name:
 *       标签名称
 *     b:
 *       保存的值
 * Create a byte tag
 * Arguments:
 * 		name:
 * 			the name of the tag.
 * 		b:
 * 			the byte*/
TagByte *NBT_createByteTag(const char *name,byte b);
/* 判断一个NBT节点是否为TAG_BYTE类型
 * Judge if the tag is byte*/
int NBT_isByte(NBTTAG *tag);
/* 返回所保存的b值
 * Return the storged b*/
byte NBT_getByte(TagByte *tag);
/* 设置保存的b
 * Set b*/
void NBT_setByte(TagByte *tag,byte b);
/* 将tag以二进制形式存储(不含标签名和ID)
 * Serialize a tag to stream as NBT binary format (excude the name and the type)*/
NBTErr NBT_writeByte(TagByte *tag,FILE *stream);
/* 从二进制流中读取一个TAG_BYTE数据(不含标签名和ID)
 * Deserialize from binary stream to a tag (excude the name and the type)*/
TagByte *NBT_readByte(const char *name,FILE *stream);
/* 复制一个TAG_BYTE
 * Copy a byte tag*/
TagByte *NBT_copyByte(TagByte *tag);
/* 释放这个TAG_BYTE占用的资源
 * Release this byte tag*/
void NBT_destroyByte(TagByte *tag);


TagShort *NBT_createShortTag(const char *name,short s);
int NBT_isShort(NBTTAG *tag);
short NBT_getShort(TagShort *tag);
void NBT_setShort(TagShort *tag,short s);
NBTErr NBT_writeShort(TagShort *tag,FILE *stream);
TagShort *NBT_readShort(const char *name,FILE *stream);
TagShort *NBT_copyShort(TagShort *tag);
void NBT_destroyShort(TagShort *tag);


TagInt *NBT_createIntTag(const char *name,int i);
int NBT_isInt(NBTTAG *tag);
int NBT_getInt(TagInt *tag);
void NBT_setInt(TagInt *tag,int i);
NBTErr NBT_writeInt(TagInt *tag,FILE *stream);
TagInt *NBT_readInt(const char *name,FILE *stream);
TagInt *NBT_copyInt(TagInt *tag);
void NBT_destroyInt(TagInt *tag);


TagLong *NBT_createLongTag(const char *name,long long l);
int NBT_isLong(NBTTAG *tag);
long long NBT_getLong(TagLong *tag);
void NBT_setLong(TagLong *tag,long long l);
NBTErr NBT_writeLong(TagLong *tag,FILE *stream);
TagLong *NBT_readLong(const char *name,FILE *stream);
TagLong *NBT_copyLong(TagLong *tag);
void NBT_destroyLong(TagLong *tag);


TagFloat *NBT_createFloatTag(const char *name,float f);
int NBT_isFloat(NBTTAG *tag);
float NBT_getFloat(TagFloat *tag);
void NBT_setFloat(TagFloat *tag,float f);
NBTErr NBT_writeFloat(TagFloat *tag,FILE *stream);
TagFloat *NBT_readFloat(const char *name,FILE *stream);
TagFloat *NBT_copyFloat(TagFloat *tag);
void NBT_destroyFloat(TagFloat *tag);


TagDouble *NBT_createDoubleTag(const char *name,double d);
int NBT_isDouble(NBTTAG *tag);
double NBT_getDouble(TagDouble *tag);
void NBT_setDouble(TagDouble *tag,double d);
NBTErr NBT_writeDouble(TagDouble *tag,FILE *stream);
TagDouble *NBT_readDouble(const char *name,FILE *stream);
TagDouble *NBT_copyDouble(TagDouble *tag);
void NBT_destroyDouble(TagDouble *tag);


/* 创建一个count长度的byte数组(array)
 * Create a byte array tag from the array pointer and the length is count*/
TagByteArray *NBT_createByteArrayTag(const char *name,int count,const byte *array);
int NBT_isByteArray(NBTTAG *tag);
/* 获取整个byte数组(需自行free)
 * 参数:
 *     count  返回实际的byte数组长度
 * 返回:
 *     一个malloc得到的byte数组(另外的,修改不会引起TAG值改变)
 * Get the whole byte array (free() by yourself)
 * 		The length of the array will be set to the *count
 * 		The byte array will return
 * */
byte *NBT_getByteArray(TagByteArray *tag,int *count);
void NBT_setByteArray(TagByteArray *tag,int count,const byte *array);
byte NBT_getByteArrayIndex(TagByteArray *tag,int index);
int NBT_getByteArraySize(TagByteArray *tag);
NBTErr NBT_writeByteArray(TagByteArray *tag,FILE *stream);
TagByteArray *NBT_readByteArray(const char *name,FILE *stream);
TagByteArray *NBT_copyByteArray(TagByteArray *tag);
void NBT_destroyByteArray(TagByteArray *tag);


TagString *NBT_createStringTag(const char *name,const char *value);
int NBT_isString(NBTTAG *tag);
char *NBT_getString(TagString *tag,int *count);
void NBT_setString(TagString *tag,const char *str);
int NBT_getStringSize(TagString *tag);
NBTErr NBT_writeString(TagString *tag,FILE *stream);
TagString *NBT_readString(const char *name,FILE *stream);
TagString *NBT_copyString(TagString *tag);
void NBT_destroyString(TagString *tag);


/* 创建一个count长度的List,元素必须为相同类型
 * Create a list tag, all the elements must be same type*/
TagList *NBT_createListTag(const char *name,int count,NBTTAG **array);
int NBT_isList(NBTTAG *tag);
NBTTAG **NBT_getList(TagList *tag,int *count);
void NBT_setList(TagList *tag,int count,NBTTAG **array);
NBTTAG *NBT_getListIndex(TagList *tag,int index);
int NBT_getListSize(TagList *tag);
NBTTAGType NBT_getListType(TagList *tag);
NBTErr NBT_writeList(TagList *tag,FILE *stream);
TagList *NBT_readList(const char *name,FILE *stream);
TagList *NBT_copyList(TagList *tag);
void NBT_destroyList(TagList *tag);


/* 创建一个size长度的Compound,元素必须无重名
 * Create a compound tag with size elements and all the element must have different name*/
TagCompound *NBT_createCompoundTag(const char *name,int size,NBTTAG **array);
int NBT_isCompound(NBTTAG *tag);
HashMap(char *,TAG *) *NBT_getCompound(TagCompound *tag);
void NBT_setCompound(TagCompound *tag,HashMap(char *,TAG *) *map);
NBTTAG *NBT_getCompoundIndex(TagCompound *tag,const char *index);
void NBT_setCompoundIndex(TagCompound *tag,NBTTAG *ele);
void NBT_removeCompoundIndex(TagCompound *tag,const char *index);
void NBT_setCompoundByte(TagCompound *tag,const char *index,byte value);
void NBT_setCompoundShort(TagCompound *tag,const char *index,short value);
void NBT_setCompoundInt(TagCompound *tag,const char *index,int value);
void NBT_setCompoundLong(TagCompound *tag,const char *index,long long value);
void NBT_setCompoundFloat(TagCompound *tag,const char *index,float value);
void NBT_setCompoundDouble(TagCompound *tag,const char *index,double value);
void NBT_setCompoundString(TagCompound *tag,const char *index,const char *value);
/* 若无法取到值，则返回fallback,字符串返回复制后的值
 * If failed to get the value, fallback will be returned but if fallback is a string (const char *),
 * 		copied value will be returned*/
byte NBT_getCompoundByte(TagCompound *tag,const char *index,byte fallback);
short NBT_getCompoundShort(TagCompound *tag,const char *index,short fallback);
int NBT_getCompoundInt(TagCompound *tag,const char *index,int fallback);
long long NBT_getCompoundLong(TagCompound *tag,const char *index,long long fallback);
float NBT_getCompoundFloat(TagCompound *tag,const char *index,float fallback);
double NBT_getCompoundDouble(TagCompound *tag,const char *index,double fallback);
char *NBT_getCompoundString(TagCompound *tag,const char *index,int *count,const char *fallback);
void NBT_clearCompoundMap(TagCompound *tag);
int NBT_getCompoundSize(TagCompound *tag);
char **NBT_getCompoundKeys(TagCompound *tag,int *count);
NBTTAGType NBT_getCompoundIndexType(TagCompound *tag,const char *index);
NBTErr NBT_writeCompound(TagCompound *tag,FILE *stream);
TagCompound *NBT_readCompound(const char *name,FILE *stream);
TagCompound *NBT_copyCompound(TagCompound *tag);
void NBT_destroyCompound(TagCompound *tag);


TagIntArray *NBT_createIntArrayTag(const char *name,int size,const int *array);
int NBT_isIntArrayTag(NBTTAG *tag);
int *NBT_getIntArray(TagIntArray *tag,int *count);
void NBT_setIntArray(TagIntArray *tag,int count,const int *array);
int NBT_getIntArrayIndex(TagIntArray *tag,int index);
int NBT_getIntArraySize(TagIntArray *tag);
NBTErr NBT_writeIntArray(TagIntArray *tag,FILE *stream);
TagIntArray *NBT_readIntArray(const char *name,FILE *stream);
TagIntArray *NBT_copyIntArray(TagIntArray *tag);
void NBT_destroyIntArray(TagIntArray *tag);


TagLongArray *NBT_createLongArrayTag(const char *name,int size,const long long *array);
int NBT_isLongArray(NBTTAG *tag);
long long *NBT_getLongArray(TagLongArray *tag,int *count);
void NBT_setLongArray(TagLongArray *tag,int count,const long long *array);
long long NBT_getLongArrayIndex(TagLongArray *tag,int index);
int NBT_getLongArraySize(TagLongArray *tag);
NBTErr NBT_writeLongArray(TagLongArray *tag,FILE *stream);
TagLongArray *NBT_readLongArray(const char *name,FILE *stream);
TagLongArray *NBT_copyLongArray(TagLongArray *tag);
void NBT_destroyLongArray(TagLongArray *tag);




/* 通用函数
 * Generic functions*/
/* 判断标签类型
 * Get the type of the tag*/
NBTTAGType NBT_getType(NBTTAG *tag);
/* 写入标签,包括ID名称和辅助信息
 * Serialize tag with name, type and other things*/
NBTErr NBT_write(NBTTAG *tag,FILE *stream);
/* 写入标签,仅辅助信息
 * Serialize tag without name and type*/
NBTErr NBT_writeBody(NBTTAG *tag,FILE *stream);
/* 写入标签,仅名称和ID
 * Serialize the name and the type of the tag*/
NBTErr NBT_writeHead(NBTTAG *tag,FILE *stream);
/* 读入标签,包括ID名称和辅助信息
 * Deserialize the whole tag*/
NBTTAG *NBT_read(FILE *stream);
/* 读入标签,仅辅助信息
 * Deserialize the tag without name and type*/
NBTTAG *NBT_readBody(const char *name,NBTTAGType type,FILE *stream);
/* 读入标签,仅名称和ID
 * Deserialize the tag's name and type*/
char *NBT_readHead(NBTTAGType *typep,FILE *stream);
/* 复制标签
 * Copy the tag*/
NBTTAG *NBT_copy(NBTTAG *tag);
/* 获取标签名
 * Get the name (free() by yourself)*/
char *NBT_getName(NBTTAG *tag);
/* 设置标签名
 * Set the name*/
void NBT_setName(NBTTAG *tag,const char *name);
/* 删除标签
 * Release the tag*/
void NBT_destroy(NBTTAG *tag);
/* 删除标签数组
 * Relase the tag array*/
void NBT_releaseArray(NBTTAG **tags,int count);
/* 弄成NBT格式字符串
 * Print NBT Tag as NBT-Style String
 * A little different from minecraft nbt:
 * 		There is an "L" after the int list, byte list and long list to 
 * 			show difference between int array, byte array and long array
 * 		In minecraft, int lists and int arrays have same toString() outputs
 * 			so you will get wrong NBT data in minecraft if you serialize a
 * 			tag with int lists to string and deserialize the string to NBT
 * 		In order to fix this issue, I put an "L" after the int list, same as
 * 			what I did in nbtc java library so you can serialize a tag in
 * 			java and deserialize it in c if you use nbtc library*/
void NBT_print(NBTTAG *tag,FILE *stream);
/* 弄成JSON格式字符串
 * Print the JSON-Style String*/
void NBT_printJSON(NBTTAG *tag,FILE *stream);
/* 从NBT读入
 * Read from stream as NBT-Style String*/
NBTTAG *NBT_scan(FILE *stream);
/* 错误处理函数
 * Some functions about errors*/
NBTErr NBT_getLastError();
void NBT_getErrorDetail(char *str,int len);

typedef struct nbtbaseinterface_t{
	void (*toNBT)(void *,NBTTAG *);
	void (*fromNBT)(void *,NBTTAG *);
}nbtbaseinterface_t;
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112 /*C11 only*/
#define __n_class_cast_to_tag(obj) ((NBTTAG *)_Generic((obj),NBTTAG *:obj,\
		TagByte *:obj,\
		TagShort *:obj,\
		TagInt *:obj,\
		TagLong *:obj,\
		TagFloat *:obj,\
		TagDouble *:obj,\
		TagByteArray *:obj,\
		TagString *:obj,\
		TagList *:obj,\
		TagCompound *:obj,\
		TagIntArray *:obj,\
		TagLongArray *:obj))
void *__n_class_cast_to_type(void *obj,int type);
#define __n_class_cast_to_(obj,type,tid) (_Generic((obj),NBTTAG*:(type *)__n_class_cast_to_type((obj),tid),type *:(obj)))
#define NBT(obj) __n_class_cast_to_tag(obj)
#define NBT_BYTE(obj) __n_class_cast_to_(obj,TagByte,1)
#define NBT_SHORT(obj) __n_class_cast_to_(obj,TagShort,2)
#define NBT_INT(obj) __n_class_cast_to_(obj,TagInt,3)
#define NBT_LONG(obj) __n_class_cast_to_(obj,TagLong,4)
#define NBT_FLOAT(obj) __n_class_cast_to_(obj,TagFloat,5)
#define NBT_DOUBLE(obj) __n_class_cast_to_(obj,TagDouble,6)
#define NBT_BYTEARRAY(obj) __n_class_cast_to_(obj,TagByteArray,7)
#define NBT_STRING(obj) __n_class_cast_to_(obj,TagString,8)
#define NBT_LIST(obj) __n_class_cast_to_(obj,TagList,9)
#define NBT_COMPOUND(obj) __n_class_cast_to_(obj,TagCompound,10)
#define NBT_INTARRAY(obj) __n_class_cast_to_(obj,TagIntArray,11)
#define NBT_LONGARRAY(obj) __n_class_cast_to_(obj,TagLongArray,12)
#ifndef _NBT_C_BODY
#define NBT_getType(obj) NBT_getType(NBT(obj))
#define NBT_write(obj,stream) NBT_write(NBT(obj),stream)
#define NBT_writeBody(obj,stream) NBT_write(NBT(obj),stream)
#define NBT_writeHead(obj,stream) NBT_writeHead(NBT(obj),stream)
#define NBT_copy(obj) ((typeof(obj))NBT_copy(NBT(obj)))
#define NBT_getName(obj) NBT_getName(NBT(obj))
#define NBT_destroy(obj) NBT_destroy(NBT(obj))
#define NBT_print(obj,stream) NBT_print(NBT(obj),stream)
#define NBT_printJSON(obj,stream) NBT_printJSON(NBT(obj),stream)
#endif /*_NBT_C_BODY*/
#endif /*C11 condition*/
#endif

TagCompound *NBT_readFromFile(char *path);
NBTErr NBT_writeToFile(TagCompound *cpd,char *path);
#endif
