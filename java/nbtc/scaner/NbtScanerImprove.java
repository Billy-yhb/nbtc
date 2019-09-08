package nbtc.scaner;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Stack;

import nbtc.nbt.*;

public class NbtScanerImprove {
	private final String[] words;
	@SuppressWarnings("unused")
	private final NbtStringBuilder all;
	private int index=0;
	private final Integer[] linenos;
	private NbtScanerImprove(String[]src,NbtStringBuilder al,Integer[] integers){
		words=src;
		all=al;
		linenos=integers;
	}
	public static TagCompound nextCompound(InputStream in) throws IOException {
		return (TagCompound) word(in).readTag("");
	}
	private NbtTag<?> readTag(String name) throws IOException{
		String legal="abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$1234567890.";
		String legal_start=legal+"\"";
		if(words[index].equals("{")) {
			TagCompound cpd=new TagCompound();
			if(words[index+1].equals("}")) {
				index+=2;
				cpd.setName(name);
				return cpd;
			}
			while(!words[index++].equals("}")) {
				String key=readString(words[index++]);
				if(!words[index++].equals(":")){
					throw new NbtTextFormatException(linenos[index-1]);
				}
				NbtTag<?> tag=readTag(key);
				cpd.value.put(key, tag);
				if(!(words[index].equals(",")||words[index].equals("}"))){
					throw new NbtTextFormatException(linenos[index]);
				}
			}
			cpd.setName(name);
			return cpd;
		}else if(words[index].equals("[")) {
			TagList list=new TagList();
			if(words[index+1].equals("]")) {
				index+=2;
				list.setName(name);
				return list;
			}
			while(!words[index++].equals("]")) {
				NbtTag<?> tag=readTag("");
				list.value.add(tag);
				if(!(words[index].equals(",")||words[index].equals("]"))){
					throw new NbtTextFormatException(linenos[index]);
				}
			}
			if(words[index].equals("L")) {
				index++;
				list.setName(name);
				return list;
			}
			switch(list.type()) {
				case NbtTag.TAG_BYTE:
					byte[] arr=new byte[list.value.size()];
					for(int i=0;i<arr.length;i++) {
						arr[i]=((TagByte)list.value.get(i)).value;
					}
					TagByteArray rval=new TagByteArray();
					rval.value=arr;
					rval.setName(name);
					return rval;
				case NbtTag.TAG_INT:
					int[] arri=new int[list.value.size()];
					for(int i=0;i<arri.length;i++) {
						arri[i]=((TagInt)list.value.get(i)).value;
					}
					TagIntArray rvali=new TagIntArray();
					rvali.value=arri;
					rvali.setName(name);
					return rvali;
				case NbtTag.TAG_LONG:
					long[] arrl=new long[list.value.size()];
					for(int i=0;i<arrl.length;i++) {
						arrl[i]=((TagLong)list.value.get(i)).value;
					}
					TagLongArray rvall=new TagLongArray();
					rvall.value=arrl;
					rvall.setName(name);
					return rvall;
				default:
					list.setName(name);
					return list;
			}
		}else if(words[index].charAt(0)<='9'&&words[index].charAt(0)>='0'||words[index].charAt(0)=='-') {
			String num=words[index++];
			char end=num.charAt(num.length()-1);
			BigDecimal big;
			NbtTag<?> tag;
			if(num.charAt(num.length()-1)<='9'&&num.charAt(num.length()-1)>='0') {
				big=new BigDecimal(num);
				if(num.contains(".")) {
					tag=new TagDouble();
					((TagDouble)tag).value=big.doubleValue();
				}else {
					tag=new TagInt();
					((TagInt)tag).value=big.intValue();
				}
			}else {
				big=new BigDecimal(num.substring(0, num.length()-1));
				switch(end) {
					case 'b':
					case 'B':
						tag=new TagByte();
						((TagByte)tag).value=big.byteValue();
						break;
					case 'i':
					case 'I':
						tag=new TagInt();
						((TagInt)tag).value=big.intValue();
						break;
					case 's':
					case 'S':
						tag=new TagShort();
						((TagShort)tag).value=big.shortValue();
						break;
					case 'l':
					case 'L':
						tag=new TagLong();
						((TagLong)tag).value=big.longValue();
						break;
					case 'd':
					case 'D':
						tag=new TagDouble();
						((TagDouble)tag).value=big.doubleValue();
						break;
					case 'f':
					case 'F':
						tag=new TagFloat();
						((TagFloat)tag).value=big.floatValue();
						break;
					default:throw new NbtTextFormatException(linenos[index-1]);
				}
			}
			tag.setName(name);
			return tag;
		}else if(legal_start.indexOf(words[index].charAt(0))>=0) {
			TagString str=new TagString();
			str.setName(name);
			str.value=readString(words[index]);
			index++;
			return str;
		}else {
			throw new NbtTextFormatException(linenos[index]);
		}
	}
	private String readString(String key) {
		if(key.charAt(0)=='"') {
			StringBuilder builder=new StringBuilder();
			for(int i=1;i<key.length()-1;i++) {
				char b=key.charAt(i);
				if(b=='\\') {
					i++;
					b=key.charAt(i);
					switch(b) {
						case 'n':
							builder.append("\n");
							break;
						case 'r':
							builder.append("\r");
							break;
						case 'b':
							builder.append("\b");
							break;
						case '\\':
							builder.append("\\");
							break;
						case '\t':
							builder.append("\t");
							break;
						case '"':
							builder.append("\"");
							break;
						case '\'':
							builder.append("'");
							break;
					}
				}else {
					builder.append(b);
				}
			}
			return builder.toString();
		}else {
			return key;
		}
	}
	private static HashMap<InputStream,Byte> unread_map=new HashMap<>();
	private static byte read(InputStream in,NbtStringBuilder all) throws IOException{
		if(unread_map.containsKey(in))
			return unread_map.remove(in);
		int b=in.read();
		if(b<0) {
			throw new EOFException();
		}
		all.append(b);
		return (byte)b;
	}
	private static void unread(InputStream in, byte b) {
		unread_map.put(in, b);
	}
	private static NbtScanerImprove word(InputStream in) throws IOException{
		NbtStringBuilder val=new NbtStringBuilder();
		ArrayList<String> words=new ArrayList<>();
		ArrayList<Integer> linenos=new ArrayList<>();
		Stack<Character> bra=new Stack<>();
		NbtStringBuilder all=new NbtStringBuilder();
		String legal="abcdefghiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$1234567890.-";
		do {
			byte b=read(in,all);
			if(b=='{') {
				bra.push('}');
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add("{");
				linenos.add(all.getLine());
			}else if(b=='}') {
				if(bra.isEmpty()||bra.peek()!='}')
					throw new NbtTextFormatException(all.getLine());
				bra.pop();
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add("}");
				linenos.add(all.getLine());
			}else if(b=='[') {
				bra.push(']');
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add("[");
				linenos.add(all.getLine());
			}else if(b==']') {
				if(bra.isEmpty()||bra.peek()!=']')
					throw new NbtTextFormatException(all.getLine());
				bra.pop();
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add("]");
				linenos.add(all.getLine());
			}else if(b==',') {
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add(",");
				linenos.add(all.getLine());
			}else if(b==':') {
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				words.add(":");
				linenos.add(all.getLine());
			}else if(b==' '||b=='\n'||b=='\r'||b=='\t') {
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
			}else if(b=='"') {
				if(val.len!=0) {
					words.add(val.toString());
					linenos.add(all.getLine());
					val=new NbtStringBuilder();
				}
				NbtStringBuilder builder=new NbtStringBuilder();
				builder.append('"');
				while((b=read(in,all))!='"') {
					builder.append(b);
					if(b=='\\') {
						builder.append(read(in,all));
					}
				}
				words.add(builder.append('"').toString());
				linenos.add(all.getLine());
			}else if(legal.indexOf(b)>=0) {
				while(legal.indexOf(b)>=0) {
					val.append(b);
					b=read(in,all);
				}
				unread(in,b);
				words.add(val.toString());
				linenos.add(all.getLine());
				val=new NbtStringBuilder();
			}else {
				throw new NbtTextFormatException(all.getLine());
			}
		}while(!bra.isEmpty());
		return new NbtScanerImprove(words.toArray(new String[0]),all,linenos.toArray(new Integer[0]));
	}
	private static class NbtStringBuilder{
		public int len=0;
		public byte[] buffer=new byte[1024];
		public void append(byte b) {
			if(len==buffer.length) {
				buffer=Arrays.copyOf(buffer, len*2);
			}
			buffer[len++]=b;
		}
		public NbtStringBuilder append(int i) {
			append((byte)i);
			return this;
		}
		public String toString() {
			try {
				return new String(buffer,0,len,"UTF-8");
			} catch (UnsupportedEncodingException e) {
				throw new RuntimeException(e); 
			}
		}
		public int getLine() {
			int line=1;
			for(int i=0;i<len;i++) {
				if(buffer[i]=='\n') {
					line++;
				}
			}
			return line;
		}
	}
}
