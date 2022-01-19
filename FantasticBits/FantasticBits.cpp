// Kyle Orcutt
// CodingGame - Fantastic bits bot game
// Global Rank: 251/9000 (Legend league)

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath> 

using namespace std;

class Entity {
private:
    int entityId;
    string entityType;
    int x;
    int y;
    int vx;
    int vy;
    int state;
public:
    Entity(int entityId, string entityType, int x, int y, int vx, int vy, int state) {
        this->entityId = entityId;
        this->entityType = entityType;
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        this->state = state;
    }
    int getEntityId() {
        return entityId;
    }
    string getEntityType() {
        return entityType;
    }
    int getX() {
        return x;
    }
    int getY() {
        return y;
    }
    int getVX() {
        return vx;
    }
    int getVY() {
        return vy;
    }
    int getNewX() {
        return x + vx;
    }
    int getNewY() {
        return y + vy;
    }
    int getState() {
        return state;
    }
    void setState(int state) {
        this->state = state;
    }
};

class Goal {
public:
    int x;
    int y;
public:
    Goal(int x, int y) {
        this->x = x;
        this->y = y;
    }
    int getX() {
        return x;
    }
    int getY() {
        return y;
    }
};

Entity getClosestEntity(Entity entity, vector<Entity> entities) {
    Entity myClosestEntity = entities[0];
    int minDistance = 17000;

    for (int i = 0; i < entities.size(); i++) {
        Entity tempEntity = entities[i];
        int distance = (int)sqrt((entity.getX() - tempEntity.getX()) * (entity.getX() - tempEntity.getX()) + (entity.getY() - tempEntity.getY()) * (entity.getY() - tempEntity.getY()));
        if (distance < minDistance && tempEntity.getState() == 0) {
            myClosestEntity = tempEntity;
            minDistance = distance;
        }
    }

    return myClosestEntity;
}

//int getDistance(Entity entity1, Entity entity2) {
//    return (int)sqrt((entity1.getX() - entity2.getX()) * (entity1.getX() - entity2.getX()) + (entity1.getY() - entity2.getY()) * (entity1.getY() - entity2.getY()));
//}

int getDistance(Entity entity1, Entity entity2) {
     return (int)sqrt((entity1.getNewX()- entity2.getNewX()) * (entity1.getNewX() - entity2.getNewX()) + (entity1.getNewY() - entity2.getNewY()) * (entity1.getNewY() - entity2.getNewY()));
}

int getGoodPetrificus(int myTeamId, vector<Entity> snaffles) {
    Entity scoredSnaffle = snaffles[0];
    bool isDanger = false;
    int snaffleIndex = -1;
    for (int k = 0; k < snaffles.size(); k++) {
        //if ((myTeamId == 0? snaffles[k].getVX() < -1000 : snaffles[k].getVX() > 1000) && snaffles[k].getNewY() < 5900 && snaffles[k].getNewY() > 1500 && snaffles[k].getVX() < 0){
        if ((myTeamId == 0 ? (snaffles[k].getNewX() < 4000 && snaffles[k].getVX() < -300) : snaffles[k].getNewX() > 12000 && snaffles[k].getVX() > 300) && snaffles[k].getNewY() > 1500 && snaffles[k].getNewY() < 5900) {
            scoredSnaffle = snaffles[k];
            isDanger = true;
            snaffleIndex = k;
        }
    }
    if (isDanger)
        return snaffleIndex;
    else
        return -1;
}

bool isFlipendoCollision(int myTeamId, Goal myGoal, Entity myWizard, vector<Entity> entities, int entityRadius, int goalY) {
    int entityY = 8000;
    for (int i = 0; i < entities.size(); i++) {
        int x = myWizard.getNewX() - entities[i].getNewX();
        int y = myWizard.getNewY() - entities[i].getNewY();

        if (myTeamId == 0 ? x < 0 : x > 0) {
            int slope = y / x;
            entityY = abs((myGoal.getX() - myWizard.getNewX()) * slope + myWizard.getNewY() - myGoal.getY());

            if (goalY < entityY + entityRadius && goalY > entityY - entityRadius)
                return true;
        }
    }
    return false;
}

bool isGoodFlipendo(int myTeamId, Goal myGoal, Entity myWizard, vector<Entity> enemyWizards, vector<Entity> bludgers, Entity snaffle) {
    int x = snaffle.getNewX() - myWizard.getNewX();
    int y = snaffle.getNewY() - myWizard.getNewY();
    bool isGoodTarget = false;
    int snaffleDistance = getDistance(myWizard, snaffle);

    if (myTeamId == 1 ? x < 0 : x > 0) {
        // if ((myTeamId == 1? x < 0 && snaffle.getX() < 8000 : x > 0 && snaffle.getX() > 8000) && snaffleDistance < 4000){
        int slope = y / x;
        int goalY = abs((myGoal.getX() - snaffle.getNewX()) * slope + snaffle.getNewY() - myGoal.getY());

        bool isEnemyCollision = isFlipendoCollision(myTeamId, myGoal, myWizard, enemyWizards, 400, goalY);
        //bool isBludgerCollision = isFlipendoCollision(myTeamId, myGoal, myWizard, bludgers, 200, goalY);

        if (goalY < 4000 / 2 + 300 && !isEnemyCollision /*&& !isBludgerCollision /*&& (myTeamId == 0 ? snaffle.getX() > 8000 : snaffle.getX() < 8000)*/) {
            isGoodTarget = true;
        }
    }
    return isGoodTarget;
}

int getGoodFlipendoIndex(int myTeamId, vector<Entity> snaffles, Entity myWizard, Goal myGoal) {
    Entity scoredSnaffle = snaffles[0];
    for (int k = 0; k < snaffles.size(); k++) {
        int snaffleDistance = getDistance(myWizard, snaffles[k]);
        int x = snaffles[k].getNewX() - myWizard.getNewX();
        int y = snaffles[k].getNewY() - myWizard.getNewY();

        if ((myTeamId == 1 ? x < 0 : x > 0) && snaffleDistance < 4000) {
            int slope = y / x;
            int goalY = (myGoal.getX() - snaffles[k].getX() + snaffles[k].getVX()) * slope + snaffles[k].getY() + snaffles[k].getVY();
            if (abs(goalY - myGoal.getY()) < 4000 / 2 + 300) {
                return k;
            }
        }
    }
    return -1;
}

vector<Entity> sortEntities(Entity entity, vector<Entity> entities) {
    for (int i = 0; i < entities.size() - 1; i++) {
        for (int j = 0; j < entities.size() - i - 1; j++) {
            Entity tempEntity1 = entities[j];
            Entity tempEntity2 = entities[j + 1];
            int distance1 = (int)sqrt((entity.getX() - tempEntity1.getX()) * (entity.getX() - tempEntity1.getX()) + (entity.getY() - tempEntity1.getY()) * (entity.getY() - tempEntity1.getY()));
            int distance2 = (int)sqrt((entity.getX() - tempEntity2.getX()) * (entity.getX() - tempEntity2.getX()) + (entity.getY() - tempEntity2.getY()) * (entity.getY() - tempEntity2.getY()));
            if (distance1 > distance2) {
                entities[j] = tempEntity2;
                entities[j + 1] = tempEntity1;
            }
        }

    }
    return entities;
}

void printEntities(vector<Entity> entities) {
    for (int i = 0; i < entities.size(); i++) {
        cerr << entities[i].getEntityId() << " " << entities[i].getEntityType() << " " << entities[i].getX() << " " << entities[i].getY() << " " << entities[i].getVX() << " " << entities[i].getVY() << " " << entities[i].getState() << endl;
    }
}

int main()
{
    int myTeamId; // if 0 you need to score on the right of the map, if 1 you need to score on the left
    int opTeamId = 1;
    cin >> myTeamId; cin.ignore();
    if (myTeamId == 1)
        opTeamId = 0;

    vector<Goal> goals;
    goals.push_back(Goal( 0, 3750 ));
    goals.push_back(Goal(16000, 3750));
    Goal myGoal = goals[myTeamId];
    Goal oppGoal = goals[opTeamId];

    int wizard1Last = -1;
    int wizard2Last = -1;
    int pointsNeeded = -1;
    bool firstTurn = true;

    // game loop
    while (1) {
        int myScore;
        int myMagic;
        cin >> myScore >> myMagic; cin.ignore();
        int opponentScore;
        int opponentMagic;
        cin >> opponentScore >> opponentMagic; cin.ignore();
        int entities; // number of entities still in game
        cin >> entities; cin.ignore();

        if (firstTurn == true) {
            pointsNeeded = (entities - 6) / 2 + 1;
            firstTurn = false;
        }

        vector<Entity> myWizards;
        vector<Entity> enemyWizards;
        vector<Entity> bludgers;
        vector<Entity> snaffles;

        for (int i = 0; i < entities; i++) {
            int entityId; // entity identifier
            string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
            int x; // position
            int y; // position
            int vx; // velocity
            int vy; // velocity
            int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
            cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();

            if (entityType == "WIZARD") {
                myWizards.push_back(Entity(entityId, entityType, x, y, vx, vy, state));
            }

            if (entityType == "OPPONENT_WIZARD") {
                enemyWizards.push_back(Entity(entityId, entityType, x, y, vx, vy, state));
            }

            if (entityType == "BLUDGER") {
                bludgers.push_back(Entity(entityId, entityType, x, y, vx, vy, state));
            }

            if (entityType == "SNAFFLE") {
                snaffles.push_back(Entity(entityId, entityType, x, y, vx, vy, state));
            }
        }
        for (int i = 0; i < 2; i++) {
            int j = 0;
            if (i == 0)
                j = 1;

            Entity closestSnaffle1 = getClosestEntity(myWizards[i], snaffles);
            Entity closestSnaffle2 = getClosestEntity(myWizards[j], snaffles);

            int snaffle1_Wiz1_Distance = getDistance(myWizards[i], closestSnaffle1);
            int snaffle2_Wiz1_Distance = getDistance(myWizards[i], closestSnaffle2);
            int snaffle1_Wiz2_Distance = getDistance(myWizards[j], closestSnaffle1);
            int snaffle2_Wiz2_Distance = getDistance(myWizards[j], closestSnaffle2);

            vector<Entity> sortedSnaffByWiz = sortEntities(myWizards[i], snaffles);

            Entity myGoalEntity = { -1,"GOAL",myGoal.getX(),myGoal.getY(),myGoal.getX(),myGoal.getY(),0 };
            vector<Entity> sortedSnaffByGoal = sortEntities(myGoalEntity, snaffles);

            int winningSnaffleIndex = pointsNeeded - opponentScore - 1;
            int winSnaffle_Wiz1_Distance = getDistance(myWizards[i], sortedSnaffByGoal[winningSnaffleIndex]);
            int winSnaffle_Wiz2_Distance = getDistance(myWizards[j], sortedSnaffByGoal[winningSnaffleIndex]);
            bool danger = false;

            int winningSnaffleX = sortedSnaffByGoal[winningSnaffleIndex].getNewX();
            // if (winningSnaffleIndex < 3 && (myTeamId == 0 ? winningSnaffleX < 8000 : winningSnaffleX > 8000)) {
            if ((myTeamId == 0 ? winningSnaffleX < 9000 : winningSnaffleX > 7000)) {
                danger = true;
            }

            int petrificusIndex = getGoodPetrificus(myTeamId, sortedSnaffByWiz);
            int patrificusSnaffle_Wiz1_Distance;
            int patrificusSnaffle_Wiz2_Distance;
            if (petrificusIndex >= 0) {
                patrificusSnaffle_Wiz1_Distance = getDistance(myWizards[i], sortedSnaffByWiz[petrificusIndex]);
                patrificusSnaffle_Wiz2_Distance = getDistance(myWizards[j], sortedSnaffByWiz[petrificusIndex]);
            }

            int closestSnaffleX = closestSnaffle1.getNewX();
            int myWizardX = myWizards[i].getNewX();
            int isGoodFlipendoTarget = isGoodFlipendo(myTeamId, myGoal, myWizards[i], enemyWizards, bludgers, closestSnaffle1);

            int goodFlipendoIndex = getGoodFlipendoIndex(myTeamId, sortedSnaffByWiz, myWizards[i], myGoal);
            //cerr << "good Flipendo snaffle: " << goodFlipendoIndex+1 << endl;


            // Command conditionals and output
            if (myWizards[i].getState() == 1) {
                cout << "THROW " + to_string(oppGoal.getX()) + " " + to_string(oppGoal.getY()) + " 500" << endl;
            }
            else if (myMagic >= 15 && danger == true && (myTeamId == 0 ? winningSnaffleX < myWizardX : winningSnaffleX > myWizardX) && snaffle1_Wiz1_Distance > 2000 && winSnaffle_Wiz1_Distance < 8000 && winSnaffle_Wiz2_Distance > winSnaffle_Wiz1_Distance) {
                myMagic -= 15;
                cout << "ACCIO " + to_string(sortedSnaffByGoal[winningSnaffleIndex].getEntityId()) + " WIN!" << endl;
                //if (i == 0 ? wizard1Last = sortedSnaffByWiz[0].getEntityId() : wizard2Last = sortedSnaffByWiz[0].getEntityId());
            }

            else if (myMagic >= 20 && isGoodFlipendoTarget == true) {
                myMagic -= 20;
                cout << "FLIPENDO " << closestSnaffle1.getEntityId() << endl;
                //if (i == 0 ? wizard1Last = closestSnaffle1.getEntityId() : wizard2Last = closestSnaffle1.getEntityId());
            }

            // else if (myMagic >= 20 && goodFlipendoIndex >= 0){
            //     cout << "FLIPENDO " << sortedSnaffByWiz[goodFlipendoIndex].getEntityId() << endl;
            //  }
            //else if ((danger == true ? myMagic >= 10 : myMagic >= 15) && petrificusIndex >= 0 && patrificusSnaffle_Wiz1_Distance < patrificusSnaffle_Wiz2_Distance) {
            //    myMagic -= 10;
            //    cout << "PETRIFICUS " << sortedSnaffByWiz[petrificusIndex].getEntityId() << endl;

            //    //if (i == 0 ? wizard1Last = sortedSnaffByWiz[petrificusIndex].getEntityId() : wizard2Last = sortedSnaffByWiz[petrificusIndex].getEntityId());
            //}
            else if (myMagic >= 15 && (myTeamId == 0 ? closestSnaffleX < myWizardX : closestSnaffleX > myWizardX) && snaffle1_Wiz1_Distance > 2000 && snaffle1_Wiz1_Distance < 8000 && snaffle1_Wiz2_Distance > snaffle1_Wiz1_Distance) {
                myMagic -= 15;
                cout << "ACCIO " + to_string(sortedSnaffByWiz[0].getEntityId()) << endl;
            }

            else {
                if (danger == true && winSnaffle_Wiz2_Distance > winSnaffle_Wiz1_Distance) {
                    string s = to_string(sortedSnaffByGoal[winningSnaffleIndex].getNewX()) + " " + to_string(sortedSnaffByGoal[winningSnaffleIndex].getNewY());
                    cout << "MOVE " + s + " 150" << endl;
                }
                else if (snaffle1_Wiz1_Distance < snaffle1_Wiz2_Distance || snaffles.size() < 2) {
                    string s = to_string(sortedSnaffByWiz[0].getNewX()) + " " + to_string(sortedSnaffByWiz[0].getNewY());
                    cout << "MOVE " + s + " 150" << endl;

                    //if (i == 0 ? wizard1Last = sortedSnaffByWiz[0].getEntityId() : wizard2Last = sortedSnaffByWiz[0].getEntityId());
                }
                else {
                    string s = to_string(sortedSnaffByWiz[1].getNewX()) + " " + to_string(sortedSnaffByWiz[1].getNewY());
                    cout << "MOVE " + s + " 150" << endl;

                    //if (i == 0 ? wizard1Last = sortedSnaffByWiz[1].getEntityId() : wizard2Last = sortedSnaffByWiz[1].getEntityId());
                }
            }
        }
    }
}