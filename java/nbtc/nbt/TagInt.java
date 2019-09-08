package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagInt extends NbtTag<TagInt> {
	public int value=0;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readInt();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeInt(value);
	}

	@Override
	protected int id() {
		return 3;
	}

	@Override
	protected TagInt copy() {
		TagInt b=new TagInt();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"";
	}

}
