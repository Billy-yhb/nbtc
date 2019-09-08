package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagString extends NbtTag<TagString> {
	public String value="";
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=new String(in.readNBytes(in.readShort()),"UTF-8");
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		byte[] bytes=value.getBytes("UTF-8");
		out.writeShort(bytes.length);
		out.write(bytes);
	}

	@Override
	protected int id() {
		return 8;
	}

	@Override
	protected TagString copy() {
		TagString b=new TagString();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		StringBuilder sb=new StringBuilder();
		printStr(value,sb);
		return sb.toString();
	}

}
