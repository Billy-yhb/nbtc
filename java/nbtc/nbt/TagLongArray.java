package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagLongArray extends NbtTag<TagLongArray> {
	public long[] value=new long[0];
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		int length=in.readInt();
		value=new long[length];
		for(int i=0;i<length;i++)
			value[i]=in.readLong();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeInt(value.length);
		for(int i=0;i<value.length;i++)
			out.writeLong(value[i]);
	}

	@Override
	protected int id() {
		return 12;
	}

	@Override
	protected TagLongArray copy() {
		TagLongArray b=new TagLongArray();
		b.value=value.clone();
		return b;
	}

	@Override
	public String toString(int deep) {
		StringBuilder r=new StringBuilder("[ ");
		for(int i=0;i<value.length;i++) {
			r.append(value[i]+"L");
			if(i<value.length-1) {
				r.append(", ");
				if((i+1)%20==0) {
					endl(deep, r);
				}
			}
		}
		r.append(']');
		return r.toString();
	}
}
