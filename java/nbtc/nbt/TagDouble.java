package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagDouble extends NbtTag<TagDouble>{
	public Double value=0d;
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		value=in.readDouble();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeDouble(value);
	}

	@Override
	protected int id() {
		return 6;
	}

	@Override
	protected TagDouble copy() {
		TagDouble b=new TagDouble();
		b.value=value;
		return b;
	}

	@Override
	public String toString(int deep) {
		return value+"d";
	}

}
