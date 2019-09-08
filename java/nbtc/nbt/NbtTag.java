package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.GZIPOutputStream;
import java.util.zip.GZIPInputStream;

public abstract class NbtTag <T extends NbtTag<T>> {
	
	public static final int TAG_END=0;
	public static final int TAG_BYTE=1;
	public static final int TAG_SHORT=2;
	public static final int TAG_INT=3;
	public static final int TAG_LONG=4;
	public static final int TAG_FLOAT=5;
	public static final int TAG_DOUBLE=6;
	public static final int TAG_BYTEARRAY=7;
	public static final int TAG_STRING=8;
	public static final int TAG_LIST=9;
	public static final int TAG_COMPOUND=10;
	public static final int TAG_INTARRAY=11;
	public static final int TAG_LONGARRAY=12;
	
	public String name="";
	protected abstract void readFrom(DataInputStream in) throws IOException;
	protected abstract void writeTo(DataOutputStream out) throws IOException;
	protected abstract int id();
	protected abstract T copy();
	public String toString() {
		return toString(1);
	}
	protected final void endl(int deep,StringBuilder builder) {
		builder.append("\n");
		for(int i=0;i<deep;i++) {
			builder.append("\t");
		}
	}
	protected final void printStr(String str,StringBuilder builder) {
		String as="abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$1234567890";
		boolean fall=false;
		if(str.equals("")) {
			builder.append("\"\"");
			return;
		}
		if(str.charAt(0)<'9'&&str.charAt(0)>'0')
			fall=this instanceof TagString;
		for(char n:str.toCharArray()) {
			if(as.indexOf(n)<0)
				fall=true;
		}
		if(fall) {
			builder.append('"');
			for(char c:str.toCharArray()) {
				switch(c) {
					case '"':
						builder.append("\\\"");
						break;
					case '\t':
						builder.append("\\t");
						break;
					case '\n':
						builder.append("\\n");
						break;
					case '\b':
						builder.append("\\b");
						break;
					case '\r':
						builder.append("\\r");
						break;
					default:
						builder.append(c);
				}
			}
			builder.append('"');
		}else {
			builder.append(str);
		}
	}
	public abstract String toString(int deep);
	public String getName() {
		return name;
	}
	public void setName(String s) {
		name=s;
	}
	@SuppressWarnings("unchecked")
	public static <E extends NbtTag<E>> E allocTag(int id,String name){
		if(id==0)
			return null;
		NbtTag<?> rval;
		switch(id) {
			case 1:
				rval=new TagByte();
				break;
			case 2:
				rval=new TagShort();
				break;
			case 3:
				rval=new TagInt();
				break;
			case 4:
				rval=new TagLong();
				break;
			case 5:
				rval=new TagFloat();
				break;
			case 6:
				rval=new TagDouble();
				break;
			case 7:
				rval=new TagByteArray();
				break;
			case 8:
				rval=new TagString();
				break;
			case 9:
				rval=new TagList();
				break;
			case 10:
				rval=new TagCompound();
				break;
			case 11:
				rval=new TagIntArray();
				break;
			case 12:
				rval=new TagLongArray();
				break;
			default:
				throw new IllegalArgumentException("Incorrect id "+id);
		}
		rval.setName(name);
		return (E)rval;
	}
	public static TagCompound readFromFile(InputStream file) throws IOException{
		DataInputStream in=new DataInputStream(new GZIPInputStream(file));
		TagCompound rval=(TagCompound)readTag(in);
		in.close();
		return rval;
	}
	public static void writeToFile(TagCompound cpd,OutputStream file) throws IOException{
		DataOutputStream out=new DataOutputStream(new GZIPOutputStream(file));
		writeTag(out, cpd);
		out.close();
	}
	@SuppressWarnings({ "rawtypes", "unchecked" })
	public static NbtTag readTag(DataInputStream stream) throws IOException {
		byte type=stream.readByte();
		if(type==0)
			return null;
		String name=new String(stream.readNBytes(stream.readShort()),"UTF-8");
		NbtTag t=allocTag(type,name);
		t.readFrom(stream);
		return t;
	}
	public static void writeTag(DataOutputStream out,NbtTag<?> tag) throws IOException {
		out.writeByte((byte)tag.id());
		out.writeShort(tag.name.length());
		out.write(tag.name.getBytes("UTF-8"));
		tag.writeTo(out);
	}
}
