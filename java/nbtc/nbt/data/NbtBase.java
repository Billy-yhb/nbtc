package nbtc.nbt.data;

import nbtc.nbt.TagCompound;

public interface NbtBase {
	public void toNbt(TagCompound cpd);
	public void fromNbt(TagCompound cpd);
}
