package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagFloat extends NbtTag<TagFloat>{
	public float value=0f;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readFloat();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeFloat(value);
	}

	@Override
	protected int id() {
		return 5;
	}

	@Override
	protected TagFloat copy() {
		TagFloat b=new TagFloat();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"f";
	}

}
