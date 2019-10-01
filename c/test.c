#include "nbt.h"
#include <assert.h>
#include <stdlib.h>
#define out(something) printf(_Generic((something),byte:"%hhd\n",short:"%hd\n",int:"%d\n",long:"%ld\n",long long:"%lld\n",char*:"%s\n",const char *:"%s\n"),something)
int main(void){

	TagByte *b=NBT_createByteTag("",255),*c;
	NBT_print(b,stdout);/*-1b*/
	out("\n");
	char *name=NBT_getName(b);
	out(name);/**/
	free(name);
	NBT_setName(b,"NoName");
	name=NBT_getName(b);
	out(name);/*NoName*/
	free(name);
	out(NBT_getByte(b));/*-1*/
	NBT_setByte(b,70);
	out(NBT_getByte(b));/*70*/
	c=NBT_copyByte(b);
	name=NBT_getName(b);
	out(name);/*NoName*/
	free(name);
	out(NBT_getByte(b));/*70*/
	out(!!NBT(b));/*1*/
	out(!!NBT_BYTE(b));/*1*/
	out(!!NBT_BYTE((NBTTag *)b));/*1*/
	out(!!NBT_SHORT((NBTTag *)b));/*0*/
	out(!!NBT_SHORT((TagShort *)b));/*1*/
	NBT_destroyByte(b);
	NBT_destroyByte(c);

	TagInt *i=NBT_createIntTag("testint",0);
	TagCompound *cpd=NBT_createCompoundTag("root",0,NULL);
	NBT_printJSON(cpd,stdout);
	out("\n");
	NBT_setCompoundIndex(cpd,i);
	NBT_printJSON(cpd,stdout);
	out("\n");
	NBT_setCompoundInterface(cpd,"1",(long long[]){102,200000000},NBT_BASE_UUID);
	NBT_setCompoundDouble(cpd,"double",2.5);
	NBT_print(cpd,stdout);
	out("\n");
	NBT_destroy(i);
	i=NBT_INT(NBT_getCompoundIndexNonCopy(cpd,"testint"));
	NBT_setInt(i,100);
	NBT_print(cpd,stdout);
	out("\n");
	NBT_destroy(cpd);

	return 0;
}