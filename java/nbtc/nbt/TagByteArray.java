package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class TagByteArray extends NbtTag<TagByteArray> {
	public byte[] value=new byte[0];
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		int length=in.readInt();
		value=new byte[length];
		for(int i=0;i<length;i++)
			value[i]=in.readByte();
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		out.writeInt(value.length);
		for(int i=0;i<value.length;i++)
			out.writeByte(value[i]);
	}

	@Override
	protected int id() {
		return 7;
	}

	@Override
	protected TagByteArray copy() {
		TagByteArray b=new TagByteArray();
		b.value=value.clone();
		return b;
	}

	@Override
	public String toString(int deep) {
		StringBuilder r=new StringBuilder("[ ");
		for(int i=0;i<value.length;i++) {
			r.append(value[i]+"b");
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
