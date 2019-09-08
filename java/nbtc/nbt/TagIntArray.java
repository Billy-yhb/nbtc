package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagIntArray extends NbtTag<TagIntArray> {
	public int[] value=new int[0];
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		int length=in.readInt();
		value=new int[length];
		for(int i=0;i<length;i++)
			value[i]=in.readInt();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeInt(value.length);
		for(int i=0;i<value.length;i++)
			out.writeInt(value[i]);
	}

	@Override
	protected int id() {
		return 11;
	}

	@Override
	protected TagIntArray copy() {
		TagIntArray b=new TagIntArray();
		b.value=value.clone();
		return b;
	}
	
	@Override
	public String toString(int deep) {
		StringBuilder r=new StringBuilder("[ ");
		for(int i=0;i<value.length;i++) {
			r.append(value[i]);
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
