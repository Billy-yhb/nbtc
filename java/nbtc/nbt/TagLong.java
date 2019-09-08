package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagLong extends NbtTag<TagLong>{
	public long value;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readLong();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeLong(value);
	}

	@Override
	protected int id() {
		return 4;
	}

	@Override
	protected TagLong copy() {
		TagLong b=new TagLong();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"L";
	}

}
