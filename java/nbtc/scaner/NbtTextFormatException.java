package nbtc.scaner;

import java.io.IOException;

public class NbtTextFormatException extends IOException {

	private static final long serialVersionUID = -6227771753111485369L;
	private int lineno;
	public NbtTextFormatException(int lineno,String info) {
		super(info);
		this.lineno=lineno;
	}
	public NbtTextFormatException(int lineno) {
		this(lineno,"Incorrect nbt text at line "+lineno);
	}
	public int getLineno() {
		return lineno;
	}
}
