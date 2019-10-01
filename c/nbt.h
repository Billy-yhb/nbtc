#ifndef INCLUDE_NBT__
#define INCLUDE_NBT__
#include <stdio.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C"{
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
typedef struct NBTTag NBTTag;
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
}NBTTagType;
typedef enum NBTErr{
	NBT_NOERR,/*无错  no error*/
	NBT_IOERR,/*输入输出错误  IO error*/
	NBT_ILLEGAL,/*传入的NBT对象错误，例如List含有不同类型  wrong NBT data eg. diffent tag type in a list*/
	NBT_NULLPTR/*接受空指针  get a NULL pointer*/
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

typedef struct NBTBaseInterface{
	NBTTagType nbtType;
	void (*toNBT)(const void *,NBTTag *,struct NBTBaseInterface *,va_list options);/*actually there is no const here*/
	void (*fromNBT)(void *,const NBTTag *,struct NBTBaseInterface *,va_list options);
}NBTBaseInterface;
/*NBT_serialize(long long uuid_in[2],TagCompound *tag,NBT_BASE_UUID);
 *	save uuid to compound*/
extern NBTBaseInterface *NBT_BASE_UUID; 
/*NBT_serialize(_Bool *bool_in,TagByte *tag,NBT_BASE_BOOL);
 *	save bool to byte*/
extern NBTBaseInterface *NBT_BASE_BOOL;
/*NBT_serialize(int *enum_in,TagString *tag,NBT_BASE_ENUM,char *enum_str[]);
 *  example enum:
 * 		enum some_enum{a,b,c,d}
 * 		void test(TagString *str){
 * 			enum some_enum e;
 * 			NBT_serialize(&e,(void *)str,NBT_BASE_ENUM,{"a","b","c","d",NULL});
 * 		}*/
extern NBTBaseInterface *NBT_BASE_ENUM;

void NBT_serialize(void *object,NBTTag *tag,NBTBaseInterface *interface,...);
void NBT_deserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,...);
void NBT_vserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,va_list options);
void NBT_vdeserialize(void *object,NBTTag *tag,NBTBaseInterface *interface,va_list options);
NBTTag *NBT_allocTagByInterface(NBTBaseInterface *interface);

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
int NBT_isByte(const NBTTag *tag);
/* 返回所保存的b值
 * Return the storged b*/
byte NBT_getByte(const TagByte *tag);
/* 设置保存的b
 * Set b*/
void NBT_setByte(TagByte *tag,byte b);
/* 将tag以二进制形式存储(不含标签名和ID)
 * Serialize a tag to stream as NBT binary format (excude the name and the type)*/
NBTErr NBT_writeByte(const TagByte *tag,FILE *stream);
/* 从二进制流中读取一个TAG_BYTE数据(不含标签名和ID)
 * Deserialize from binary stream to a tag (excude the name and the type)*/
TagByte *NBT_readByte(const char *name,FILE *stream);
/* 复制一个TAG_BYTE
 * Copy a byte tag*/
TagByte *NBT_copyByte(const TagByte *tag);
/* 释放这个TAG_BYTE占用的资源
 * Release this byte tag*/
void NBT_destroyByte(TagByte *tag);


TagShort *NBT_createShortTag(const char *name,short s);
int NBT_isShort(const NBTTag *tag);
short NBT_getShort(const TagShort *tag);
void NBT_setShort(TagShort *tag,short s);
NBTErr NBT_writeShort(const TagShort *tag,FILE *stream);
TagShort *NBT_readShort(const char *name,FILE *stream);
TagShort *NBT_copyShort(const TagShort *tag);
void NBT_destroyShort(TagShort *tag);


TagInt *NBT_createIntTag(const char *name,int i);
int NBT_isInt(const NBTTag *tag);
int NBT_getInt(const TagInt *tag);
void NBT_setInt(TagInt *tag,int i);
NBTErr NBT_writeInt(const TagInt *tag,FILE *stream);
TagInt *NBT_readInt(const char *name,FILE *stream);
TagInt *NBT_copyInt(const TagInt *tag);
void NBT_destroyInt(TagInt *tag);


TagLong *NBT_createLongTag(const char *name,long long l);
int NBT_isLong(const NBTTag *tag);
long long NBT_getLong(const TagLong *tag);
void NBT_setLong(TagLong *tag,long long l);
NBTErr NBT_writeLong(const TagLong *tag,FILE *stream);
TagLong *NBT_readLong(const char *name,FILE *stream);
TagLong *NBT_copyLong(const TagLong *tag);
void NBT_destroyLong(TagLong *tag);


TagFloat *NBT_createFloatTag(const char *name,float f);
int NBT_isFloat(const NBTTag *tag);
float NBT_getFloat(const TagFloat *tag);
void NBT_setFloat(TagFloat *tag,float f);
NBTErr NBT_writeFloat(const TagFloat *tag,FILE *stream);
TagFloat *NBT_readFloat(const char *name,FILE *stream);
TagFloat *NBT_copyFloat(const TagFloat *tag);
void NBT_destroyFloat(TagFloat *tag);


TagDouble *NBT_createDoubleTag(const char *name,double d);
int NBT_isDouble(const NBTTag *tag);
double NBT_getDouble(const TagDouble *tag);
void NBT_setDouble(TagDouble *tag,double d);
NBTErr NBT_writeDouble(const TagDouble *tag,FILE *stream);
TagDouble *NBT_readDouble(const char *name,FILE *stream);
TagDouble *NBT_copyDouble(const TagDouble *tag);
void NBT_destroyDouble(TagDouble *tag);


/* 创建一个count长度的byte数组(array)
 * Create a byte array tag from the array pointer and the length is count*/
TagByteArray *NBT_createByteArrayTag(const char *name,int count,const byte *array);
int NBT_isByteArray(const NBTTag *tag);
/* 获取整个byte数组(需自行free)
 * 参数:
 *     count  返回实际的byte数组长度
 * 返回:
 *     一个malloc得到的byte数组(另外的,修改不会引起TAG值改变)
 * Get the whole byte array (free() by yourself)
 * 		The length of the array will be set to the *count
 * 		The byte array will return
 * */
byte *NBT_getByteArray(const TagByteArray *tag,int *count);
void NBT_setByteArray(TagByteArray *tag,int count,const byte *array);
byte NBT_getByteArrayIndex(const TagByteArray *tag,int index);
int NBT_getByteArraySize(const TagByteArray *tag);
NBTErr NBT_writeByteArray(const TagByteArray *tag,FILE *stream);
TagByteArray *NBT_readByteArray(const char *name,FILE *stream);
TagByteArray *NBT_copyByteArray(const TagByteArray *tag);
void NBT_destroyByteArray(TagByteArray *tag);


TagString *NBT_createStringTag(const char *name,const char *value);
int NBT_isString(const NBTTag *tag);
char *NBT_getString(const TagString *tag,int *count);
void NBT_setString(TagString *tag,const char *str);
int NBT_getStringSize(const TagString *tag);
NBTErr NBT_writeString(const TagString *tag,FILE *stream);
TagString *NBT_readString(const char *name,FILE *stream);
TagString *NBT_copyString(const TagString *tag);
void NBT_destroyString(TagString *tag);


/* 创建一个count长度的List,元素必须为相同类型
 * Create a list tag, all the elements must be same type*/
TagList *NBT_createListTag(const char *name,int count,NBTTag **array);
int NBT_isList(const NBTTag *tag);
NBTTag **NBT_getList(const TagList *tag,int *count);
void NBT_setList(TagList *tag,int count,NBTTag **array);
NBTTag *NBT_getListIndex(const TagList *tag,int index);
int NBT_getListSize(const TagList *tag);
NBTTagType NBT_getListType(const TagList *tag);
NBTErr NBT_writeList(const TagList *tag,FILE *stream);
TagList *NBT_readList(const char *name,FILE *stream);
TagList *NBT_copyList(const TagList *tag);
void NBT_destroyList(TagList *tag);


/* 创建一个size长度的Compound,元素必须无重名
 * Create a compound tag with size elements and all the element must have different name*/
TagCompound *NBT_createCompoundTag(const char *name,int size,NBTTag **array);
int NBT_isCompound(const NBTTag *tag);
HashMap(char *,TAG *) *NBT_getCompound(const TagCompound *tag);
void NBT_setCompound(TagCompound *tag,HashMap(char *,TAG *) *map);
NBTTag *NBT_getCompoundIndex(const TagCompound *tag,const char *index);
NBTTag *NBT_getCompoundIndexNonCopy(const TagCompound *tag,const char *index);
void NBT_setCompoundIndex(TagCompound *tag,NBTTag *ele);
void NBT_removeCompoundIndex(TagCompound *tag,const char *index);
void NBT_setCompoundByte(TagCompound *tag,const char *index,byte value);
void NBT_setCompoundShort(TagCompound *tag,const char *index,short value);
void NBT_setCompoundInt(TagCompound *tag,const char *index,int value);
void NBT_setCompoundLong(TagCompound *tag,const char *index,long long value);
void NBT_setCompoundFloat(TagCompound *tag,const char *index,float value);
void NBT_setCompoundDouble(TagCompound *tag,const char *index,double value);
void NBT_setCompoundString(TagCompound *tag,const char *index,const char *value);
void NBT_setCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,...);
void NBT_vsetCompoundInterface(TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,va_list options);
/* 若无法取到值，则返回fallback,字符串返回复制后的值
 * If failed to get the value, fallback will be returned but if fallback is a string (const char *),
 * 		copied value will be returned*/
byte NBT_getCompoundByte(const TagCompound *tag,const char *index,byte fallback);
short NBT_getCompoundShort(const TagCompound *tag,const char *index,short fallback);
int NBT_getCompoundInt(const TagCompound *tag,const char *index,int fallback);
long long NBT_getCompoundLong(const TagCompound *tag,const char *index,long long fallback);
float NBT_getCompoundFloat(const TagCompound *tag,const char *index,float fallback);
double NBT_getCompoundDouble(const TagCompound *tag,const char *index,double fallback);
char *NBT_getCompoundString(const TagCompound *tag,const char *index,int *count,const char *fallback);
void NBT_getCompoundInterface(const TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,...);
void NBT_vgetCompoundInterface(const TagCompound *tag,const char *index,void *object,NBTBaseInterface *interface,va_list options);
void NBT_clearCompoundMap(TagCompound *tag);
int NBT_getCompoundSize(const TagCompound *tag);
char **NBT_getCompoundKeys(const TagCompound *tag,int *count);
NBTTagType NBT_getCompoundIndexType(const TagCompound *tag,const char *index);
NBTErr NBT_writeCompound(const TagCompound *tag,FILE *stream);
TagCompound *NBT_readCompound(const char *name,FILE *stream);
TagCompound *NBT_copyCompound(const TagCompound *tag);
void NBT_destroyCompound(TagCompound *tag);


TagIntArray *NBT_createIntArrayTag(const char *name,int size,const int *array);
int NBT_isIntArrayTag(const NBTTag *tag);
int *NBT_getIntArray(const TagIntArray *tag,int *count);
void NBT_setIntArray(TagIntArray *tag,int count,const int *array);
int NBT_getIntArrayIndex(const TagIntArray *tag,int index);
int NBT_getIntArraySize(const TagIntArray *tag);
NBTErr NBT_writeIntArray(const TagIntArray *tag,FILE *stream);
TagIntArray *NBT_readIntArray(const char *name,FILE *stream);
TagIntArray *NBT_copyIntArray(const TagIntArray *tag);
void NBT_destroyIntArray(TagIntArray *tag);


TagLongArray *NBT_createLongArrayTag(const char *name,int size,const long long *array);
int NBT_isLongArray(const NBTTag *tag);
long long *NBT_getLongArray(const TagLongArray *tag,int *count);
void NBT_setLongArray(TagLongArray *tag,int count,const long long *array);
long long NBT_getLongArrayIndex(const TagLongArray *tag,int index);
int NBT_getLongArraySize(const TagLongArray *tag);
NBTErr NBT_writeLongArray(const TagLongArray *tag,FILE *stream);
TagLongArray *NBT_readLongArray(const char *name,FILE *stream);
TagLongArray *NBT_copyLongArray(const TagLongArray *tag);
void NBT_destroyLongArray(TagLongArray *tag);




/* 通用函数
 * Generic functions*/
/* 判断标签类型
 * Get the type of the tag*/
NBTTagType NBT_getType(const NBTTag *tag);
/* 写入标签,包括ID名称和辅助信息
 * Serialize tag with name, type and other things*/
NBTErr NBT_write(const NBTTag *tag,FILE *stream);
/* 写入标签,仅辅助信息
 * Serialize tag without name and type*/
NBTErr NBT_writeBody(const NBTTag *tag,FILE *stream);
/* 写入标签,仅名称和ID
 * Serialize the name and the type of the tag*/
NBTErr NBT_writeHead(const NBTTag *tag,FILE *stream);
/* 读入标签,包括ID名称和辅助信息
 * Deserialize the whole tag*/
NBTTag *NBT_read(FILE *stream);
/* 读入标签,仅辅助信息
 * Deserialize the tag without name and type*/
NBTTag *NBT_readBody(const char *name,NBTTagType type,FILE *stream);
/* 读入标签,仅名称和ID
 * Deserialize the tag's name and type*/
char *NBT_readHead(NBTTagType *typep,FILE *stream);
/* 复制标签
 * Copy the tag*/
NBTTag *NBT_copy(NBTTag *tag);
/* 获取标签名
 * Get the name (free() by yourself)*/
char *NBT_getName(const NBTTag *tag);
/* 设置标签名
 * Set the name*/
void NBT_setName(NBTTag *tag,const char *name);
/* 删除标签
 * Release the tag*/
void NBT_destroy(NBTTag *tag);
/* 删除标签数组
 * Relase the tag array*/
void NBT_releaseArray(NBTTag **tags,int count);
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
void NBT_print(const NBTTag *tag,FILE *stream);
/* 弄成JSON格式字符串
 * Print the JSON-Style String*/
void NBT_printJSON(const NBTTag *tag,FILE *stream);
/* 从NBT读入
 * Read from stream as NBT-Style String*/
NBTTag *NBT_scan(FILE *stream);
/* 错误处理函数
 * Some functions about errors*/
NBTErr NBT_getLastError();
void NBT_getErrorDetail(char *str,int len);

#ifndef __cplusplus
void *__n_class_cast_to_type(const void *obj,int type);
#if __STDC_VERSION__ && __STDC_VERSION__ >= 201112 /*C11 only*/
#define __n_class_cast_to_tag(obj) (_Generic((obj),NBTTag *:obj,\
		TagByte *:(NBTTag *)obj,\
		TagShort *:(NBTTag *)obj,\
		TagInt *:(NBTTag *)obj,\
		TagLong *:(NBTTag *)obj,\
		TagFloat *:(NBTTag *)obj,\
		TagDouble *:(NBTTag *)obj,\
		TagByteArray *:(NBTTag *)obj,\
		TagString *:(NBTTag *)obj,\
		TagList *:(NBTTag *)obj,\
		TagCompound *:(NBTTag *)obj,\
		TagIntArray *:(NBTTag *)obj,\
		TagLongArray *:(NBTTag *)obj,\
		const NBTTag *:obj,\
		const TagByte *:(const NBTTag *)obj,\
		const TagShort *:(const NBTTag *)obj,\
		const TagInt *:(const NBTTag *)obj,\
		const TagLong *:(const NBTTag *)obj,\
		const TagFloat *:(const NBTTag *)obj,\
		const TagDouble *:(const NBTTag *)obj,\
		const TagByteArray *:(const NBTTag *)obj,\
		const TagString *:(const NBTTag *)obj,\
		const TagList *:(const NBTTag *)obj,\
		const TagCompound *:(const NBTTag *)obj,\
		const TagIntArray *:(const NBTTag *)obj,\
		const TagLongArray *:(const NBTTag *)obj))
#define __n_class_cast_to_(obj,type,tid) (_Generic((obj),NBTTag*:(type *)__n_class_cast_to_type((obj),tid),type *:(obj),const NBTTag*:(const type *)__n_class_cast_to_type((obj),tid),const type *:(obj)))
#define NBT(obj) __n_class_cast_to_tag(obj)
#else /*C11 condition*/
/*Always use pointer because struct is incorrect*/
struct NBTTag {
	char nbt_cast_most[4];
};
struct bytet_t {
	char nbt_cast_most[5];
};
struct shortt_t {
	char nbt_cast_most[6];
};
struct intt_t {
	char nbt_cast_most[7];
};
struct longt_t {
	char nbt_cast_most[8];
};
struct floatt_t {
	char nbt_cast_most[9];
};
struct doublet_t {
	char nbt_cast_most[10];
};
struct bytarrt_t {
	char nbt_cast_most[11];
};
struct strt_t {
	char nbt_cast_most[12];
};
struct listt_t {
	char nbt_cast_most[13];
};
struct compoundt_t {
	char nbt_cast_most[14];
};
struct intarrt_t {
	char nbt_cast_most[15];
};
struct lonarrt_t {
	char nbt_cast_most[16];
};
#define __n_class_cast_to_(obj,type,tid) ((type *)(((sizeof((obj)->nbt_cast_most)-4)==tid ? (obj) : __n_class_cast_to_type((obj),tid))))
#define NBT(obj) ((NBTTag *)(sizeof((obj)->nbt_cast_most) ? (obj) : NULL))
#endif /*C11 condition*/
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
#define NBT_setName(obj,name) NBT_setName(NBT(obj),name)
#define NBT_destroy(obj) NBT_destroy(NBT(obj))
#define NBT_print(obj,stream) NBT_print(NBT(obj),stream)
#define NBT_printJSON(obj,stream) NBT_printJSON(NBT(obj),stream)
#define NBT_setCompoundIndex(obj,ele) NBT_setCompoundIndex(obj,NBT(ele))
#endif /*_NBT_C_BODY*/
#endif /*__cplusplus*/ /*No type-safety cast for cpp. c++不支持类型安全的强制类型转换*/

TagCompound *NBT_readFromFile(char *path);
NBTErr NBT_writeToFile(const TagCompound *cpd,char *path);
#ifdef __cplusplus
}
#endif
#endif
