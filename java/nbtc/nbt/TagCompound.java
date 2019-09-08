package nbtc.nbt;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class TagCompound extends NbtTag<TagCompound> {
	public Map<String,NbtTag<?>> value=new HashMap<>();
	
	@Override
	protected void readFrom(DataInputStream in) throws IOException {
		NbtTag<?> next;
		while((next=NbtTag.readTag(in))!=null) {
			value.put(next.getName(), next);
		}
	}

	@Override
	protected void writeTo(DataOutputStream out) throws IOException {
		for(NbtTag<?> tag:value.values()) {
			NbtTag.writeTag(out, tag);
		}
		out.writeByte(0);
	}

	@Override
	protected int id() {
		return 10;
	}

	@Override
	protected TagCompound copy() {
		TagCompound cpd=new TagCompound();
		for(String s:value.keySet()) {
			cpd.value.put(s, value.get(s));
		}
		return cpd;
	}

	@Override
	public String toString(int deep) {
		StringBuilder r=new StringBuilder("{");
		String[] keys=value.keySet().toArray(new String[0]);
		for(int i=0;i<keys.length;i++) {
			endl(deep,r);
			String key=keys[i];
			printStr(key,r);
			r.append(": ");
			r.append(value.get(key).toString(deep+1));
			if(i<keys.length-1) {
				r.append(", ");
			}
		}
		endl(deep-1,r);
		r.append("}");
		return r.toString();
	}

	public NbtTag<?> get(String key){
		return value.get(key);
	}
	
	public void put(NbtTag<?> tag) {
		value.put(tag.name, tag);
	}
}
