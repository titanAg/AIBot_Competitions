import java.util.*;

// Kyle Orcutt
// CodingGame - Fall Challenge 2020 (Monte Carlo tree search, BFS, graph traversal) 
// https://www.codingame.com/multiplayer/bot-programming/fall-challenge-2020 
// Contest Rank: 1489/7011

class Player {
    //Game variables
    private static ArrayList<Recipe> potions = new ArrayList<Recipe>();
    private static ArrayList<Recipe> previousPotions = new ArrayList<Recipe>();
    //private static ArrayList<Recipe> spells = new ArrayList<Recipe>();
    private static ArrayList<Recipe> tomes = new ArrayList<Recipe>();
    private static ArrayList<Recipe> allSpells = new ArrayList<Recipe>();
    private static ArrayList<Tier> tiers = new ArrayList<Tier>();
    private static Storage myInv = null;
    private static int myScore = 0;
    private static int opScore = 0;

//    ArrayList<ArrayList<Recipe>> sortedSpells = new ArrayList<ArrayList<Recipe>>(); // 0 = blue, 1 = green, 2 = orange, 3 = yellow

    private static HashMap<Integer, Recipe> spellMap = new HashMap<Integer, Recipe>();
    private static HashMap<Integer, Recipe> tomeMap = new HashMap<Integer, Recipe>();
    private static HashMap<Integer, Recipe> potionMap = new HashMap<Integer, Recipe>();

    private static int potionCount = 0;

    private static void runTurnCleanup() {
        tiers.clear();
        potions.clear();
        //spells.clear();
        allSpells.clear();
        tomes.clear();

        spellMap.clear();
        tomeMap.clear();
        potionMap.clear();
    }

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        ArrayList<Recipe> spells = new ArrayList<Recipe>();
        // game loop
        while (true) {
            int actionCount = in.nextInt(); // the number of spells and recipes in play
            for (int i = 0; i < actionCount; i++) {
                int actionId = in.nextInt(); // the unique ID of this spell or recipe
                String actionType = in.next(); // in the first league: BREW; later: CAST, OPPONENT_CAST, LEARN, BREW
                int delta0 = in.nextInt(); // tier-0 ingredient change
                int delta1 = in.nextInt(); // tier-1 ingredient change
                int delta2 = in.nextInt(); // tier-2 ingredient change
                int delta3 = in.nextInt(); // tier-3 ingredient change
                int price = in.nextInt(); // the price in rupees if this is a potion
                int tomeIndex = in.nextInt(); // in the first two leagues: always 0; later: the index in the tome if this is a tome spell, equal to the read-ahead tax
                int taxCount = in.nextInt(); // in the first two leagues: always 0; later: the amount of taxed tier-0 ingredients you gain from learning this spell
                boolean castable = in.nextInt() != 0; // in the first league: always 0; later: 1 if this is a castable player spell
                boolean repeatable = in.nextInt() != 0; // for the first two leagues: always 0; later: 1 if this is a repeatable player spell

                if (actionType.equals("BREW")) {
                    potions.add(new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0));
                    potionMap.put(actionId, new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0));
                }
                if (actionType.equals("CAST")) {
                    Recipe spell1 = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0);
                    Recipe spell2 = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0);
                    Recipe spell3 = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0);
                    spell1.setCastable(castable);
                    spell2.setCastable(castable);
                    spell3.setCastable(castable);
                    spells.add(spell1);

                    spellMap.put(actionId,spell2);
                    allSpells.add(spell3);
                    allSpells.get(spells.size() - 1 ).setCastable(castable);
                }
                if (actionType.equals("LEARN")) {
                    Recipe tomeRecipe = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0);
                    Recipe tome = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, 0);
                    tomes.add(tome);
                    tomeMap.put(actionId, tome);
                    allSpells.add(tomeRecipe);
                }
            }
            //TODO call method updateScores()
            //debug("All spells \n" + allSpells);
            for (int i = 0; i < 2; i++) {
                int inv0 = in.nextInt(); // tier-0 ingredients in inventory
                int inv1 = in.nextInt();
                int inv2 = in.nextInt();
                int inv3 = in.nextInt();
                int score = in.nextInt(); // amount of rupees
                if (i == 0) {
                    myInv = new Storage(inv0, inv1, inv2, inv3, score);
                    tiers.add(new Tier(0, inv0, 0.5));
                    tiers.add(new Tier(1, inv1, 1.5));
                    tiers.add(new Tier(2, inv2, 2.5));
                    tiers.add(new Tier(3, inv3, 3.5));
                    tiers = getMatValues(spells);

                    debug(myInv.toString());
                }
            }
            updateScores();
            /*********************************************
             * Sim Code (incomplete)
             * TODO build proper tree and add BFS/MCTS/Minimax 
             *********************************************/
            ArrayList<Simulation> turns = new ArrayList<Simulation>();
            for (Recipe potion : potions) {
                ArrayList<Recipe> tempSpells = new ArrayList<Recipe>();
                ArrayList<Recipe> temp = spells.size() < 13 ? allSpells : spells;
                for (Recipe spell : temp) {
                    tempSpells.add(spell.getObject());
                }
                int[] reqItems = getRequiredItems(potion);
                Simulation turn = getTurnsRequired(potion, new ArrayList<Recipe>(), 0, tempSpells, reqItems);
                turns.add(turn);

                debug("turns for potion " + potion.getActionId() + " is: " + turn.getMoveCount());
                double price = turn.getTargetPotion().getPrice() / turn.getMoveCount() + turn.getTargetPotion().getPrice();
                debug("Rupees gained per turn: " + price + "\n");
            }
            Simulation targetTurn1 = getTargetTurn(turns);
            debug("Turn 1 targeting potion " + targetTurn1.getTargetPotion().getActionId());

            Simulation targetTurn2 = getTargetTurn(turns);
            debug("Turn 2 targeting potion " + targetTurn2.getTargetPotion().getActionId());

            Simulation targetTurn = targetTurn1.getTargetPotion().getPrice() > targetTurn2.getTargetPotion().getPrice() ? targetTurn1 : targetTurn2;
//            Simulation targetTurn = targetTurn1;
            debug("MOVES " + targetTurn1.getMoves());
//            if (learnTaxedTome()){
//                debug("LEARNING TAXED TOME");
//            }
            if (targetTurn.getMoveCount() == 1) {
                potionCount++;
                System.out.println("BREW " + targetTurn.getTargetPotion().getActionId());
            }
            else {
                ArrayList<ArrayList<Recipe>> moves = getBestMoves(targetTurn);

                for (ArrayList<Recipe> move : moves) {
                    debug(move.toString());
                }

                if (moves.get(0).size() > 0) {
//                    debug("TOME MOVES " + moves.toString());
                    makeBestMove(moves, spells);
                }
                else if (moves.get(1).size() > 0) {
                    makeBestMove(moves, spells);
                }
                else if (moves.get(2).size() > 0) {
                    sendSpellCommand(moves.get(2).get(0));
                }
                else {
                    makeOtherMove(moves.get(3), spells);
                }
            }
            //debug("POTION COUNT " + potionCount);

            for (Recipe potion : potions) {
                previousPotions.add(potion.getObject());
            }
            spells.clear();
            runTurnCleanup();
        }
    }

    public static void updateScores() {
//        debug(previousPotions);
        previousPotions.clear();
    }

    public static void learnBestTome() {
        Recipe targetTome = tomes.get(0);
//        double bestScore = targetTome.getDelta0() * 0.5 + targetTome.getDelta1() * 1.5 +
//                           targetTome.getDelta2() * 2.5 + targetTome.getDelta3() * 3.5 -
//                           targetTome.getTomeIndex() * 0.5;
//        for (Recipe tome : tomes) {
//            double score = tome.getDelta0() * 0.5 + tome.getDelta1() * 1.5 +
//                           tome.getDelta2() * 2.5 + tome.getDelta3() * 3.5 -
//                           tome.getTomeIndex() * 0.5;
//            if (isLearnable(tome) && score > bestScore) {
//                targetTome = tome;
//                bestScore = score;
//            }
//        }
        System.out.println("LEARN " + targetTome.getActionId());
    }

    private static boolean learnTaxedTome() {
        Recipe targetTome = null;
        for (Recipe tome : tomes) {
            if (tome.getTomeIndex() < 1) {
                int totalInv = myInv.getInv0() + myInv.getInv1() + myInv.getInv2() + myInv.getInv3();
                int tomeTax = tome.getTaxCount();
                if (totalInv + tomeTax <= 10 && tomeTax > 2) {
                    targetTome = tome;
                    break;
                }
            }
        }
        if (targetTome != null) {
            System.out.println("LEARN " + targetTome.getActionId());
            return true;
        }
        return false;
    }

    private static boolean isLearnable(Recipe tome) {
        return tome.getTomeIndex() <= myInv.getInv0();
    }

    // Calculates material value based on input recipes
    public static ArrayList<Tier> getMatValues(ArrayList<Recipe> recipes) {
        //TODO
        ArrayList<Tier> tempTiers = tiers;
        for (Tier t : tiers) {
            for (Recipe r : recipes) {
                int[] rItems = r.getItemArray();
                double gainedItems = rItems[t.getId()];

                if (gainedItems > 0) {
                    double turns = 1;
                    double tier0 = rItems[0] < 0 ? Math.abs(rItems[0]) * tiers.get(0).getValue() : 0;
                    double tier1 = rItems[1] < 0 ? Math.abs(rItems[1]) * tiers.get(1).getValue() : 0;
                    double tier2 = rItems[2] < 0 ? Math.abs(rItems[2]) * tiers.get(2).getValue() : 0;
                    double tier3 = rItems[3] < 0 ? Math.abs(rItems[3]) * tiers.get(3).getValue() : 0;
                    double score = turns / gainedItems + tier0 + tier1 + tier2 + tier3;
                    if (score < tempTiers.get(t.getId()).getValue()) {
                        tempTiers.get(t.getId()).setValue(score);
                    }
                }
            }
        }
        return tempTiers;
    }

    public static Simulation getTargetTurn(ArrayList<Simulation> turns) {
        double bestRupeeRate = turns.get(0).getTargetPotion().getPrice() / turns.get(0).getMoveCount() + turns.get(0).getTargetPotion().getPrice();
//        double bestRupeeRate = turns.get(0).getMoveCount();
//        double bestRupeeRate = turns.get(0).getTargetPotion().getPrice();
        Simulation targetTurn = turns.get(0);
        for (Simulation turn : turns) {
            double rupeeRate = turn.getTargetPotion().getPrice() / turn.getMoveCount() + turn.getTargetPotion().getPrice();
//            double rupeeRate = turn.getMoveCount();
//            double rupeeRate = turn.getTargetPotion().getPrice();
            if (rupeeRate > bestRupeeRate) {
                bestRupeeRate = rupeeRate;
                targetTurn = turn;
            }
        }
        turns.remove(targetTurn);
        return targetTurn;
    }

    // Gets a categorized list of moves
    public static ArrayList<ArrayList<Recipe>> getBestMoves(Simulation turn) {
        ArrayList<ArrayList<Recipe>> moves = new ArrayList<ArrayList<Recipe>>();
        moves.add(new ArrayList<Recipe>()); // Tomes index 0
        moves.add(new ArrayList<Recipe>()); // Castable and playable index 1
        moves.add(new ArrayList<Recipe>()); // Not castable but still playable index 2
        moves.add(new ArrayList<Recipe>()); // Not playable index 3
        // TODO account for repeatable spells
        for (Recipe spell : turn.getMoves()) {
            Recipe spellMapping = spellMap.get(spell.getActionId());

            if (tomeMap.containsKey(spell.getActionId())) {
                moves.get(0).add(tomeMap.get(spell.getActionId()));
            }
            else if (isPlayable(spellMapping)){
                //debug("Getting best spellMap " + spellMapping.toString());
                if (spellMapping.getCastable()) {
                    moves.get(1).add(spellMapping);
                }
                else {
                    moves.get(2).add(spellMapping);
                }
            }
            else {
//                debug("Getting best spellMap " + spellMapping.toString());
                moves.get(3).add(spellMapping);
            }
        }
        int i = 0;
        return moves;
    }

    // Makes best move for input moves
    public static void makeBestMove(ArrayList<ArrayList<Recipe>> moves, ArrayList<Recipe> spells) {
        // Tomes index 0, Castable index 1, Not castable index 2
        int targetIndex = 1;
        if (moves.get(0).size() > 0) {
            // tome action
            int lowest = moves.get(0).get(0).getTomeIndex();
            int index = 0;
            debug("lowest index " + lowest + " " + index);
            ArrayList<Recipe> tomeMoves = moves.get(0);
            for (int i = 0; i < tomeMoves.size(); i++) {
                boolean allPositive = tomeMoves.get(i).getDelta0() >= 0 &&
                                      tomeMoves.get(i).getDelta1() >= 0 &&
                                      tomeMoves.get(i).getDelta2() >= 0 &&
                                      tomeMoves.get(i).getDelta3() >= 0;
                if (allPositive) {
                    index = i;
                    break;
                }
                if (tomeMoves.get(i).getTomeIndex() <= lowest) {
                    lowest = tomeMoves.get(i).getTomeIndex();
                    index = i;
                }
            }
            sendSpellCommand(moves.get(0).get(index));
            return;
        }
        if (moves.get(1).size() > 0) {
            // send castable spell
            targetIndex = 1;
        }
        else if (moves.get(2).size() > 0) {
            targetIndex = 2;
        }
        sendSpellCommand(moves.get(targetIndex).get(0));
    }

    public static void makeOtherMove(ArrayList<Recipe> moves, ArrayList<Recipe> spells) {
        Recipe r = null;
        for (Recipe move : moves) {
            int[] reqItems = getRequiredItems(move);

//            debug("Looking at spell " + move.getActionId());
//            debug("Required items " + reqItems[0] + " " + reqItems[1] + " " + reqItems[2] + " " + reqItems[3]);

            for (int i = 0; i < reqItems.length; i++) {
                if (reqItems[i] < 0) {
                    for (Recipe spell : spells) {
                        int qty = spell.getItemArray()[i];
                        if (isPlayable(spell) && qty > 0) {
                            r = spell;
                        }
                    }
                }
            }
            if (r != null) {
                sendSpellCommand(r);
                return;
            }
            for (Recipe spell : spells) {
//                int qty = spell.getItemArray()[];
                if (isPlayable(spell)) {
                    sendSpellCommand(spell);
                    return;
                }
            }
        }
        for (Recipe potion : potions) {
            if (isPlayable(potion)){
                System.out.println("BREW " + potion.getActionId());
            }
        }
        System.out.println("WAIT");
    }

    // returns true if there are enough ingredients in inventory for input spell
    public static boolean isPlayable(Recipe spell) {
        int totalSize = spell.getDelta0() + spell.getDelta1() + spell.getDelta2() + spell.getDelta3() +
                        myInv.getInv0() + myInv.getInv1() + myInv.getInv2() + myInv.getInv3();
        int maxInvSize = 10;
        return spell.getDelta0() + myInv.getInv0() >= 0 &&
                spell.getDelta1() + myInv.getInv1() >= 0 &&
                spell.getDelta2() + myInv.getInv2() >= 0 &&
                spell.getDelta3() + myInv.getInv3() >= 0 &&
                totalSize <= maxInvSize;

    }

    // Calculates the turns required for input potion
    public static Simulation getTurnsRequired(Recipe targetPotion, ArrayList<Recipe> moves, double moveCount, ArrayList<Recipe> spellPool, int[] reqItems) {
        //debug("simulating turns for potion " + targetPotion + "\n");
        //debug("Moves.size() = " + moves.size());
        debug("Req Items: " + reqItems[0] + " " + reqItems[1] + " " + reqItems[2] + " " + reqItems[3]);
        Simulation turn = new Simulation(targetPotion, moves, moveCount);
        if (reqItems[0] >= 0 && reqItems[1] >= 0 && reqItems[2] >= 0 && reqItems[3] >= 0){
            // Add brew cost
            turn.setMoveCount(moveCount+1);
            return turn;
        }

        ArrayList<ArrayList<Recipe>> sortedSpells = new ArrayList<ArrayList<Recipe>>();
        sortedSpells.add(new ArrayList<Recipe>()); // blue 0
        sortedSpells.add(new ArrayList<Recipe>()); // green 1
        sortedSpells.add(new ArrayList<Recipe>()); // orange 2
        sortedSpells.add(new ArrayList<Recipe>()); // yellow 3
        for (int i = reqItems.length - 1; i >= 0; i--) {
            if (reqItems[i] < 0) {
                for (Recipe spell : spellPool) {
                    // TODO compare spells
                    int tomeIndex = spell.getTomeIndex();
                    //debug("LOOKING AT SPELL " + spell.toString());
                    if (spell.getActionType().equals("CAST") || tomeIndex <= myInv.getInv0() && tomeIndex < 3) {
                        if (reqItems[0]+spell.getDelta0() >= 0 && reqItems[1]+spell.getDelta1() >= 0 && reqItems[2]+spell.getDelta2() >= 0 && reqItems[3]+spell.getDelta3() >= 0){
                            // Add brew cost
                            moves.add(spell.getObject());
                            //debug("ADDING LAST SPELL TO MOVES " + spell);
                            turn = new Simulation(targetPotion, moves, moveCount+2);
                            //turn.setMoveCount(moveCount+1);
                            return turn;
                        }
                        int[] items = spell.getItemArray();
                        if (items[i] > 0) {
                            sortedSpells.get(i).add(spell);
                        }
                    }
                }
            }
        }
        Recipe targetSpell = null;
        if (sortedSpells.get(3).size() > 0 && reqItems[3] < 0) {
            targetSpell = getBestSpell(sortedSpells.get(3), reqItems);
        }
        else if (sortedSpells.get(2).size() > 0 && reqItems[2] < 0) {
            targetSpell = getBestSpell(sortedSpells.get(2), reqItems);
        }
        else if (sortedSpells.get(1).size() > 0 && reqItems[1] < 0) {
            targetSpell = getBestSpell(sortedSpells.get(1), reqItems);
        }
        else if (sortedSpells.get(0).size() > 0 && reqItems[0] < 0) {
            targetSpell = getBestSpell(sortedSpells.get(0), reqItems);
        }
        else {
            moveCount++;
            castInternalRest(spellPool);
            return getTurnsRequired(targetPotion, moves, moveCount, spellPool, reqItems);
        }
        if (targetSpell == null) {
            moveCount++;
            castInternalRest(spellPool);
            return getTurnsRequired(targetPotion, moves, moveCount, spellPool, reqItems);
        }
        targetSpell.setCastable(false);
        moves.add(targetSpell.getObject());
        moves.get(moves.size() - 1 ).setCastable(targetSpell.getCastable());
        moveCount++;
        int[] items = targetSpell.getItemArray();
        reqItems[0] += items[0];
        reqItems[1] += items[1];
        reqItems[2] += items[2];
        reqItems[3] += items[3];
        //TODO remove spell if temp
//                        debug("moves: " + moveCount);
//                        debug("turn Moves: " + turn.getMoveCount());
        return getTurnsRequired(targetPotion, moves, moveCount, spellPool, reqItems);
    }

    public static void castInternalRest(ArrayList<Recipe> s) {
        for (Recipe spell : s) {
            spell.setCastable(true);
        }
    }

    public static Recipe getBestSpell(ArrayList<Recipe> s, int[] recItems) {
        Recipe targetSpell = null;
        double highestScore = -20;
        for (Recipe r : s) {
            int spellSize = r.getDelta0() + r.getDelta1() + r.getDelta2() + r.getDelta3();
            int myInvSize = recItems[0] + recItems[1] + recItems[2] + recItems[3];
            if (spellSize + myInvSize <= 10) {
                int[] spellItems = r.getItemArray();
                double item0 = recItems[0] < 0 || spellItems[0] < 0 ? spellItems[0] * 0.5 : 0;
                double item1 = recItems[1] < 0 || spellItems[1] < 0 ? spellItems[1] * 1.5 : 0;
                double item2 = recItems[2] < 0 || spellItems[2] < 0 ? spellItems[2] * 2.5 : 0;
                double item3 = recItems[3] < 0 || spellItems[3] < 0 ? spellItems[3] * 3.5 : 0;
                double score = item0 + item1 + item2 + item3;
                if (score >= highestScore) {
                    highestScore = score;
                    targetSpell = r;
                }
            }
        }
        return targetSpell;
    }

    // Gets array of required items for inputted potion
    public static int[] getRequiredItems(Recipe potion) {
        return new int[] { potion.getDelta0() + myInv.getInv0(),
                           potion.getDelta1() + myInv.getInv1(),
                           potion.getDelta2() + myInv.getInv2(),
                           potion.getDelta3() + myInv.getInv3()};
    }

    public static double getScore(Recipe recipe) {
        double item0 = recipe.getDelta0() == 0 ? 0 : (recipe.getDelta0() + myInv.getInv0()) * - 1 * tiers.get(0).getValue();
        double item1 = recipe.getDelta1() == 0 ? 0 : (recipe.getDelta1() + myInv.getInv1()) * - 1 * tiers.get(1).getValue();
        double item2 = recipe.getDelta2() == 0 ? 0 : (recipe.getDelta2() + myInv.getInv2()) * - 1 * tiers.get(2).getValue();
        double item3 = recipe.getDelta3() == 0 ? 0 : (recipe.getDelta3() + myInv.getInv3()) * - 1 * tiers.get(3).getValue();
        double score = (item0 + item1 + item2 + item3) / ((double)recipe.getPrice() * 2);
        //debug("Score for potion: " + recipe.getActionId() + " is " + score);
        return score;
    }

    public static void sendSpellCommand(Recipe spell) {
        if (tomeMap.containsKey(spell.getActionId()) && tomeMap.get(spell.getActionId()).getTomeIndex() <= myInv.getInv0()) {
            System.out.println("LEARN " + spell.getActionId());
        }
        else if (spell.getCastable()) {
            System.out.println("CAST " + spell.getActionId());
        }
        else {
            System.out.println("REST");
        }
    }

    public static void debug(ArrayList<Recipe> items){
        for (Recipe rec : items) {
            System.err.println(rec.toString());
        }
    }

    public static void debug(String msg) {
        System.err.println(msg);
    }
}

class Simulation {
    private Recipe targetPotion;
    private ArrayList<Recipe> moves;
    private double moveCount;
    private double rupeeRate;
    Simulation(Recipe targetPotion, ArrayList<Recipe> moves, double moveCount) {
        this.targetPotion = targetPotion;
        this.moves = moves;
        this.moveCount = moveCount;
        rupeeRate = targetPotion.getPrice() / moveCount;
    }

    public Recipe getTargetPotion() { return targetPotion; }

    public ArrayList<Recipe> getMoves() { return moves; }

    public double getMoveCount() { return moveCount; }

    public void setMoves(ArrayList<Recipe> moves) { this.moves = moves; }

    public void setMoveCount(double moveCount) { this.moveCount = moveCount; }

    public double getRupeeRate() { return this.rupeeRate; }

    @Override
    public String toString(){
        String s = "Turn move count " + this.moveCount + "\n";
        for (Recipe move : moves)
            s += moves.toString() + "\n";
        return s;
    }
}

class Tier {
    private int id;
    private int qty;
    private double value;
    Tier(int id, int qty, double value) {
        this.id = id;
        this.qty = qty;
        this.value = value;
    }

    public int getId() { return id; }

    public int getQty() { return qty; }

    public double getValue() { return value; }

    public void setQty(int qty) { this.qty = qty; }

    public void setValue(double value) { this.value = value; }

    @Override
    public String toString() {
        return "\nTier " + id + " qty: " + qty + " value: " + value;
    }
}

class Storage {
    private int inv0;
    private int inv1;
    private int inv2;
    private int inv3;
    private int score;
    Storage(int inv0, int inv1, int inv2, int inv3, int score) {
        this.inv0 = inv0;
        this.inv1 = inv1;
        this.inv2 = inv2;
        this.inv3 = inv3;
        this.score = score;
    }
    public int getInv0(){ return inv0; }
    public int getInv1() { return inv1; }
    public int getInv2() { return inv2; }
    public int getInv3() { return inv3; }
    public int getScore() { return score; }
    public int[] getItemArray() { return new int[] { this.inv0, this.inv1, this.inv2, this.inv3}; }

    @Override
    public String toString() {
        return "My Inventory: " + inv0 + ", " + inv1 + ", " + inv2 + ", " + inv3 + " Score: " + score;
    }
}

class Recipe {
    private int actionId;
    private int delta0;
    private int delta1;
    private int delta2;
    private int delta3;
    private int price;
    private double total;
    private boolean castable;
    private String actionType;
    private int tomeIndex;
    private int taxCount;
    private boolean repeatable;
    Recipe(String actionType, int actionId, int delta0, int delta1, int delta2, int delta3, int price, int tomeIndex, int taxCount, boolean repeatable, double total) {
        this.actionType = actionType;
        this.actionId = actionId;
        this.delta0 = delta0;
        this.delta1 = delta1;
        this.delta2 = delta2;
        this.delta3 = delta3;
        this.price = price;
        this.tomeIndex = tomeIndex;
        this.taxCount = taxCount;
        this.repeatable = repeatable;
        this.total = total;
    }

    public int getActionId() { return this.actionId; }
    public int getDelta0() { return this.delta0; }
    public int getDelta1() { return this.delta1; }
    public int getDelta2() { return this.delta2; }
    public int getDelta3() { return this.delta3; }
    public int getPrice() { return this.price; }
    public double getTotal() { return total; }
    public boolean getCastable() { return this.castable; }
    public int[] getItemArray() { return new int[] { this.delta0, this.delta1, this.delta2, this.delta3}; }
    public int getTomeIndex(){ return this.tomeIndex; }
    public int getTaxCount() { return this.taxCount; }
    public boolean getRepeatable() { return this.repeatable; }

    public void setCastable(boolean castable) { this.castable = castable; }
    public String getActionType() { return this.actionType; }

    public Recipe getObject() {
        Recipe r = new Recipe(actionType, actionId, delta0, delta1, delta2, delta3, price, tomeIndex, taxCount, repeatable, total);
        r.setCastable(this.castable);
        return r;
    }

    @Override
    public String toString() {
        return "\nRecipe id: " + actionId + " Materials: " + delta0 + ", " + delta1 + ", " + delta2 + ", " + delta3 + " castable? " +
                castable + " Price: " + price + " Total: " + total +
                "actionType: " + actionType + " tomeIndex: " + tomeIndex + " taxCount: " + taxCount  + " repeatable? " + repeatable;
    }
}