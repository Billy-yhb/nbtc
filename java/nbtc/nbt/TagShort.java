package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagShort extends NbtTag<TagShort>{
	public short value;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readShort();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeShort(value);
	}

	@Override
	protected int id() {
		return 2;
	}

	@Override
	protected TagShort copy() {
		TagShort b=new TagShort();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"s";
	}

}
