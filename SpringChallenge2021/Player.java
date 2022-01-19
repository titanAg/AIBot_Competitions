
// Kyle Orcutt
// CodingGame - Spring Challenge 2021
// Contest Rank: 2256/6867 (Silver League)

// https://www.codingame.com/multiplayer/bot-programming/spring-challenge-2021

// "The game takes place in a forest, in which gentle wood spirits reside. 
//  Their job is to make sure trees complete their lifecycle.
//  Grow trees at strategic locations of the forest to maximize your points.
//  Inspired by the board game Photosynthesis."

import java.util.*;

class Player {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int numberOfCells = in.nextInt(); // 37
        Map<Integer, Cell> cellMap = new HashMap<>();
        ArrayList<Cell> cells = new ArrayList<>();
        for (int i = 0; i < numberOfCells; i++) {
            int index = in.nextInt(); // 0 is the center cell, the next cells spiral outwards
            int richness = in.nextInt(); // 0 if the cell is unusable, 1-3 for usable cells
            int neigh0 = in.nextInt(); // the index of the neighbouring cell for each direction
            int neigh1 = in.nextInt();
            int neigh2 = in.nextInt();
            int neigh3 = in.nextInt();
            int neigh4 = in.nextInt();
            int neigh5 = in.nextInt();
            cells.add(new Cell(index, richness, neigh0, neigh1, neigh2, neigh3, neigh4, neigh5, null));
            cellMap.put(index, new Cell(index, richness, neigh0, neigh1, neigh2, neigh3, neigh4, neigh5, null));
        }

        // game loop
        while (true) {
            int day = in.nextInt(); // the game lasts 24 days: 0-23
            int nutrients = in.nextInt(); // the base score you gain from the next COMPLETE action
            int sun = in.nextInt(); // your sun points
            int score = in.nextInt(); // your current score
            int oppSun = in.nextInt(); // opponent's sun points
            int oppScore = in.nextInt(); // opponent's score
            boolean oppIsWaiting = in.nextInt() != 0; // whether your opponent is asleep until the next day

            int[] treeCounts = new int[4]; // counts of each level
            int[] oppTreeCounts = new int[4]; // opponent counts of each level
            int numberOfTrees = in.nextInt(); // the current amount of trees
            for (int i = 0; i < numberOfTrees; i++) {
                int cellIndex = in.nextInt(); // location of this tree
                int size = in.nextInt(); // size of this tree: 0-3
                boolean isMine = in.nextInt() != 0; // 1 if this is your tree
                boolean isDormant = in.nextInt() != 0; // 1 if this tree is dormant
                if (isMine) {
                    treeCounts[size]++;
                } else {
                    oppTreeCounts[size]++;
                }
                cellMap.get(cellIndex).tree = new Tree(cellIndex, size, isMine, isDormant);
            }
            int numberOfPossibleActions = in.nextInt(); // all legal actions
            if (in.hasNextLine()) {

                in.nextLine();
            }
            //Map<Integer, String> actionMap = new HashMap<>();
            Map<String, ArrayList<String>> actionMap = new HashMap<>();
            ArrayList<Action> actions = new ArrayList<>();
            for (int i = 0; i < numberOfPossibleActions; i++) {
                String possibleAction = in.nextLine(); // try printing something from here to start with
                if (!possibleAction.startsWith("WAIT")) {
                    System.err.println(possibleAction);
                    String[] s = possibleAction.split(" ");

                    ArrayList<String> list = actionMap.containsKey(s[0]) ?
                                             actionMap.get(s[0]) :
                                             new ArrayList<>();
                    list.add(s[0].startsWith("SEED") ? s[1] + " " + s[2] : s[1]);
                    actionMap.put(s[0], list);

                    Action action = new Action(s[0],
                                               s.length > 2 ? Integer.parseInt(s[1]) : -1,
                                               s.length > 2 ? Integer.parseInt(s[2]) : Integer.parseInt(s[1]));
                    actions.add(action);
                }
            }

            State state = new State(day,
                                    nutrients,
                                    sun,
                                    score,
                                    oppSun,
                                    oppScore,
                                    oppIsWaiting,
                                    actionMap,
                                    actions,
                                    cellMap,
                                    treeCounts,
                                    oppTreeCounts);

            //System.err.println(state);

            Turn turn = new Turn(state);
            Action action = turn.maxValueCommand();
            System.err.println("TAKING-> " + action);
            if (state.treeTotal() < 2 && state.daysLeft > 4) {
                boolean sent = false;
                for (Action a : state.actions) {
                    if (a.type.equals("SEED")) {
                        a.sendCommand();
                        sent = true;
                        break;
                    }
                }
                if (!sent)
                    System.out.println("WAIT");
            }
            else if (action != null) {
//                action.sendCommand();
                boolean shouldSeed = action.type.equals("SEED") && state.daysLeft > 4;
                boolean shouldGrow = action.type.equals("GROW") && state.daysLeft > 1;
                if (shouldSeed || shouldGrow || action.type.equals("COMPLETE")) {
                    action.sendCommand();
                } else {
                    System.out.println("WAIT");
                }
            } else {
                System.out.println("WAIT");
            }

            // GROW cellIdx | SEED sourceIdx targetIdx | COMPLETE cellIdx | WAIT <message>

            initTrees(cellMap);
        }
    }

    public static void initTrees(Map<Integer,Cell> cellMap) {
        for (Integer k : cellMap.keySet()) {
            cellMap.get(k).tree = null;
        }
    }

}

class Turn {
    State state;

    public Turn(State state) {
        this.state = state;
    }

    public Action maxValueCommand() {
        int max = Integer.MIN_VALUE;
        Map<Integer, ArrayList<Action>> tempMap = new HashMap<>();
        for (Action action : state.actions) {
            int value =  valueOfAction(action);
//            int value =  valueOfAction(action) - costOfAction(action);
            System.err.println(action + " totalV: " + value + " baseV: " + valueOfAction(action));

            if (value >= max) {
                max = value;
                ArrayList<Action> s = tempMap.get(max);
                if (s == null) {
                    s = new ArrayList<>();
                }
                s.add(action);
                tempMap.put(max, s);
            }
        }

        if (max > Integer.MIN_VALUE) {
//            int tempMax = 0;
//            Action action = null;
//            for (Action a : tempMap.get(max)) {
//                int richness = state.cellMap.get(a.targetI).richness;
//                if (richness > tempMax) {
//                    tempMax = richness;
//                    action = a;
////                    System.err.println(action + " cost: " + min);
//                }
//            }
//            return action;
            return tempMap.get(max).get(0);
        } else {
            return null;
        }
    }

    public int costOfAction(Action action) {
        switch (action.type) {
            case "SEED":
                return state.treeCounts[0];
            case "GROW":
                Tree tree = state.cellMap.get(action.targetI).tree;
                return tree.size == 0 ?
                       1 + state.treeCounts[0] :     // 0->1 cost
                       tree.size == 1 ?
                       3 + state.treeCounts[1] : // 1->2 cost
                       7 + state.treeCounts[2];  // 2->3 cost
            case "COMPLETE":
                return 4;
        }
        return 0;
    }

    public int valueOfAction(Action action) {
        Cell target = state.cellMap.get(action.targetI);
        if (action.type.equals("COMPLETE")) {
//            return state.nutrients + target.richness;
            return state.daysLeft < 4 || state.treeCounts[3] > 5 ? (state.nutrients + target.richness)+ state.day : 0;
        } else if (action.type.equals("GROW")){
//            return (target.tree.size+1) * state.daysLeft + target.richness;
            return (target.tree.size+1)*(target.tree.size+1) + target.richness;
        }
        else if (action.type.equals("SEED")){
//            return state.daysLeft + target.richness;
            int neighCount = state.getMyNeighborCount(action.targetI);
//            System.err.println("id " + action.targetI + " has " + neighCount + " neighbors");
            return target.richness - neighCount;
        }
        return 0;
    }
}

class State {
    int day; // the game lasts 24 days: 0-23
    int daysLeft;
    int nutrients; // the base score you gain from the next COMPLETE action
    int sun; // your sun points
    int score; // your current score
    int oppSun; // opponent's sun points
    int oppScore; // opponent's score
    int sunPos; // 0-5 -> east, northE, northW, west, southW, southE
    int[] treeCounts; // counts of each level
    int[] oppTreeCounts; // opponent counts of each level
    boolean oppIsWaiting; // whether your opponent is asleep until the next day
    Map<String, ArrayList<String>> actionMap; // all non-wait actions
    ArrayList<Action> actions;
    Map<Integer, Cell> cellMap = new HashMap<>();

    public State(int day,
                 int nutrients,
                 int sun,
                 int score,
                 int oppSun,
                 int oppScore,
                 boolean oppIsWaiting,
                 Map<String, ArrayList<String>> actionMap,
                 ArrayList<Action> actions,
                 Map<Integer, Cell> cellMap,
                 int[] treeCounts,
                 int[] oppTreeCounts) {
        this.day = day;
        this.nutrients = nutrients;
        this.sun = sun;
        this.score = score;
        this.oppSun = oppSun;
        this.oppScore = oppScore;
        this.oppIsWaiting = oppIsWaiting;
        this.sunPos = day % 6;
        this.actionMap = actionMap;
        this.actions = actions;
        this.cellMap = cellMap;
        this.treeCounts = treeCounts;
        this.oppTreeCounts = oppTreeCounts;
        this.daysLeft = 23-day;
    }

    public int getMyNeighborCount(int cellId) {
        Cell cell = cellMap.get(cellId);
        int count = 0;
        for (int i = 0; i < cell.neighbors.length; i++) {
            if (cell.neighbors[i] > -1 && cellMap.get(cell.neighbors[i]).tree != null)
                count++;
        }
        return count;
    }

    private boolean isMyNeighbor(Cell c1, Cell c2) {
        return c1.neighbors2.contains(c2.index);
    }

    public int treeTotal() {
        return treeCounts[0]+treeCounts[1]+treeCounts[2]+treeCounts[3];
    }

    public String getRichestIndex(String command) {
        String best = "";
        int highestN = 0;
        for (String s : actionMap.get(command)) {
//            System.err.println("Checking for richness: " + s);
            int cellKey = Integer.parseInt(!s.contains(" ") ? s : s.split(" ")[1]);
//            System.err.println("Checking for richness: " + cellMap.get(cellKey));
            int currentN = cellMap.get(cellKey).richness;
            if (currentN > highestN) {
                best = s;
                highestN = currentN;
            }
        }
        return best;
    }

    public void calculateMoves() {

    }

    @Override
    public String toString() {
        return "State{" +
               "day=" + day +
               ", daysLeft=" + daysLeft +
               ", nutrients=" + nutrients +
               ", sun=" + sun +
               ", score=" + score +
               ", oppSun=" + oppSun +
               ", oppScore=" + oppScore +
               ", sunPos=" + sunPos +
               ", treeCounts=" + Arrays.toString(treeCounts) +
               ", oppTreeCounts=" + Arrays.toString(oppTreeCounts) +
               ", oppIsWaiting=" + oppIsWaiting +
               ", actionMap=" + actionMap +
               ", actions=" + actions +
               ", cellMap=" + cellMap +
               '}';
    }
}

class Action {
    String type;
    int sourceI;
    int targetI;

    public Action(String type, int sourceI, int targetI) {
        this.type = type;
        this.sourceI = sourceI;
        this.targetI = targetI;
    }

    public void sendCommand() {
        System.out.println(type + (sourceI > -1 ? " " + sourceI : "") + " " + targetI);
    }

    @Override
    public String toString() {
        return "Action{" +
               "type='" + type + '\'' +
               ", sourceI=" + sourceI +
               ", targetI=" + targetI +
               '}';
    }
}

class Cell {
    int index; // 0 is the center cell, the next cells spiral outwards
    int richness; // 0 if the cell is unusable, 1-3 for usable cells
    int neighbors[]; // the index of the neighbouring cell for each direction
    Set<Integer> neighbors2 = new HashSet<>();
    Tree tree;

    public Cell(int index,
                int richness,
                int neigh0,
                int neigh1,
                int neigh2,
                int neigh3,
                int neigh4,
                int neigh5,
                Tree tree) {
        this.index = index;
        this.richness = richness;
        this.neighbors = new int[]{neigh0, neigh1, neigh2, neigh3, neigh4, neigh5};
        this.tree = tree;
        neighbors2.add(neigh0);
        neighbors2.add(neigh1);
        neighbors2.add(neigh2);
        neighbors2.add(neigh3);
        neighbors2.add(neigh4);
        neighbors2.add(neigh5);
    }

    @Override
    public String toString() {
        return "Cell{" +
               "index=" + index +
               ", richness=" + richness +
               ", neigh0=" + neighbors[0] +
               ", neigh1=" + neighbors[1] +
               ", neigh2=" + neighbors[2] +
               ", neigh3=" + neighbors[3] +
               ", neigh4=" + neighbors[4] +
               ", neigh5=" + neighbors[5] +
               ", \ntree=" + tree +
               '}';
    }
}

class Tree {
    int cellIndex; // location of this tree
    int size; // size of this tree: 0-3
    boolean isMine; // 1 if this is your tree
    boolean isDormant; // 1 if this tree is dormant

    public Tree(int cellIndex, int size, boolean isMine, boolean isDormant) {
        this.cellIndex = cellIndex;
        this.size = size;
        this.isMine = isMine;
        this.isDormant = isDormant;
    }

    @Override
    public String toString() {
        return "Tree{" +
               "cellIndex=" + cellIndex +
               ", size=" + size +
               ", isMine=" + isMine +
               ", isDormant=" + isDormant +
               '}';
    }
}