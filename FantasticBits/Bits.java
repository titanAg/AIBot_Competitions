// Kyle Orcutt
// CodingGame - Fantastic bits bot game
// Global Rank: 300/9000 (Gold league)

import java.util.*;
import java.io.*;
import java.math.*;


class Entity extends Player{
    private int entityId;
    private String entityType; 
    private int x;
    private int y;
    private int vx;
    private int vy;
    private int state;
    
    Entity(int entityId, String entityType, int x, int y, int vx, int vy, int state){
        this.entityId = entityId;
        this.entityType = entityType;
        this.x = x;
        this.y = y;
        this.vx = vx;
        this.vy = vy;
        this.state = state;
    }
    
    int getX(){
        return x;
    }
    int getY(){
        return y;   
    }
    int getState(){
        return state;
    }
    int getEntityId() {
    	return entityId;
    }
    int getVx(){
        return vx;
    }
    int getVy(){
        return vy;
    }
    
    int predictX(int x){
        return x + vx;
    }
    int predictY(int y){
        return y + vy;
    }
    void distance(){
        
    }
}



class Goal extends Player{
    int x;
    int y;
    
    Goal(int x, int y){
    	this.x = x;
    	this.y = y;
    }
    
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }
    
}
 
class Player{

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        int myTeamId = in.nextInt(); // if 0 you need to score on the right of the map, if 1 you need to score on the left
        ArrayList<Goal> goals = new ArrayList();
        goals.add(new Goal(16000, 3750));
        goals.add(new Goal(0, 3750));
        int home = 0;
        if (myTeamId == 0)
            home = 1;
        Goal side = new Goal(goals.get(myTeamId).getX(), goals.get(myTeamId).getY());
        Goal side2 = new Goal(goals.get(home).getX(), goals.get(home).getY());
        boolean usedObliviate = false;
        int coolDown = 5;
        // game loop
        while (true) {
            ArrayList<Entity> snaffles = new ArrayList();
            ArrayList<Entity> turnEntities = new ArrayList();
            ArrayList<Entity> wizards = new ArrayList();
            ArrayList<Entity> enemies = new ArrayList();
            ArrayList<Entity> bludgers = new ArrayList();
            int myScore = in.nextInt();
            int myMagic = in.nextInt();
            int opponentScore = in.nextInt();
            int opponentMagic = in.nextInt();
            int entities = in.nextInt(); // number of entities still in game
            for (int i = 0; i < entities; i++) {
                int entityId = in.nextInt(); // entity identifier
                String entityType = in.next(); // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
                int x = in.nextInt(); // position
                int y = in.nextInt(); // position
                int vx = in.nextInt(); // velocity
                int vy = in.nextInt(); // velocity
                int state = in.nextInt(); // 1 if the wizard is holding a Snaffle, 0 otherwise
                Entity entity = new Entity(entityId, entityType, x, y, vx, vy, state);
                turnEntities.add(entity);
                if (entityType.equals("SNAFFLE")){
                    snaffles.add(entity);
                }else if (entityType.equals("WIZARD")){
                    wizards.add(entity);
                }else if (entityType.equals("OPPONENT_WIZARD")){
                    enemies.add(entity);
                }else if (entityType.equals("BLUDGER")){
                    bludgers.add(entity);   
                }

            }
            for (int i = 0; i < 2; i++) {
                Entity myWizard = wizards.get(i);
                int index = 0;
                if (i == 0)
                    index = 1;
                Entity myOtherWizard = wizards.get(index);
                Entity nearestSnaffle = null;
                Entity nextNearestSnaffle = null;
                int minDistance = 0;
                int nextMinDistance = 0;
                for(int j = 0; j < snaffles.size(); j++) {
                    Entity snaffle = snaffles.get(j);
                    int distance = (int)Math.sqrt(((snaffle.getX() - myWizard.getX()) * (snaffle.getX() - myWizard.getX())) + (snaffle.getY() - myWizard.getY()) * (snaffle.getY() - myWizard.getY()));
                    if (nearestSnaffle == null || distance < minDistance) {
                        nearestSnaffle = snaffle;
                        minDistance = distance;
                    }
                    if (nextNearestSnaffle == null || (distance < nextMinDistance && distance != minDistance)){
                        nextNearestSnaffle = snaffle;
                        nextMinDistance = distance;
                    }
                    
                }
                Entity nearestEnemy = null;
                int minDistanceEnemy = 0;
                for(int j = 0; j < enemies.size(); j++) {
                    Entity enemy = enemies.get(j);
                    int distance = (int)Math.sqrt((enemy.predictX(enemy.getX()) - myWizard.getX()) * (enemy.predictX(enemy.getX()) - myWizard.getX()) + (enemy.predictY(enemy.getY()) - myWizard.getY()) * (enemy.predictY(enemy.getY()) - myWizard.getY()));
                    if (nearestEnemy == null || distance < minDistanceEnemy) {
                        nearestEnemy = enemy;
                        minDistanceEnemy = distance;
                    }
                }
                Entity nearestBludger = null;
                int minDistanceBludger = 0;
                for(int j = 0; j < bludgers.size(); j++) {
                    Entity bludger = bludgers.get(j);
                    int distance = (int)Math.sqrt((bludger.predictX(bludger.getX()) - myWizard.getX()) * (bludger.predictX(bludger.getX()) - myWizard.getX()) + (bludger.predictY(bludger.getY()) - myWizard.getY()) * (bludger.predictY(bludger.getY()) - myWizard.getY()));
                    if (nearestBludger == null || distance < minDistanceBludger) {
                        nearestBludger = bludger;
                        minDistanceBludger = distance;
                    }
                }
                
                int otherWizardSnaffleDist = (int)Math.sqrt((nearestSnaffle.predictX(nearestSnaffle.getX()) - myOtherWizard.getX()) * (nearestSnaffle.predictX(nearestSnaffle.getX()) - myOtherWizard.getX()) + (nearestSnaffle.predictY(nearestSnaffle.getY()) - myOtherWizard.getY()) * (nearestSnaffle.predictY(nearestSnaffle.getY()) - myOtherWizard.getY()));
                int closestBludger = Math.abs((int)Math.sqrt((nearestBludger.predictX(nearestBludger.getX()) - myWizard.getX()) * (nearestBludger.predictX(nearestBludger.getX()) - myWizard.getX()) + (nearestBludger.predictY(nearestBludger.getY()) - myWizard.getY()) * (nearestBludger.predictY(nearestBludger.getY()) - myWizard.getY())));
                int snaffleDistance = (int)Math.sqrt((nearestSnaffle.getX() - myWizard.getX()) * (nearestSnaffle.getX()  - myWizard.getX()) + (nearestSnaffle.getY()  - myWizard.getY()) * (nearestSnaffle.getY()  - myWizard.getY()));
                int closestEnemyDist = Math.abs((int)Math.sqrt((nearestEnemy.predictX(nearestEnemy.getX()) - myWizard.getX()) * (nearestEnemy.predictX(nearestEnemy.getX()) - myWizard.getX()) + (nearestEnemy.predictY(nearestEnemy.getY()) - myWizard.getY()) * (nearestEnemy.predictY(nearestEnemy.getY()) - myWizard.getY())));
                int opponent1SnaffleDist = Math.abs((int)Math.sqrt((enemies.get(0).getX() - (nearestSnaffle.predictX(nearestSnaffle.getX()))) * (enemies.get(0).getX() - (nearestSnaffle.predictX(nearestSnaffle.getX()))) + (enemies.get(0).getY() - (nearestSnaffle.predictY(nearestSnaffle.getY()))) * (enemies.get(0).getY() - (nearestSnaffle.predictY(nearestSnaffle.getY())))));
                int opponent2SnaffleDist = Math.abs((int)Math.sqrt((enemies.get(1).getX() - (nearestSnaffle.predictX(nearestSnaffle.getX()))) * (enemies.get(1).getX() - (nearestSnaffle.predictX(nearestSnaffle.getX()))) + (enemies.get(1).getY() - (nearestSnaffle.predictY(nearestSnaffle.getY()))) * (enemies.get(1).getY() - (nearestSnaffle.predictY(nearestSnaffle.getY())))));
                
                int x = nearestSnaffle.getX() - myWizard.getX();
                int y = nearestSnaffle.getY() - myWizard.getY();
                boolean goodTarget = false;
                //if  (coolDown == 60){
                    //usedObliviate = false;   
                //}
                    
                
                if (x != 0){
                    int slope = y / x;
                    int goalY = (side.getX() - nearestSnaffle.getX()) * slope + nearestSnaffle.getY();
                    if (Math.abs(goalY - side.getY()) < 4000/2+300){
                        goodTarget = true;   
                    }
                }
                
                /*int snaffleIndex = 0;
                for (int i = 0; i < snaffles.size(); i++){
                    if (myTeamId == 0? snaffles.get(i).getX() < 500 && (snaffles.get(i).getY() > 1750 && snaffles.get(i).getY() < 5750): snaffles.get(i).getX() < 500 && (snaffles.get(i).getY() > 1750 && snaffles.get(i).getY() < 5750)){
                        snaffleIndex = i;
                    }
                }*/
                
                    if (wizards.get(i).getState() == 1){
                        System.out.println("THROW " + side.getX() + " " + side.getY() + " 500");
                        
                    }else if (myMagic >= 20 && (myTeamId == 1? x < 0 : x > 0) && goodTarget == true){
                        System.out.println("FLIPENDO " + nearestSnaffle.getEntityId());
                    }else if (myMagic >= 20 && (myTeamId == 0? x < 0 : x > 0) /*&& (opponent1SnaffleDist < snaffleDistance || opponent2SnaffleDist < snaffleDistance)*/ && snaffleDistance > 2000){
                        if (otherWizardSnaffleDist < snaffleDistance)
                            System.out.println("ACCIO " + nextNearestSnaffle.getEntityId());
                        else
                            System.out.println("ACCIO " + nearestSnaffle.getEntityId());  
                    }else {
                        if (otherWizardSnaffleDist < snaffleDistance)
                            System.out.println("MOVE " + nextNearestSnaffle.getX() + " " + nextNearestSnaffle.getY() + " 150");
                        else
                            System.out.println("MOVE " + nearestSnaffle.getX() + " " + nearestSnaffle.getY() + " 150");
                    }
            }
            snaffles.clear();
            wizards.clear();
            bludgers.clear();
            enemies.clear();
            coolDown++;
            
        }
    }
}