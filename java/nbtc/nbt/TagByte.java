package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagByte extends NbtTag<TagByte>{
	public byte value=0;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readByte();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeByte(value);
	}

	@Override
	protected int id() {
		return 1;
	}

	@Override
	protected TagByte copy() {
		TagByte b=new TagByte();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"b";
	}

}
