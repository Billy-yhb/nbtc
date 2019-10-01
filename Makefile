JAVA_SOURCE=java/nbtc/nbt/NbtTag.java \
		java/nbtc/nbt/TagByte.java \
		java/nbtc/nbt/TagShort.java \
		java/nbtc/nbt/TagInt.java \
		java/nbtc/nbt/TagLong.java \
		java/nbtc/nbt/TagFloat.java \
		java/nbtc/nbt/TagDouble.java \
		java/nbtc/nbt/TagByteArray.java \
		java/nbtc/nbt/TagIntArray.java \
		java/nbtc/nbt/TagLongArray.java \
		java/nbtc/nbt/TagString.java \
		java/nbtc/nbt/TagList.java \
		java/nbtc/nbt/TagCompound.java \
		java/nbtc/nbt/data/NbtBase.java \
		java/nbtc/scaner/Main.java \
		java/nbtc/scaner/NbtScanerImprove.java \
		java/nbtc/scaner/NbtTextFormatException.java \
		java/module-info.java
C_SOURCE=c/nbt.h \
		c/nbt.c \
		c/hashmap.h \
		c/hashmap.c
all:libnbtc.so nbtc.jar
libnbtc.so:${C_SOURCE}
	gcc c/nbt.c c/hashmap.c -std=gnu11 -g -shared -fPIC -lz -Wall -Wno-parentheses -o libnbtc.so
java-bin:${JAVA_SOURCE}
	javac -d java-bin ${JAVA_SOURCE} --release 11
nbtc.jar:java-bin
	jar --create --file nbtc.jar --main-class nbtc.scaner.Main --module-version 1.0 -C java-bin .
test:libnbtc.so c/test.c
	gcc c/test.c -g -L . -lnbtc -o test&&LD_LIBRARY_PATH=. ./test
.PHONY:clean install uninstall all
clean:
	-rm -rf libnbtc.so java-bin nbtc.jar