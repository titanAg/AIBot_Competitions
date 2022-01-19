// Kyle Orcutt
// CodingGame - A Code of Fire and Ice Contest
// Contest Rank: 694/2160 (Silver League)

import java.util.*;
import java.io.*;
import java.math.*;

class Player {
	
    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        Game g = new Game();      
        
        g.startInput(in);
        // game loop
        while (true) {           
            g.turnInput(in);
            g.output();
        }
    }
}

class Game{
	ArrayList<Building> buildings;
	ArrayList<Unit> units;
	boolean isUsed = false;
    int myUnits = 0;
    String mineSpots = "";
	String oppSpots = "";
	String mySpots = "";
    String[][] board;
    String[][] mine = new String[12][12];
    int mineCost = 20;
    int mineCount = 0;
	
	int gold, income;
	public Game() {
		buildings = new ArrayList<Building>();
		units = new ArrayList<Unit>();
	}
    
    public void startInput(Scanner in) {
        fillMines();
        int numberMineSpots = in.nextInt();
        for (int i = 0; i < numberMineSpots; i++) {
            int x = in.nextInt();
            int y = in.nextInt();
            mine[x][y] = "y";
            System.err.println(mineSpots);
        }
    }
    
    public void fillMines(){
        for (int i = 0; i < 12; i++){
            for (int j = 0; j < 12; j++){
                mine[j][i] = ".";   
            }
        }
    }
	
	public void turnInput(Scanner in) {
		buildings.clear();
		units.clear();
		myUnits = 0;
		board = new String[12][12];
		oppSpots = "";
		mySpots = "";
		mineCount = 0;
		
        gold = in.nextInt();
        income = in.nextInt();
        int opponentGold = in.nextInt();
        int opponentIncome = in.nextInt();
        for (int i = 0; i < 12; i++) {
            String line = in.next();
            for (int j = 0; j < line.length(); j++){
                board[j][i] = Character.toString(line.charAt(j));
            }
        }
        int buildingCount = in.nextInt();
        for (int i = 0; i < buildingCount; i++) {
            int owner = in.nextInt();
            int buildingType = in.nextInt();
            int x = in.nextInt();
            int y = in.nextInt();
            if (owner == 0 && buildingType == 0)
                board[x][y] = "h";
            if (buildingType == 1 && owner == 0){
                board[x][y] = "m";
                mineCount++;   
            }
            buildings.add(new Building(owner, buildingType, x, y));
        }
        int unitCount = in.nextInt();
        for (int i = 0; i < unitCount; i++) {
            int owner = in.nextInt();
            int unitId = in.nextInt();
            int level = in.nextInt();
            int x = in.nextInt();
            int y = in.nextInt();
            if (owner == 0){
                myUnits++;
                board[x][y] = "t";
                mySpots += String.format(" %d %d ,", x, y);
            }else{
                board[x][y] = "e";
                oppSpots += String.format(" %d %d ,", x, y);
            }
            units.add(new Unit(owner, unitId, level, x, y));
        }		
        printBoard();
	}
	
	public void output(){
        // To debug: System.err.println("Debug messages...");
            System.out.println(getCommand());
	}
	
	public String getCommand(){
	    String c = "";
	   // if (mineCount < 10)
	        c += buildMines();
	    c += buildTower();
	    c += trainUnits();

	    if (myUnits > 0){
            for (Unit unit : units){
                if (unit.owner == 0){
                    if (unit.level > 1)
                        c += "MOVE " + unit.id + " " + getClosestEnemy(unit) + "; ";
                    else if (getNextPos(unit).equals("WAIT"))
                        c += "WAIT; ";
                    else
                        c += "MOVE " + unit.id + " " + getNextPos(unit) + "; ";
                }
                // }else if (unit % 3 == 0)
                //     c += "MOVE " + Integer.toString(unit) + " " + getOpponentHQ() + "; ";
                //else
                    //c += "MOVE " + Integer.toString(unit) + " " + getClosestEnemy(unit) + "; ";
            }          
	    }else {
	       c += "WAIT; ";
	    }
	    return c.substring(0,c.length()-2);
	}
	
	public String trainUnits(){
	    String s = "";
	    for (int i = 0; i < 10; i++){
    	    if ((myUnits < 2 && gold >= 10) || gold >= 10 && income >= 2){
    	        s += "TRAIN 1 " + getTrainSpot(1) + "; ";
    	        gold -= 10;
    	    } 
    	    if (gold >= 20 && income >= 4){
    	        s += "TRAIN 2 " + getTrainSpot(2) + "; ";
    	        gold -= 20;
    	    }
       	    if (gold >= 30 && income >= 20){
    	        s +=   "TRAIN 3 " + getTrainSpot(3) + "; ";
    	        gold -= 30;
    	    }

	    }
	    return s;
	}
	
	public String getTrainSpot(int lvl){
	    String c = "";
	    int dis = 100;
	    int x = -1;
	    int y = -1;
	   // if (lvl > 1 && c.contains("0"))
	   //     target = 11;
	   // if (lvl == 1 && c.contains("11"))
	   //     target = 11;
	    for (int i = 0; i < 12; i++){
	        for (int j = 0; j < 12; j++){
	            if (lvl > 1){
    	           if (board[i][j].equals(".") || board[i][j].equals("X") || board[i][j].equals("x") || board[i][j].equals("e")){
    	               int distance = (int) Math.sqrt((getHQ() - i)*(getHQ() - i) + (getHQ() - j)*(getHQ() - j));
    	               if (distance < dis){
    	                    dis = distance;
    	                    c = String.format("%d %d", i, j);
    	                    x = i;
    	                    y = j;
    	               }
    	                    
    	           }
	            }else {
	                if (i > 0 && i < 11 && j > 0 && j < 11){
    	               if (board[i][j].equals("O") && board[i+getPosShift()][j].equals("X") || board[i+getPosShift()][j].equals(".") || board[i][j+getPosShift()].equals("X") || board[i][j+getPosShift()].equals(".")){
    	                    int distance = (int) Math.sqrt((getHQ() - i)*(getHQ() - i) + (getHQ() - j)*(getHQ() - j));
    	                    if (distance < dis){
        	                    dis = distance;
        	                    if (board[i+getPosShift()][j].equals("X") || board[i+getPosShift()][j].equals(".")){
        	                        c = String.format("%d %d", i+getPosShift(), j);
        	                        x = i+getPosShift();
        	                        y = j;
        	                    }if (board[i][j+getPosShift()].equals("X") || board[i][j+getPosShift()].equals(".")){
        	                        c = String.format("%d %d", i, j+getPosShift());
        	                        x = i;
        	                        y = j+getPosShift();
        	                    }
    	                    }
    	                    
    	                }
	                }
	            }
	        }   
	    }
	    if (c.equals("") || myUnits < 1 && lvl == 1){
	        return getMyHQ();
	    }
	    board[x][y] = "t";
	    return c;
	}
	
	public String buildMines(){
	    String s = "";
	    String c = getOpponentHQ();
	    int dis = 100;
	    int target = 0;
	    if (c.contains("0"))
	        target = 11;
        for (int i = 0; i < 12; i++){
            for (int j = 0; j < 12; j++){
                if (board[j][i].equals("O") && mine[j][i].equals("y")){
	               int distance = (int) Math.sqrt((target - i)*(target - i) + (target - j)*(target - j));
	               if (distance < dis){
	                    dis = distance;
	                    c = String.format("%d %d ", j, i);
	               }       
                }
            }
	    }
	    if (gold > mineCost && !c.equals(getOpponentHQ())){
	       s = "BUILD MINE " + c + "; ";
	       mineCost+=4;
	    }
        return s;   
	}
	
	public String buildTower(){
	    String s = getOpponentHQ();
	    if (s.contains("11"))
	        s = "1 1";
	    else
	        s = "10 10";
	    if (gold > 15){
	        return "BUILD TOWER " + s + "; ";
	        //return "BUILD TOWER " + getTrainSpot() + "; ";
	    }
	   return "";
	}
	
	public int getPosShift(){
	    if (getOpponentHQ().contains("11"))
	        return 1;
	    else 
	        return -1;
	}
	
	public String getNextPos(Unit unit){
	    String c = getOpponentHQ();
	    int x = unit.x;
	    int y = unit.y;

	    int left = x+getPosShift();
	    int right = x+(getPosShift()*-1);
	    int up = y+getPosShift();
	    int down = y+(getPosShift()*-1);
	    int target = 0;
	   // if (c.contains("11"))
	   //     target = 11;
	   
	    int dis1 = (int) Math.sqrt((getOHQ() - left)*(getOHQ() - left) + (getOHQ() - y)*(getOHQ() - y));
        int dis2 = (int) Math.sqrt((getOHQ() - right)*(getOHQ() - right) + (getOHQ() - y)*(getOHQ() - y));
        int dis3 = (int) Math.sqrt((getOHQ() - x)*(getOHQ() - x) + (getOHQ() - up)*(getOHQ() - up));
        int dis4 = (int) Math.sqrt((getOHQ() - x)*(getOHQ() - x) + (getOHQ() - down)*(getOHQ() - down));
        
        if (left >= 0 && left <= 11 && (board[left][y].equals(".") || (board[left][y].equals("X") && !oppSpots.contains(String.format("%d %d", left, y))))){
                c = String.format("%d %d", left, y);
        }
        if (up >= 0 && up <= 11 && (board[x][up].equals(".") || board[x][up].equals("X")  && !oppSpots.contains(String.format("%d %d", x, up)))){
                c = String.format("%d %d", x, up);
        }
        if (right >= 0 && right <= 11 && (board[right][y].equals(".") || board[right][y].equals("X")  && !oppSpots.contains(String.format("%d %d", right, y)))){
                c = String.format("%d %d", right, y);
        }
        if (down >= 0 && down <= 11 && (board[x][down].equals(".") || board[x][down].equals("X")  && !oppSpots.contains(String.format("%d %d", x, down)))){
                c =  String.format("%d %d", x, down);           
        }
        
        // else {
        //     c = "WAIT";
        // }                      
       return c;
	}
	
	public String getClosestEnemy(Unit unit){
	    int closest = 50;
	    String c = getOpponentHQ();
	    int x = unit.x;
	    int y = unit.y;
	            
       for (Unit u : units){
            if (u.owner == 1){
                int distance = (int) Math.sqrt((u.x - x)*(u.x - x) + (u.y - y)*(u.y - y));
                if (distance < closest){
                    int ps = getPosShift();
                    int targetX = u.x;
                    int targetY = u.y;
                    if (getMyHQ().contains(Integer.toString(u.x)))
                        targetX = u.x;
                    if (getMyHQ().contains(Integer.toString(u.y)))
                        targetY = u.y;
                        c = String.format("%d %d", targetX, targetY);
                    closest = distance; 
                }
            }
       }
       return c;
	}
	
	public ArrayList<Integer> getMyUnits(){
	    ArrayList<Integer> myUnitIds = new ArrayList<Integer>();
	    for (Unit unit : units){
	        if (unit.owner == 0)
	            myUnitIds.add(unit.id);
	    }
	    return myUnitIds;
	}
	
	public String getMyHQ(){
	    String p = "";
	    int targetX = 0;
	    int targetY = 0;
	    for (Building b : buildings){
	        if (b. owner == 0 && b.building == 0){
	            if (b.x > 0) {
	            	targetX = b.x-1;
	            	targetY = b.y-1;
	            }else {
	            	targetX = b.x+1;
	            	targetY = b.y+1;
	                
	            }
	            if (isUsed)
	            	p = String.format("%d %d", targetX, b.y);
	            else
	            	p = String.format("%d %d", b.x, targetY);
	            isUsed = !isUsed;
	        }
	    }
	    return p;
	}
	
	public int getHQ(){
	    String s = getMyHQ();
	    if (s.contains("11"))
	        return 11;
	    return 0;
	}
	
	public int getOHQ(){
	    String s = getOpponentHQ();
	    if (s.contains("11"))
	        return 11;
	    return 0;
	}
	
	public String getOpponentHQ(){
		String p = "";
		for (Building b : buildings) {
			if (b.owner == 1 && b.building == 0)
				p = String.format("%d %d", b.x, b.y);
		}
		return p;
	}
	
	public void printBoard(){
	    for (int i = 0; i < 12; i++){
	        for (int j = 0; j < 12; j++){
	            System.err.print(board[j][i]);   
	        }
	        System.err.println();
	    }
	}
}


class Unit{
	int owner;
	int level;
	int id;
	int x;
	int y;
	public Unit(int o, int id, int l, int x, int y) {
		owner = o;
		level = l;
		this.id = id;
		this.x = x;
		this.y = y;
	}
}
    
class Building{
	int owner;
	int building;
	int x;
	int y;
    public Building(int o, int bt, int x, int y) {
    	owner = o;
    	building = bt;
    	this.x = x;
    	this.y = y;
    }
}
