package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;

public class TagList extends NbtTag<TagList>{
	public ArrayList<NbtTag<?>> value=new ArrayList<>();
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		int type=in.readByte();
		int length=in.readInt();
		value=new ArrayList<>(length);
		for(int i=0;i<length;i++) {
			NbtTag<?> tag=NbtTag.allocTag(type, "");
			tag.readFrom(in);
			value.add(i,tag);
		}
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		if(value.isEmpty()) {
			out.writeByte(0);
			out.writeInt(0);
		}else {
			int tid=value.get(0).id();
			out.writeByte(tid);
			out.writeInt(value.size());
			for(NbtTag<?> tag:value) {
				if(tag.id()==tid&&tag.name.isEmpty()) {
					tag.writeTo(out);
				}else {
					throw new IllegalArgumentException("The element "+tid+" is not correct");
				}
			}
		}
	}

	@Override
	protected int id() {
		return 9;
	}

	public int type() {
		return value.isEmpty()?0:value.get(0).id();
	}
	@Override
	protected TagList copy() {
		TagList r=new TagList();
		r.value=new ArrayList<>(value.size());
		for(int i=0;i<value.size();i++)
			r.value.add(i,value.get(i));
		return r;
	}

	@Override
	public String toString(int deep) {
		int type=type();
		StringBuilder r=new StringBuilder("[");
		if(type>6)
			endl(deep,r);
		NbtTag<?>[] v=value.toArray(new NbtTag<?>[0]);
		for(int i=0;i<v.length;i++) {
			r.append(v[i].toString(deep+1));
			if(i<v.length-1) {
				r.append(", ");
				if((i+1)%20==0||type>6) {
					endl(deep, r);
				}
			}
		}
		if(type>6)
			endl(deep-1,r);
		r.append(']');
		if(type==NbtTag.TAG_BYTE||type==NbtTag.TAG_INT||type==NbtTag.TAG_LONG)
			r.append(" L");
		return r.toString();
	}

	public NbtTag<?> get(int i){
		return value.get(i);
	}
	
	public void set(int i,NbtTag<?> tag){
		value.set(i, tag);
	}
}
