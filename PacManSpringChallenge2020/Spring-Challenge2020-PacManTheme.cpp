/****************************************************************

// Kyle Orcutt
// CodingGame Spring Challenge 2020 - PacMan Theme
// Contest Rank: 1914/4955 (Silver League)

https://www.codingame.com/multiplayer/bot-programming/spring-challenge-2020

"Control a team of pacs and eat more pellets than your opponent. 
Beware, an enemy pac just might be waiting around the corner to take a bite out of you!"

 ****************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;

/****************************************************************
	Structs and Classes
 ****************************************************************/

struct pac
{
	int pacId; // pac number (unique within a team)
	bool mine; // true if this pac is yours
	int x; // position in the grid
	int y; // position in the grid
	string typeId; // unused in wood leagues
	int speedTurnsLeft; // unused in wood leagues
	int abilityCooldown; // unused in wood leagues
	int targetX;
	int targetY;
};

struct pellet
{
	int x;
	int y;
	int value; // amount of points this pellet is worth
	int id;
};

/****************************************************************
	Main
 ****************************************************************/

void loadPacs(vector<struct pac>& myPacs, vector<struct pac>& opponentPacs, vector<vector<string>>& board);

void printPacTeam(vector<struct pac> pacs);

void loadPellets(vector<struct pellet>& smallPellets, vector<struct pellet>& largePellets, vector<vector<string>>& board);

bool moveToLargePellet(struct pac& myPac, vector<struct pac> myPacs, vector<struct pellet> pellets);

bool moveToClosestPellet(struct pac& myPac, vector<struct pac> myPacs, vector<struct pellet> pellets, vector<vector<string>> board);

void printBoard(vector<vector<string>> board);

void updateBoard(struct pac myPac, struct pac lastPac, vector<vector<string>>& board);

bool isNeighbor(int x1, int y1, int x2, int y2);

bool switchPacType(struct pac myPac, vector<struct pac> opponentPacs);

bool moveToOpponent(struct pac myPac, vector<struct pac> opponentPacs);

vector<vector<int>> getSearchPoints(vector<vector<string>> board);

bool moveToClosestSearch(struct pac myPac, vector<struct pac> pacs, vector<vector<int>> searchTargets, vector<vector<string>> board);

void resetBoard(vector<vector<string>>& board)
{
	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board[i].size(); j++)
		{
			if (board[i][j] != "#")
				board[i][j] = " ";
		}
	}
}


int main()
{
	int width; // size of the grid
	int height; // top left corner is (x=0, y=0)
	cin >> width >> height; cin.ignore();
	vector<vector<string>> board(height, vector<string>(width, "."));
	vector<struct pac> lastPacs;

	for (int i = 0; i < height; i++) {
		string row;
		getline(cin, row); // one line of the grid: space " " is floor, pound "#" is wall
		for (int j = 0; j < row.length(); j++)
		{
			board[i][j] = row[j];
			cerr << board[i][j] << " ";
		}
		cerr << endl;
	}

	vector<vector<int>> searchTargets = getSearchPoints(board);
	cerr << "Searchable target size = " << searchTargets.size() << endl;

	// game loop
	while (1) {
		int myScore;
		int opponentScore;
		cin >> myScore >> opponentScore; cin.ignore();

		resetBoard(board);

		vector<struct pac> myPacs;
		vector<struct pac> opponentPacs;
		loadPacs(myPacs, opponentPacs, board);

		vector<struct pellet> smallPellets;
		vector<struct pellet> largePellets;

		loadPellets(smallPellets, largePellets, board);

		//printPacTeam(myPacs);
		//cerr << "LAST PACS:" << endl;
		//printPacTeam(lastPacs);
		for (int i = 0; i < myPacs.size(); i++)
		{
			//cerr << "PAC " << myPacs[i].pacId << " has started its turn" << endl;
			if (myPacs[i].abilityCooldown > 5 && !lastPacs.empty())
			{
				//cerr << "PAC " << myPacs[i].pacId << " has started board update" << endl;
				for (int j = 0; j < lastPacs.size(); j++) {
					if (myPacs[i].pacId == lastPacs[j].pacId) {
						//updateBoard(myPacs[i], lastPacs[j], board);
						break;
					}
				}
				//cerr << "PAC " << myPacs[i].pacId << " has finished board update" << endl;
			}

			bool commandUsed = false;
			if (myPacs[i].abilityCooldown < 1) {
				commandUsed = switchPacType(myPacs[i], opponentPacs);
				cerr << "PAC " << myPacs[i].pacId << " used switch? " << commandUsed << endl;
			}
			if (!commandUsed)
			{
				//commandUsed = moveToOpponent(myPacs[i], opponentPacs);
			}
			if (!largePellets.empty() && !commandUsed)
			{
				commandUsed = moveToLargePellet(myPacs[i], myPacs, largePellets);
				cerr << "PAC " << myPacs[i].pacId << " found large pellet? " << commandUsed << endl;
			}
			if (!commandUsed) {
				//cerr << "PAC " << myPacs[i].pacId << " can't find large pellet, now searching for small pellet" << endl;
				//commandUsed = moveToClosestPellet(myPacs[i], myPacs, smallPellets, board);
				//cerr << "PAC " << myPacs[i].pacId << " found small pellet? " << commandUsed << endl;
				commandUsed = moveToClosestSearch(myPacs[i], myPacs, searchTargets, board);
			}
			//cerr << "COMMAND USED " << commandUsed << endl;

		}

		cout << endl;

		lastPacs = myPacs;

		//if (searchTargets.size() > 0) {
		//	for (int i = 0; i < searchTargets.size(); i++)
		//	{
		//		cerr << "Search Targets: " << searchTargets[i][0] << " " << searchTargets[i][1] << endl;
		//	}
		//}

		//cerr << "small/large Pellets.size() " << smallPellets.size() << " " << largePellets.size() << endl;
		//printPacTeam(myPacs);
		//printPacTeam(opponentPacs);
		printBoard(board);

	}
}

/****************************************************************
	Debug
 ****************************************************************/

void printBoard(vector<vector<string>> board)
{
	for (int i = 0; i < board.size(); i++)
	{
		for (int j = 0; j < board[i].size(); j++)
		{
			cerr << board[i][j] << " ";
		}
		cerr << endl;
	}
}

void printPacTeam(vector<struct pac> pacs)
{
	int length = pacs.size();
	if (length < 1)
	{
		cerr << "No Pacs to Report" << endl;
		return;
	}
	if (pacs[0].mine)
		cerr << "My Pacs:" << endl;
	else
		cerr << "Opponent Pacs:" << endl;

	for (int i = 0; i < length; i++)
	{
		struct pac blinky = pacs[i];
		cerr << "Pac Id " << blinky.pacId << " x " << blinky.x << " y " << blinky.y << " Type Id " << blinky.typeId << endl;
		cerr << "  Speed Turns " << blinky.speedTurnsLeft << " Cooldown " << blinky.abilityCooldown << endl;
		cerr << "  Targeting X " << blinky.targetX << " Y " << blinky.targetY << endl;
	}
	cerr << endl;
}


/****************************************************************
	Load and store game input
 ****************************************************************/

void loadPacs(vector<struct pac>& myPacs, vector<struct pac>& opponentPacs, vector<vector<string>>& board)
{
	int visiblePacCount; // all your pacs and enemy pacs in sight
	cin >> visiblePacCount; cin.ignore();
	for (int i = 0; i < visiblePacCount; i++) {
		int pacId; // pac number (unique within a team)
		bool mine; // true if this pac is yours
		int x; // position in the grid
		int y; // position in the grid
		string typeId; // unused in wood leagues
		int speedTurnsLeft; // unused in wood leagues
		int abilityCooldown; // unused in wood leagues
		cin >> pacId >> mine >> x >> y >> typeId >> speedTurnsLeft >> abilityCooldown; cin.ignore();
		if (mine)
		{
			struct pac blinky;
			myPacs.push_back(blinky);
			myPacs[myPacs.size() - 1].pacId = pacId;
			myPacs[myPacs.size() - 1].mine = mine;
			myPacs[myPacs.size() - 1].x = x;
			myPacs[myPacs.size() - 1].y = y;
			myPacs[myPacs.size() - 1].typeId = typeId;
			myPacs[myPacs.size() - 1].speedTurnsLeft = speedTurnsLeft;
			myPacs[myPacs.size() - 1].abilityCooldown = abilityCooldown;
		}
		else
		{
			struct pac pinky;
			opponentPacs.push_back(pinky);
			opponentPacs[opponentPacs.size() - 1].pacId = pacId;
			opponentPacs[opponentPacs.size() - 1].mine = mine;
			opponentPacs[opponentPacs.size() - 1].x = x;
			opponentPacs[opponentPacs.size() - 1].y = y;
			opponentPacs[opponentPacs.size() - 1].typeId = typeId;
			opponentPacs[opponentPacs.size() - 1].speedTurnsLeft = speedTurnsLeft;
			opponentPacs[opponentPacs.size() - 1].abilityCooldown = abilityCooldown;
		}
		board[y][x] = " ";
	}
}

void loadPellets(vector<struct pellet>& smallPellets, vector<struct pellet>& largePellets, vector<vector<string>>& board)
{
	int visiblePelletCount; // all pellets in sight
	cin >> visiblePelletCount; cin.ignore();

	int smallCount = 0;
	int largeCount = 0;
	for (int i = 0; i < visiblePelletCount; i++) {
		int x;
		int y;
		int value; // amount of points this pellet is worth
		cin >> x >> y >> value; cin.ignore();
		if (value == 1) {
			struct pellet pel;
			smallPellets.push_back(pel);
			smallPellets[smallCount].x = x;
			smallPellets[smallCount].y = y;
			smallPellets[smallCount].value = value;
			smallPellets[smallCount].id = smallCount;
			smallCount++;
		}
		else {
			struct pellet pel;
			largePellets.push_back(pel);
			largePellets[largeCount].x = x;
			largePellets[largeCount].y = y;
			largePellets[largeCount].value = value;
			largePellets[largeCount].id = largeCount;

			largeCount++;
		}
		board[y][x] = to_string(value);
	}
}

/****************************************************************
	Conditions
 ****************************************************************/

bool isValid(int x, int y, vector<vector<string>> board)
{
	int width = board[0].size();
	int height = board.size();
	if (x < 0 || x > width || y < 0 || y > height)
		return false;
	if (board[y][x] == "#")
		return false;
	return true;
}

bool isNeighbor(int x1, int y1, int x2, int y2)
{
	if (x1 == x2 && abs(y1 - y2) == 1)
		return true;
	else if (y1 == y2 && abs(x1 - x2) == 1)
		return true;
	return false;
}

bool isWinningType(struct pac p1, struct pac p2)
{
	string myType = p1.typeId;
	string oppType = p2.typeId;
	if (myType == "ROCK" && oppType == "SCISSORS")
		return true;
	else if (myType == "PAPER" && oppType == "ROCK")
		return true;
	else if (myType == "SCISSORS" && oppType == "PAPER")
		return true;
	return false;
}


/****************************************************************
   Commmands and supporting functions
 ****************************************************************/

vector<vector<int>> getSearchPoints(vector<vector<string>> board)
{
	vector<vector<int>> points;
	int minRow = 1;
	int maxRow = board.size() - 2;
	int minCol = 1;
	int maxCol = board[0].size() - 2;

	for (int j = minCol; j <= maxCol; j++)
	{
		if (board[minRow][j] != "#") {
			//vector<int> p = { j, minRow };
			points.push_back({ j, minRow });
		}
	}

	for (int i = minRow; i <= maxRow; i++)
	{
		if (board[i][minCol] != "#") {
			points.push_back({ minCol, i });
		}
	}

	for (int j = minCol; j <= maxCol; j++)
	{
		if (board[maxRow][j] != "#") {
			points.push_back({ j, maxRow });
		}
	}

	for (int i = minRow; i <= maxRow; i++)
	{
		if (board[i][maxCol] != "#") {
			points.push_back({ maxCol, i });
		}
	}
	return points;

}

bool moveToClosestSearch(struct pac myPac, vector<struct pac> pacs, vector<vector<int>> searchTargets, vector<vector<string>> board)
{
	int minDist = 1000;
	int index = -1;
	for (int i = 0; i < searchTargets.size(); i++)
	{
		//for (int j = 0; j < pacs.size(); j++) {
			if (searchTargets[i][0] == myPac.x && searchTargets[i][1] == myPac.y)
				board[myPac.y][myPac.x] = "#";

			if (board[myPac.y][myPac.x] != "#") {
				int xDist = pow(myPac.x - searchTargets[i][0], 2);
				int yDist = pow(myPac.y - searchTargets[i][1], 2);
				int dist = sqrt(xDist + yDist);
				if (dist < minDist)
				{
					minDist = dist;
					index = i;
				}
			}
		//}
	}

	if (index > -1)
	{
		cout << "MOVE " << myPac.pacId << " " << searchTargets[index][0] << " " << searchTargets[index][1] << " " << searchTargets[index][0] << " " << searchTargets[index][1] << " S|";
		return true;
	}
	return false;
}

string getWinningType(struct pac pac) {
	//if (pac.abilityCooldown < me)

	string currentType = pac.typeId;
	string targetType = "";
	if (currentType == "ROCK")
		targetType = "PAPER";
	else if (currentType == "PAPER")
		targetType = "SCISSORS";
	else if (currentType == "SCISSORS")
		targetType = "ROCK";
	return targetType;
}

bool moveToOpponent(struct pac myPac, vector<struct pac> opponentPacs)
{
	cerr << "Pac " << myPac.pacId << " is trying to attack opponent" << endl;
	for (int i = 0; i < opponentPacs.size(); i++) {
		int xDist = pow(myPac.x - opponentPacs[i].x, 2);
		int yDist = pow(myPac.y - opponentPacs[i].y, 2);
		int dist = sqrt(xDist + yDist);
		if (dist < 5 && isWinningType(myPac, opponentPacs[i]))
		{
			cout << "MOVE " << myPac.pacId << " " << opponentPacs[i].x << " " << opponentPacs[i].y << " ATTACK|";
			return true;
		}
	}
	return false;
}

bool switchPacType(struct pac myPac, vector<struct pac> opponentPacs)
{
	if (opponentPacs.size() > 0)
	{
		int neighborIndex = -1;
		for (int j = 0; j < opponentPacs.size(); j++)
		{
			bool hasOpponentNeighbor = isNeighbor(myPac.x, myPac.y, opponentPacs[j].x, opponentPacs[j].y);
			if (hasOpponentNeighbor)
			{
				neighborIndex = j;
			}
		}
		if (neighborIndex > -1) {
			cerr << "Neighbor Pac Id " << opponentPacs[neighborIndex].pacId << " x " << opponentPacs[neighborIndex].x << " y " << opponentPacs[neighborIndex].y << " Type Id " << opponentPacs[neighborIndex].typeId << endl;
			cerr << "  Speed Turns " << opponentPacs[neighborIndex].speedTurnsLeft << " Cooldown " << opponentPacs[neighborIndex].abilityCooldown << endl;
		}
		if (neighborIndex > -1) {
			string type = getWinningType(opponentPacs[neighborIndex]);
			ostringstream id;
			id << myPac.pacId;
			//type << opponentPacs[neighborIndex].typeId;
			if (type == myPac.typeId)
			{
				cout << "MOVE " << myPac.pacId << " " << opponentPacs[neighborIndex].x << " " << opponentPacs[neighborIndex].y << " " << opponentPacs[neighborIndex].x << " " << opponentPacs[neighborIndex].y << "|" << endl;
			}
			else
				cout << "SWITCH " << id.str() << " " << type << " SWITCH" << "|";
			return true;
		}
	}
	return false;
	//cout << "SPEED " << myPacs[i].pacId << "|";
}

//struct pellet getClosestPellet(int x, int y, vector<vector<string>> board)
//{
//	//cerr << "Searching from: " << x << " " << y << endl;
//	struct pellet target;
//	if (isValid(x, y, board)) {
//		if (board[y][x] == " ")
//		{
//			board[y][x] = ".";
//			//target = getClosestPellet(x - 1, y - 1, board);
//			target = getClosestPellet(x - 1, y, board);
//			//target = getClosestPellet(x - 1, y + 1, board);
//			target = getClosestPellet(x, y - 1, board);
//			target = getClosestPellet(x, y + 1, board);
//			//target = getClosestPellet(x + 1, y - 1, board);
//			target = getClosestPellet(x + 1, y, board);
//			//target = getClosestPellet(x + 1, y + 1, board);
//		}
//		else
//		{
//			target.x = x;
//			target.y = y;
//		}
//	}
//	return target;
//}


void addTarget(int x, int y, vector<pellet>& targets)
{
	struct pellet pel;
	pel.x = x;
	pel.y = y;
	targets.push_back(pel);
}

vector<struct pellet> getClosestPellet(int minX, int maxX, int minY, int maxY, vector<vector<string>> board)
{
	if (minX > 0)
		minX--;
	if (maxX < board[0].size() - 1)
		maxX++;
	if (minY > 0)
		minY--;
	if (maxY < board.size() - 1)
		maxY++;

	//cerr << "Searching from: " << minX << " " << maxX << " " << minY << " " << maxY << endl;

	vector<struct pellet> targets;

	for (int i = minX; i <= maxX; i++)
	{
		if (board[minY][i] == "1" || board[minY][i] == "10")
			addTarget(i, minY, targets);

		if (board[maxY][i] == "1" || board[maxY][i] == "10")
			addTarget(i, maxY, targets);
	}
	for (int i = minY; i <= maxY; i++)
	{
		if (board[i][minX] == "1" || board[i][minX] == "10")
			addTarget(minX, i, targets);

		if (board[i][maxX] == "1" || board[i][maxX] == "10")
			addTarget(maxX, i, targets);

	}
	bool isFullyChecked = minX == 0 && maxX == board[0].size() - 1 && minY == 0 && maxY == board.size() - 1;
	if (isFullyChecked) {
		return targets;
	}

	if (targets.size() == 0)
	{
		targets = getClosestPellet(minX, maxX, minY, maxY, board);
	}
	return targets;
}

int getClosestPelletIndex(struct pac myPac, vector<struct pac> myPacs, vector<struct pellet>& pellets)
{
	for (int i = 0; i < pellets.size() - 1; i++)
	{
		for (int j = 0; j < pellets.size() - i - 1; j++) {
			int xDist = pow(myPac.x - pellets[j].x, 2);
			int yDist = pow(myPac.y - pellets[j].y, 2);
			int dist = sqrt(xDist + yDist);

			int xDist2 = pow(myPac.x - pellets[j + 1].x, 2);
			int yDist2 = pow(myPac.y - pellets[j + 1].y, 2);
			int dist2 = sqrt(xDist2 + yDist2);

			if (dist > dist2)
			{
				struct pellet temp = pellets[j];
				pellets[j] = pellets[j + 1];
				pellets[j + 1] = temp;
			}
		}
	}
	//for (int i = 0; i < pellets.size(); i++)
	//	cerr << "Large Pellets for Pac: " << myPac.pacId << " " << pellets[i].x << " " << pellets[i].y << endl;

	int index = -1;
	for (int i = 0; i < pellets.size(); i++) {
		bool targeted = false;
		for (int j = 0; j < myPacs.size(); j++)
		{
			int xDist = pow(myPac.x - pellets[i].x, 2);
			int yDist = pow(myPac.y - pellets[i].y, 2);
			int dist = sqrt(xDist + yDist);

			int xDist2 = pow(myPacs[j].x - pellets[i].x, 2);
			int yDist2 = pow(myPacs[j].y - pellets[i].y, 2);
			int dist2 = sqrt(xDist2 + yDist2);
			bool isCloser = dist2 < dist;
			bool isTarget = myPacs[j].targetX == pellets[i].x && myPacs[j].targetY == pellets[i].y;
			if (myPacs[j].pacId != myPac.pacId && (isTarget || isCloser))
				targeted = true;
		}
		if (!targeted) {
			index = i;
			break;
		}
	}

	return index;
}

bool moveToLargePellet(struct pac& myPac, vector<struct pac> myPacs, vector<struct pellet> pellets)
{
	//cerr << "PAC " << myPac.pacId << " has started checking for a large pellet" << endl;
	int index = getClosestPelletIndex(myPac, myPacs, pellets);
	if (index > -1) {
		myPac.targetX = pellets[index].x;
		myPac.targetY = pellets[index].y;
		ostringstream x, y, id;
		x << pellets[index].x;
		y << pellets[index].y;
		id << myPac.pacId;
		cout << "MOVE " << id.str() << " " << x.str() << " " << y.str() << " " << x.str() << " " << y.str() << "|"; // MOVE <pacId> <x> <y>
		return true;
	}
	return false;
}

bool moveToClosestPellet(struct pac& myPac, vector<struct pac> myPacs, vector<struct pellet> pellets, vector<vector<string>> board)
{
	//cerr << "PAC " << myPac.pacId << " is starting small pellet search " << endl;
	vector<struct pellet> targets = getClosestPellet(myPac.x, myPac.x, myPac.y, myPac.y, board);

	int size = targets.size();
	size = min(10, size);
	//for (int i = 0; i < size; i++)
		//cerr << "Small Pellets for Pac: " << myPac.pacId << " " << targets[i].x << " " << targets[i].y << endl;

	int index = 0;
	for (int i = 0; i < targets.size(); i++)
	{
		bool targeted = false;
		for (int j = 0; j < myPacs.size(); j++)
		{
			int xDist = pow(myPac.x - targets[i].x, 2);
			int yDist = pow(myPac.y - targets[i].y, 2);
			int dist = sqrt(xDist + yDist);

			int xDist2 = pow(myPacs[j].x - targets[i].x, 2);
			int yDist2 = pow(myPacs[j].y - targets[i].y, 2);
			int dist2 = sqrt(xDist2 + yDist2);
			bool isCloser = dist2 <= dist;
			bool isTarget = myPacs[j].targetX == targets[i].x && myPacs[j].targetY == targets[i].y;
			if (myPacs[j].pacId != myPac.pacId && (isTarget || isCloser))
				targeted = true;
		}
		if (!targeted) {
			index = i;
			break;
		}
	}

	cerr << "TARGET " << targets[index].x << " " << targets[index].y << endl;
	myPac.targetX = targets[index].x;
	myPac.targetY = targets[index].y;
	ostringstream x, y, id;
	x << targets[index].x;
	y << targets[index].y;
	id << myPac.pacId;
	cout << "MOVE " << id.str() << " " << x.str() << " " << y.str() << " " << x.str() << " " << y.str() << "|"; // MOVE <pacId> <x> <y>
	return true;
}

void updateBoard(struct pac myPac, struct pac lastPac, vector<vector<string>>& board)
{
	int xDifference = myPac.x - lastPac.x;
	int yDifference = myPac.y - lastPac.y;
	if (abs(xDifference + yDifference) < 2)
		return;
	int x1 = myPac.x;
	int y1 = myPac.y;
	int x2 = lastPac.x;
	int y2 = lastPac.y;
	if (abs(xDifference) == 2)
	{
		if (isValid(x1 + 1, y1, board) && xDifference > 0) {
			board[y1][x1 + 1] = " ";
			board[y1][x1 + 2] = " ";
		}
		else if (isValid(x1 - 1, y1, board)) {
			board[y1][x1 - 1] = " ";
			board[y1][x1 - 2] = " ";
		}
	}
	else if (abs(yDifference) == 2)
	{
		if (isValid(x1, y1 + 1, board) && yDifference > 0) {
			board[y1 + 1][x1] = " ";
			board[y1 + 2][x1] = " ";
		}
		else if (isValid(x1, y1 - 1, board)) {
			board[y1 - 1][x1] = " ";
			board[y1 - 2][x1] = " ";
		}
	}
	else
	{
		if (x1 < x2 && y1 < y2)
		{
			if (isValid(x1 + 1, y1, board))
				board[y1][x1 + 1] = " ";
			else if (isValid(x1, y1 + 1, board))
				board[y1 + 1][x1] = " ";
		}
		else if (x1 > x2&& y1 < y2)
		{
			if (isValid(x1 - 1, y1, board))
				board[y1][x1 - 1] = " ";
			else if (isValid(x1, y1 + 1, board))
				board[y1 + 1][x1] = " ";
		}
		else if (x1 < x2 && y1 > y2)
		{
			if (isValid(x1 + 1, y1, board))
				board[y1][x1 + 1] = " ";
			else if (isValid(x1, y1 - 1, board))
				board[y1 - 1][x1] = " ";
		}
		else if (x1 > x2&& y1 > y2)
		{
			if (isValid(x1 - 1, y1, board))
				board[y1][x1 - 1] = " ";
			else if (isValid(x1, y1 - 1, board))
				board[y1 - 1][x1] = " ";
		}
	}
}