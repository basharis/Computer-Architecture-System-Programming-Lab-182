import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.*;

/*
 *	bugs and problems:
 * 		1. do not support cases like a label1 which holds a label2+offset or -label2 or label1 which holds label2+label3 etc.
*/

public class Main {
	
	public static void main(String[] args) {
		
		if(args.length == 0){
			System.out.println("Please enter an input file");
			return;
		}
		
		Hashtable<String, Integer> ht = new Hashtable<String, Integer>();
		
		File inputFile = new File(args[0]);
		File outputFile = new File("sbnFormat.txt");
		
		try {
			BufferedReader br = new BufferedReader(new FileReader(inputFile));
			int linesCounter = 0, variablesCounter = 0;
			String line;
		    while ((line = br.readLine()) != null) {
		    	if(line.indexOf(";") >= 0){
		    		line = line.substring(0, line.indexOf(";"));
		    	}
		    	line = line.trim();
		    	if(line.indexOf(")") >= 0 && (line.indexOf(";") < 0 || line.indexOf(";") > line.indexOf(")"))){
		    		line = line.substring(line.indexOf(")") + 1);
		    	}
		    	line = line.trim();
		    	if(line.length() == 0){
		    		continue;
		    	}
		    	String[] lineParts = line.split("\\s+");
		    	if(lineParts.length >= 2) {
		    		if(lineParts[0].indexOf(":") >= 0){
		    			String label = lineParts[0].substring(0, lineParts[0].indexOf(":"));
		    			ht.put(label, linesCounter * 3 + variablesCounter);
		    		}
		    		if(lineParts.length >= 3){
		    			int plusIndex = 0;
		    			if(lineParts[0].indexOf(":") >= 0){
		    				plusIndex++;
		    			}
		    			if(lineParts[plusIndex + 2].equals("+")){
		    				ht.put("+" + (linesCounter + variablesCounter), linesCounter * 3 + 3 + variablesCounter);
		    			}
		    			linesCounter++;
		    		}
		    	}
		    	if(lineParts.length <= 2) {
		    		if(lineParts.length == 2){
		    			lineParts[0] = lineParts[1];
		    		}
		    		ht.put(lineParts[0], linesCounter * 3 + variablesCounter);
		    		variablesCounter++;
		    	}
		    }
		    
		    br = new BufferedReader(new FileReader(inputFile));
		    FileWriter writer = new FileWriter(outputFile);
		    linesCounter = 0;
		    variablesCounter = 0;
		    
		    line = br.readLine();
		    while(line != null){
		    	if(line.indexOf(";") >= 0){
		    		line = line.substring(0, line.indexOf(";"));
		    	}
		    	line = line.trim();
		    	if(line.indexOf(")") >= 0 && (line.indexOf(";") < 0 || line.indexOf(";") > line.indexOf(")"))){
		    		line = line.substring(line.indexOf(")") + 1);
		    	}
		    	line = line.trim();
		    	if(line.length() == 0){
		    		line = br.readLine();
		    		continue;
		    	}
		    	String[] lineParts = line.split("\\s+");
		    	if(lineParts.length <= 2){
		    		if(lineParts.length == 2){
		    			while(!isNumeric(lineParts[1])){
		    				lineParts[0] = lineParts[1];
		    				lineParts[1] = "" + ht.get(lineParts[1]);
		    			}
		    			lineParts[0] = lineParts[1];
		    		}
		    		writer.write(lineParts[0]);
		    		variablesCounter++;
		    	}
		    	else {
		    		int partNumber = 0;
		    		if(lineParts[0].length() == 0 || lineParts[0].indexOf(":") >= 0){
		    			partNumber = 1;
		    		}
			    	while(partNumber < lineParts.length){
			    		if(lineParts[partNumber].equals("0")){
			    			writer.write("0");
			    		}
			    		else if(lineParts[partNumber].equals("+")){
			    			writer.write("" + ht.get("+" + (linesCounter + variablesCounter)));
			    		}
			    		else {
			    			int toAdd = 0;
			    			String[] labelAndNumber = {lineParts[partNumber]};
			    			if(lineParts[partNumber].indexOf("+") > 0){
			    				labelAndNumber = lineParts[partNumber].split("\\+");
			    			}
			    			if(labelAndNumber.length > 1) {
			    				toAdd = Integer.parseInt(labelAndNumber[1]);
			    			}
			    			int toWrite = ht.get("" + labelAndNumber[0]);
			    			toWrite += toAdd;
			    			writer.write("" + toWrite);
			    		}
			    		
			    		if(partNumber < lineParts.length - 1 && lineParts[partNumber + 1].substring(0,1).equals(";")){
			    			break;
			    		}
			    		
			    		if(partNumber < lineParts.length - 1){
			    			writer.write("\t");
			    		}
			    		partNumber++;
			    	}
			    	linesCounter++;
		    	}
		    	
		    	if((line = br.readLine()) != null){
		    		writer.write("\n");
		    	}
		    }
		    
		    writer.write("\n");
		    writer.flush();
		    writer.close();
		}
		catch(Exception e){
			e.printStackTrace();
		}

	}
	
	public static boolean isNumeric(String str){  
		try {  
			double d = Double.parseDouble(str);  
		}
		catch(NumberFormatException nfe){  
			return false;
		}  
		return true;  
	}

}
