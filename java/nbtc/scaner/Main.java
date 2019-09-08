package nbtc.scaner;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import nbtc.nbt.NbtTag;
import nbtc.nbt.TagCompound;

public class Main {
	@SuppressWarnings("resource")
	public static void main(String[]args) {
		if(args.length!=3) {
			usage();
		}else {
			switch(args[0]) {
				case "bin":
				try {
					FileInputStream fin=new FileInputStream(args[1]);
					FileOutputStream fout=new FileOutputStream(args[2]);
					try {
						NbtTag.writeToFile(NbtScanerImprove.nextCompound(fin), fout);
					} catch (IOException e) {
						throw new RuntimeException("Incorrect Nbt file",e);
					}
					try {
						fin.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				} catch (FileNotFoundException e) {
					throw new RuntimeException("File "+args[1]+"not found.",e);
				}
				return;
				case "txt":
					try {
						FileInputStream fin=new FileInputStream(args[1]);
						FileOutputStream fout=new FileOutputStream(args[2]);
						try {
							fout.write(NbtTag.readFromFile(fin).toString().getBytes("UTF-8"));
						} catch (IOException e) {
							throw new RuntimeException("Incorrect Nbt file",e);
						}
						try {
							fout.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
					} catch (FileNotFoundException e) {
						throw new RuntimeException("File "+args[1]+"not found.",e);
					}
					return;
				case "cmp":
					try{
						FileInputStream in1=new FileInputStream(args[1]);
						FileInputStream in2=new FileInputStream(args[2]);
						TagCompound cpd1=NbtTag.readFromFile(in1);
						TagCompound cpd2=NbtTag.readFromFile(in2);
						if(cpd1.toString().equals(cpd2.toString())){//相同的nbt始终有相同的序列化结果
							System.out.println("Files are same.");
						}else{
							System.out.println("Files are different.");
						}
					}catch(IOException e){
						e.printStackTrace();
					}
					break;
				default:
					usage();
			}
		}
	}
	private static void usage() {
		System.err.print("Usage: java -jar "+
				Main.class.getProtectionDomain().getCodeSource().getLocation().getFile()+
				" <operater> <input> <output>\n"+
				"\tOperaters:\n"+
				"\t\tbin              read text and print binaries\n"+
				"\t\ttxt              read binaries and print text\n"+
				"\t\tcmp              compare binary nbt file.\n");
	}
}
