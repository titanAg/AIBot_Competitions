/**************************************************************************

 Kyle Orcutt
 CodingGame - Ocean of Code Contest
 Contest Rank: 635/2279 (Silver League)

https://www.codingame.com/multiplayer/bot-programming/ocean-of-code

"This game is based on the board game Captain Sonar. You pilot a submarine 
and you know that an enemy is present near you because you are listening 
to its radio frequency communication. You don't know exactly where 
it is but you can hear all its orders."


***************************************************************************/

// Run history:

// 109 Silver 
// 132 - charging mines after silence
// 332 - charging silence after mines
//  77 - back to charging mines after silence
// 151 - stop charging mines
//  77 - back to charging mines after silence
//  50 - adding more path finding options
// 124 - path finding options clockwise and checking when moves < 75
// 118 - same but checking when moves < 30
// 129 - alternating silence and mine charging 
// 141 - same but only using silence when > 1 move 
// 111 - adding more path finding & removing alternating charges 
//  99 - same - I think?
// 141 - changing silence back to use when moves > 0


/**************************************************************************
***************************************************************************/

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

/**************************************************************************
	Global Variables
***************************************************************************/

const int HEIGHT = 15;
const int WIDTH = 15;

/**************************************************************************
	Classes
***************************************************************************/

class Battleship {
private:
	int row;
	int col;
	string target = "";
public:
	Battleship(int row, int col) {
		this->row = row;
		this->col = col;
	}

	int getRow() {
		return row;
	}

	int getCol() {
		return col;
	}

	void setRow(int row) {
		this->row = row;
	}

	void setCol(int col) {
		this->col = col;
	}
};

class Quad {
private:
	int id;
	int minRow;
	int maxRow;
	int minCol;
	int maxCol;
	int waterCellCount = 0;
	int landCellCount = 0;
public:
	Quad(int id, int minRow, int maxRow, int minCol, int maxCol) {
		this->id = id;
		this->minRow = minRow;
		this->maxRow = maxRow;
		this->minCol = minCol;
		this->maxCol = maxCol;
	}
	int getId() {
		return id;
	}
	int getWaterCellCount() {
		return waterCellCount;
	}
	int getLandCellCount() {
		return landCellCount;
	}
	int getMinRow() {
		return minRow;
	}
	int getMaxRow() {
		return maxRow;
	}
	int getMinCol() {
		return minCol;
	}
	int getMaxCol() {
		return maxCol;
	}
	void setWaterCellCount(int count) {
		waterCellCount = count;
	}
	void setLandCellCount(int count) {
		landCellCount = count;
	}
};

class Board {
private:
	vector<vector<string>> board;
	vector<Quad> quads;
public:
	Board() {
		vector<vector<string>> board(15, vector<string>(15, "."));
		this->board = board;
	}
	string getBoardValue(int row, int col) {
		return board[row][col];
	}
	void setBoardValue(int row, int col, string value) {
		board[row][col] = value;
	}
	void calculateLandWaterCells() {
		for (int i = 0; i < quads.size(); i++) {
			int minRow = quads[i].getMinRow();
			int maxRow = quads[i].getMaxRow();
			int minCol = quads[i].getMinCol();
			int maxCol = quads[i].getMaxCol();
			int waterCount = 0;
			for (int row = minRow; row <= maxRow; row++) {
				for (int col = minCol; col <= maxCol; col++) {
					if (board[row][col] == ".")
						waterCount++;
				}
			}
			quads[i].setWaterCellCount(waterCount);
			quads[i].setLandCellCount(25 - waterCount);
		}
	}
	void initializeQuads() {
		quads.push_back(Quad(1, 0, 4, 0, 4));
		quads.push_back(Quad(2, 0, 4, 5, 9));
		quads.push_back(Quad(3, 0, 4, 10, 14));
		quads.push_back(Quad(4, 5, 9, 0, 4));
		quads.push_back(Quad(5, 5, 9, 5, 9));
		quads.push_back(Quad(6, 5, 9, 10, 14));
		quads.push_back(Quad(7, 10, 14, 0, 4));
		quads.push_back(Quad(8, 10, 14, 5, 9));
		quads.push_back(Quad(9, 10, 14, 10, 14));
		calculateLandWaterCells();
	}
	int getSpawnQuadIndex() {
		int largestWaterBody = 0;
		int index = -1;
		for (int i = 0; i < quads.size(); i++) {
			if (quads[i].getWaterCellCount() > largestWaterBody && (i != 4 && i != 1 && i != 7)) {
				largestWaterBody = quads[i].getWaterCellCount();
				index = i;
			}
		}
		return index;
	}
	int getSpawnQuadCol(int index) {
		if (quads[index].getId() == 1 || quads[index].getId() == 4 || quads[index].getId() == 7) {
			return 0;
		}
		if (quads[index].getId() == 3 || quads[index].getId() == 6 || quads[index].getId() == 9) {
			return 14;
		}
		return 0;
	}
	vector<int> getSpawnLocation() {
		int index = getSpawnQuadIndex();
		int minRow = quads[index].getMinRow();
		int maxRow = quads[index].getMaxRow();
		//int minCol = quads[index].getMinCol();
		//int maxCol = quads[index].getMaxCol();

		int minCol = getSpawnQuadCol(index);
		int maxCol = getSpawnQuadCol(index);

		vector<int> location = { 0, 0 };
		for (int i = minRow; i <= maxRow; i++) {
			for (int j = minCol; j <= maxCol; j++) {
				if (board[i][j] == ".") {
					location[0] = i;
					location[1] = j;
					break;
				}
			}
		}
		return location;
	}
	int getMineCount(int index) {
		int minRow = quads[index].getMinRow();
		int maxRow = quads[index].getMaxRow();
		int minCol = quads[index].getMinCol();
		int maxCol = quads[index].getMaxCol();
		int mineCount = 0;
		for (int i = minRow; i <= maxRow; i++) {
			for (int j = minCol; j <= maxCol; j++) {
				if (this->getBoardValue(i, j) == "m")
					mineCount++;
			}
		}
		return mineCount++;
	}

	//Outputs board cell values
	void printBoard() {
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				cerr << board[i][j];
			}
			cerr << endl;
		}
		cerr << endl;
	}
};

/**************************************************************************
	Fuction Declarations
***************************************************************************/

void readFirstTurnInput(int& width, int& height, int& myId);
string getSpawnShipCommand();
string moveShip(string charge, queue<string> moves, string nextMove);
string fireTorpedo(int myRow, int myCol, int oppLife, int myLife);
void resetVisitedLocations(Board& board);
int getSectorId(int row, int col);
string getTorpedoTarget(int row, int col);
bool getOpponentTorpedo(vector<int>& location, string opponentOrders);
string getOpponentMovement(string opponentOrders);
string getChargeCommand(int torpedCooldown, int sonarCooldown, int silenceCooldown, int mineCooldown);
string getSilenceCommand(int row, int col);
string goSilent(int& row, int& col, queue<string> moves, string nextMove, int sameMoveCount);
string getOppSurface(string opponentOrders);
void updateOppMoves(string oppMove);
void updateOppSuface(string oppSurface);
string getDirection(int row, int col, queue<string>& moves, int option);
string getStartDirection(int row, int col, queue<string>& moves);
bool shouldSurface();
void updateProximityHit();
void updateDirectHit();
bool isGoodRangeSize();
bool isClearPath(int row, int col, int t_row, int t_col);
string placeMine();
vector<int>  getOpponentTopedo(string opponentOrders);
void updateOppTorpedo(int row, int col);
string triggerMine();

/**************************************************************************
	Main
***************************************************************************/

Board mainBoard;
Board pathBoard;
Board mineBoard;
Battleship myShip = Battleship(0, 0);
//oppLocation indices = { min row, max row, min col, max col}
vector<int> oppRange = { 0 , 14 , 0, 14 };
int targetPosition[2] = { 0 , 0 };
bool boardWasReset = false;
bool torpedoFired = false;
bool usedSurface = false;

void printMoves(queue<string> moves) {
	for (int i = 0; i < moves.size(); i++) {
		cerr << moves.front() << " ";
		moves.pop();
	}
	cerr << endl;
}

int main()
{
	int myLastLife = 6;
	int oppLastLife = 6;
	bool myDamage = false;
	bool proximityHit = false;
	bool directHit = false;
	vector<string> oppMoves;

	int width, height, myId;
	readFirstTurnInput(width, height, myId);

	mainBoard.initializeQuads();
	pathBoard = mainBoard;
	mineBoard = mainBoard;

	cout << getSpawnShipCommand() << endl;

	//queue<string> startMoves;
	//string nextMove = getStartDirection(myShip.getRow(), myShip.getCol(), startMoves);
	//queue<string> moves;
	//moves = startMoves; 
	//printMoves(startMoves);
	//cerr << "START MOVES " << startMoves.size() << endl;
	// game loop
	while (1) {
		int col, row, myLife, oppLife, torpedoCooldown, sonarCooldown, silenceCooldown, mineCooldown;
		string sonarResult, opponentOrders;
		cin >> col >> row >> myLife >> oppLife >> torpedoCooldown >> sonarCooldown >> silenceCooldown >> mineCooldown; cin.ignore();
		cin >> sonarResult; cin.ignore();
		getline(cin, opponentOrders);

		string oppSurface = getOppSurface(opponentOrders);
		updateOppSuface(oppSurface);

		string oppMove = getOpponentMovement(opponentOrders);
		oppMoves.push_back(oppMove);
		updateOppMoves(oppMoves.back());

		//vector<int> oppTorpedo = getOpponentTopedo(opponentOrders);
		//if (oppTorpedo[0] != -1)
		//	updateOppTorpedo(oppTorpedo[0], oppTorpedo[1]);

		if (myLastLife > myLife && !myDamage) {
			myDamage = true;
			myLastLife = myLife;
		}
		queue<string> moves;
		string nextMove = getDirection(row, col, moves, 1);

		if (moves.size() < 25) {
			queue<string> moves2;
			string nextMove2 = getDirection(row, col, moves2, 2);
			if (moves2.size() > moves.size()) {
				moves = moves2;
				nextMove = nextMove2;
			}
		}
		if (moves.size() < 25) {
			queue<string> moves2;
			string nextMove2 = getDirection(row, col, moves2, 3);
			if (moves2.size() > moves.size()) {
				moves = moves2;
				nextMove = nextMove2;
			}
		}
		if (moves.size() < 25) {
			queue<string> moves2;
			string nextMove2 = getDirection(row, col, moves2, 4);
			if (moves2.size() > moves.size()) {
				moves = moves2;
				nextMove = nextMove2;
			}
		}

		//printMoves(moves);
		int sameMoveCount = 0;
		//string firstMove = moves.front();
		int size = moves.size();
		size = min(4, size);
		for (int i = 0; i < size; i++) {
			if (moves.front() == nextMove) {
				sameMoveCount++;
				moves.pop();
			}
		}
		//printMoves(moves);


		//if (shouldSurface()) {
		//	cout << "SURFACE" << endl;
		//	resetVisitedLocations(mainBoard);
		//}
		//else {
		string commandString = "";

		if (oppLastLife > oppLife&& oppSurface == "" && torpedoFired) {
			if (oppLastLife - oppLife == 1) {
				updateProximityHit();
				proximityHit = true;
			}
			else {
				updateDirectHit();
				//updateOppMoves(oppMove);
				updateOppMoves(oppMoves.back());
				//updateOppMoves(oppMoves[oppMoves.size()-2]);
				directHit = true;
			}
			oppLastLife = oppLife;
			//torpedoFired = false;
			//updateOppMoves(oppMove);
		}
		torpedoFired = false;
		bool isTorpedoUsed = false;
		if (torpedoCooldown < 1 && isGoodRangeSize()) {
			commandString += fireTorpedo(row, col, oppLife, myLife);
			isTorpedoUsed = true;
		}

		if (/*myDamage && */silenceCooldown < 1 && sameMoveCount > 0) {
			commandString += goSilent(row, col, moves, nextMove, sameMoveCount);
			nextMove = moves.front();
		}
		//printMoves(moves);
		string charge = "";
		charge = getChargeCommand(torpedoCooldown, sonarCooldown, silenceCooldown, mineCooldown);
		//cerr << "CHARGE " << getChargeCommand(torpedoCooldown, sonarCooldown, silenceCooldown, mineCooldown) << endl;
		commandString += moveShip(charge, moves, nextMove);

		if (charge == "TORPEDO")
			torpedoCooldown--;


		//if (oppLastLife > oppLife && oppSurface == "" && torpedoFired) {
		//	if (oppLastLife - oppLife == 1) {
		//		//updateProximityHit();
		//		proximityHit = true;
		//	}
		//	else {
		//		updateDirectHit();
		//		//updateOppMoves(oppMove);
		//		updateOppMoves(oppMoves.back());
		//		//updateOppMoves(oppMoves[oppMoves.size()-2]);
		//		directHit = true;
		//	}
		//	oppLastLife = oppLife;
		//	//torpedoFired = false;
		//	//updateOppMoves(oppMove);
		//}
		//torpedoFired = false;
		if (torpedoCooldown < 1 && isGoodRangeSize() && !isTorpedoUsed) {
			commandString += fireTorpedo(row, col, oppLife, myLife);
		}

		//string trigger = "";
		string trigger = triggerMine();
		if (trigger != "") {
			commandString += trigger;
		}
		else if (mineCooldown < 1) {
			commandString += placeMine();
		}


		if (commandString == "") {
			resetVisitedLocations(mainBoard);
			//nextMove = getDirection(row, col, moves);
			usedSurface = !usedSurface;
		}

		//string charge = getChargeCommand(torpedoCooldown, sonarCooldown, silenceCooldown);
		//moveShip(charge, moves, nextMove);

	//}

		cout << commandString << endl;

		pathBoard = mainBoard;
		myDamage = false;
		proximityHit = false;
		//cerr << "myShip row: " << myShip.getRow() << endl;
		//cerr << "myShip col: " << myShip.getCol() << endl;

		cerr << "ship row: " << myShip.getRow() << " ship col: " << myShip.getCol() << endl;
		cerr << "min row: " << oppRange[0] << endl;
		cerr << "max row: " << oppRange[1] << endl;
		cerr << "min col: " << oppRange[2] << endl;
		cerr << "max col: " << oppRange[3] << endl;
		mineBoard.printBoard();
		//cerr << "mine count in quad 1 is " << mineBoard.getMineCount(0) << endl;
		//cerr << "mine count in quad 2 is " << mineBoard.getMineCount(1) << endl;
		//cerr << "mine count in quad 3 is " << mineBoard.getMineCount(2) << endl;
		//cerr << "mine count in quad 4 is " << mineBoard.getMineCount(3) << endl;
		//cerr << "mine count in quad 5 is " << mineBoard.getMineCount(4) << endl;
		//cerr << "mine count in quad 6 is " << mineBoard.getMineCount(5) << endl;
		//cerr << "mine count in quad 7 is " << mineBoard.getMineCount(6) << endl;
		//cerr << "mine count in quad 8 is " << mineBoard.getMineCount(7) << endl;
		//cerr << "mine count in quad 9 is " << mineBoard.getMineCount(8) << endl;


		//cerr << "Longest path = " << finLongestOverAll(row, col) << endl;
	}
}

/**************************************************************************
	Test Conditions
***************************************************************************/

bool isValid(int row, int col, Board board) {
	if (row < 0 || row > 14 || col < 0 || col > 14)
		return false;
	else if (board.getBoardValue(row, col) != ".")
		return false;
	return true;
}

bool isValidTorpedo(int row, int col) {
	if (row < 0 || row > 14 || col < 0 || col > 14)
		return false;
	else if (mainBoard.getBoardValue(row, col) == "x")
		return false;
	else if (mineBoard.getBoardValue(row, col) == "m")
		return false;
	return true;
}

bool shouldSurface() {
	int row = myShip.getRow();
	int col = myShip.getCol();
	bool canMoveLeft = isValid(row, col - 1, mainBoard) && mainBoard.getBoardValue(row, col - 1) == ".";
	bool canMoveRight = isValid(row, col + 1, mainBoard) && mainBoard.getBoardValue(row, col + 1) == ".";
	bool canMoveUp = isValid(row - 1, col, mainBoard) && mainBoard.getBoardValue(row - 1, col) == ".";
	bool canMoveDown = isValid(row + 1, col, mainBoard) && mainBoard.getBoardValue(row + 1, col) == ".";
	if (!canMoveLeft && !canMoveRight && !canMoveUp && !canMoveDown)
		return true;
	return false;
}

bool isClearPath(int row, int col, int t_row, int t_col) {
	if (t_row < row) {
		int temp = row;
		row = t_row;
		t_row = temp;
	}
	if (t_col < col) {
		int temp = col;
		col = t_col;
		t_col = temp;
	}

	for (int i = row; i <= t_row; i++) {
		for (int j = col; j <= t_col; j++) {
			if (mainBoard.getBoardValue(i, j) != ".")
				return false;
		}
	}
	return true;
}

bool isGoodRangeSize() {
	int rowCount = max(1, oppRange[1] - oppRange[0]);
	int colCount = max(1, oppRange[3] - oppRange[2]);
	int area = rowCount * colCount;
	return area <= 9;
}

bool isNeighbor(int row1, int col1, int row2, int col2) {
	if (row1 - 1 == row2 && col1 - 1 == col2)
		return true;
	if (row1 == row2 && col1 - 1 == col2)
		return true;
	if (row1 + 1 == row2 && col1 - 1 == col2)
		return true;
	if (row1 - 1 == row2 && col1 == col2)
		return true;
	if (row1 + 1 == row2 && col1 == col2)
		return true;
	if (row1 - 1 == row2 && col1 + 1 == col2)
		return true;
	if (row1 == row2 && col1 + 1 == col2)
		return true;
	if (row1 + 1 == row2 && col1 + 1 == col2)
		return true;
	return false;
}

/**************************************************************************
	Game Parameter Input

	This section contains functions that read turn input from the system.

***************************************************************************/

void readFirstTurnInput(int& width, int& height, int& myId) {
	cin >> width >> height >> myId; cin.ignore();
	for (int i = 0; i < height; i++) {
		string line;
		getline(cin, line);

		for (int j = 0; j < line.length(); j++) {
			mainBoard.setBoardValue(i, j, string(1, line[j]));
		}
	}
}

/**************************************************************************
	Opponent Command Parsing

	This section contains functions that parse opponent command string
	into usable data.

***************************************************************************/

bool getOpponentTorpedo(vector<int>& location, string opponentOrders) {
	int commandIndex = opponentOrders.find("TORPEDO");
	if (commandIndex > -1) {
		string parsedCommand = opponentOrders.substr(commandIndex);
		int length = parsedCommand.length();
		int pipeIndex = parsedCommand.find("|");
		if (pipeIndex > -1 && pipeIndex != commandIndex)
			length = pipeIndex;

		string xString = "";
		string yString = "";
		for (int i = 8; i < length; i++) {
			if (isdigit(parsedCommand[i])) {
				int xyLength = 1;
				if (i < 10)
					xString += parsedCommand.substr(i, xyLength);
				else
					yString += parsedCommand.substr(i, xyLength);
			}
		}

		location.push_back(stoi(xString));
		location.push_back(stoi(yString));
	}
	else {
		return false;
	}

	return true;
}

string getOppSurface(string opponentOrders) {
	int commandIndex = opponentOrders.find("SURFACE");
	string quadrant = "";
	if (commandIndex > -1) {
		quadrant = string(1, opponentOrders[commandIndex + 8]);
	}
	return quadrant;
}

string getOpponentMovement(string opponentOrders) {
	int commandIndex = opponentOrders.find("MOVE");
	string direction = "";
	if (commandIndex > -1) {
		direction = string(1, opponentOrders[commandIndex + 5]);
	}
	return direction;
}

vector<int> getOpponentTopedo(string opponentOrders) {
	vector<int> coordinates = { -1, -1 };
	cerr << "OPP ORDERS " << opponentOrders << endl;
	int commandIndex = opponentOrders.find("TORPEDO");
	string rowString = "";
	string colString = "";
	if (commandIndex > -1) {
		opponentOrders = opponentOrders.substr(commandIndex);
		if (opponentOrders[9] != ' ') {
			rowString = opponentOrders.substr(8, 2);
			if (opponentOrders[12] != ' ' && opponentOrders[12] != '|')
				colString = opponentOrders.substr(11, 2);
			else
				colString = opponentOrders.substr(11, 1);
		}
		else {
			rowString = opponentOrders.substr(8, 1);
			if (opponentOrders[11] != ' ' && opponentOrders[11] != '|')
				colString = opponentOrders.substr(10, 2);
			else
				colString = opponentOrders.substr(10, 1);
		}
		coordinates[0] = stoi(rowString);
		coordinates[1] = stoi(colString);
		//cerr << "ROW " << stoi(rowString) << " COL " << stoi(colString) << endl;
	}
	cerr << "OPP ORDERS " << opponentOrders << endl;
	//cerr << "ROW" << rowString << "COL" << colString << "END" << endl;

	return coordinates;
}

/**************************************************************************
	Opponent Position Tracking

	This section contains functions that update the probable location
	range of the opponent ship.

***************************************************************************/

void updateOppSuface(string oppSurface) {
	int minRow = 0;
	int maxRow = 1;
	int minCol = 2;
	int maxCol = 3;

	//update min and max opponent rows
	if (oppSurface == "1" || oppSurface == "2" || oppSurface == "3") {
		if (oppRange[minRow] > 4)
			oppRange[minRow] = 0;
		if (oppRange[maxRow] > 4)
			oppRange[maxRow] = 4;
	}
	else if (oppSurface == "4" || oppSurface == "5" || oppSurface == "6") {
		if (oppRange[minRow] < 5 || oppRange[minRow] > 9)
			oppRange[minRow] = 5;
		if (oppRange[maxRow] < 5 || oppRange[maxRow] > 9)
			oppRange[maxRow] = 9;
	}
	else if (oppSurface == "7" || oppSurface == "8" || oppSurface == "9") {
		if (oppRange[minRow] < 10)
			oppRange[minRow] = 10;
		if (oppRange[maxRow] < 10)
			oppRange[maxRow] = 14;
	}

	//update min and max opponent columns
	if (oppSurface == "1" || oppSurface == "4" || oppSurface == "7") {
		if (oppRange[minCol] > 4)
			oppRange[minCol] = 0;
		if (oppRange[maxCol] > 4)
			oppRange[maxCol] = 4;
	}
	else if (oppSurface == "2" || oppSurface == "5" || oppSurface == "8") {
		if (oppRange[minCol] < 5 || oppRange[minCol] > 9)
			oppRange[minCol] = 5;
		if (oppRange[maxCol] < 5 || oppRange[maxCol] > 9)
			oppRange[maxCol] = 9;
	}
	else if (oppSurface == "3" || oppSurface == "6" || oppSurface == "9") {
		if (oppRange[minCol] < 10)
			oppRange[minCol] = 10;
		if (oppRange[maxCol] < 10)
			oppRange[maxCol] = 14;
	}
}

void updateOppMoves(string oppMove) {
	int minRow = 0;
	int maxRow = 1;
	int minCol = 2;
	int maxCol = 3;

	if (oppMove == "N") {
		if (oppRange[minRow] > 0)
			oppRange[minRow]--;
		if (oppRange[maxRow] > 0)
			oppRange[maxRow]--;
	}
	else if (oppMove == "S") {
		if (oppRange[minRow] < 14)
			oppRange[minRow]++;
		if (oppRange[maxRow] < 14)
			oppRange[maxRow]++;
	}
	else if (oppMove == "E") {
		if (oppRange[minCol] < 14)
			oppRange[minCol]++;
		if (oppRange[maxCol] < 14)
			oppRange[maxCol]++;
	}
	else if (oppMove == "W") {
		if (oppRange[minCol] > 0)
			oppRange[minCol]--;
		if (oppRange[maxCol] > 0)
			oppRange[maxCol]--;
	}
}

void updateOppTorpedo(int row, int col) {
	int minRow = 0;
	int maxRow = 1;
	int minCol = 2;
	int maxCol = 3;

	if (row - 4 > oppRange[minRow] || row + 4 < oppRange[minRow])
		oppRange[minRow] = row - 4;
	if (row - 4 > oppRange[maxRow] || row + 4 < oppRange[maxRow])
		oppRange[maxRow] = row + 4;
	if (col - 4 > oppRange[minCol] || col + 4 < oppRange[minCol])
		oppRange[minCol] = col - 4;
	if (col - 4 > oppRange[maxCol] || col + 4 < oppRange[maxCol])
		oppRange[maxCol] = col + 4;
}

void updateProximityHit() {
	int minRowIndex = 0;
	int maxRowIndex = 1;
	int minColIndex = 2;
	int maxColIndex = 3;
	int minProxRow = targetPosition[0] - 1;
	int maxProxRow = targetPosition[0] + 1;
	int minProxCol = targetPosition[1] - 1;
	int maxProxCol = targetPosition[1] + 1;

	if (oppRange[minRowIndex] < minProxRow)
		oppRange[minRowIndex] = minProxRow;
	if (oppRange[maxRowIndex] > maxProxRow)
		oppRange[maxRowIndex] = maxProxRow;
	if (oppRange[minColIndex] < minProxCol)
		oppRange[minColIndex] = minProxCol;
	if (oppRange[maxColIndex] > maxProxCol)
		oppRange[maxColIndex] = maxProxCol;
}

void updateDirectHit() {
	int minRow = 0;
	int maxRow = 1;
	int minCol = 2;
	int maxCol = 3;
	oppRange[minRow] = targetPosition[0];
	oppRange[maxRow] = targetPosition[0];
	oppRange[minCol] = targetPosition[1];
	oppRange[maxCol] = targetPosition[1];
}


/**************************************************************************
	Command Generation

	This section contains functions that either return command strings
	for turn output, or support functions that return command strings.

***************************************************************************/

//Searches for a valid cell to spawn ship and issues command.
string getSpawnShipCommand() {
	vector<int> location = mainBoard.getSpawnLocation();
	int row = location[0];
	int col = location[1];
	myShip.setRow(row);
	myShip.setCol(col);
	return to_string(col) + " " + to_string(row);
	//return "0 0";
}

string getChargeCommand(int torpedoCooldown, int sonarCooldown, int silenceCooldown, int mineCooldown) {
	if (torpedoCooldown > 0)
		return "TORPEDO";
	//if (mineCooldown > 0)
	//	return "MINE";
	if (silenceCooldown > 0)
		return "SILENCE";
	if (mineCooldown > 0)
		return "MINE";

}

string moveShip(string charge, queue<string> moves, string nextMove) {
	int row = myShip.getRow();
	int col = myShip.getCol();
	mainBoard.setBoardValue(row, col, "o");

	//queue<string> moves;
	//string direction = getDirection(row, col, moves);

	int directionTest = nextMove.find("SURFACE");
	string command = "";

	if (nextMove == "N") {
		myShip.setRow(row - 1);
		myShip.setCol(col);
	}
	else if (nextMove == "E") {
		myShip.setRow(row);
		myShip.setCol(col + 1);
	}
	else if (nextMove == "S") {
		myShip.setRow(row + 1);
		myShip.setCol(col);
	}
	else if (nextMove == "W") {
		myShip.setRow(row);
		myShip.setCol(col - 1);
	}

	if (directionTest < 0 && nextMove != "") {
		command = "MOVE " + nextMove + " " + charge + "|";
	}


	return command;
}

string goSilent(int& row, int& col, queue<string> moves, string firstMove, int sameMoveCount) {
	// max distance in each direction - indices = { S, N, E, W }
	vector<int> maxDistances = { 0, 0, 0, 0 };

	string command = "SILENCE " + firstMove + " " + to_string(sameMoveCount) + "|";

	if (firstMove == "N") {
		int newPosition = myShip.getRow() - sameMoveCount;
		myShip.setRow(newPosition);
		for (int i = 0; i < sameMoveCount; i++)
			mainBoard.setBoardValue(row - i, col, "o");
		row = newPosition;
	}
	if (firstMove == "E") {
		int newPosition = myShip.getCol() + sameMoveCount;
		myShip.setCol(newPosition);
		for (int i = 0; i < sameMoveCount; i++)
			mainBoard.setBoardValue(row, col + i, "o");
		col = newPosition;
	}
	if (firstMove == "S") {
		int newPosition = myShip.getRow() + sameMoveCount;
		myShip.setRow(newPosition);
		for (int i = 0; i < sameMoveCount; i++)
			mainBoard.setBoardValue(row + i, col, "o");
		row = newPosition;
	}
	if (firstMove == "W") {
		int newPosition = myShip.getCol() - sameMoveCount;
		myShip.setCol(newPosition);
		for (int i = 0; i < sameMoveCount; i++)
			mainBoard.setBoardValue(row, col - i, "o");
		col = newPosition;
	}
	return command;
}

string getSilenceCommand(int row, int col) {
	// max distance in each direction - indices = { S, N, E, W }
	vector<int> maxDistances = { 0, 0, 0, 0 };

	for (int i = 1; i <= 4; i++) {
		if (isValid(row + i, col, mainBoard) && maxDistances[0] < row + i && isClearPath(row, col, row + i, col))
			maxDistances[0] = i;
		if (isValid(row - i, col, mainBoard) && maxDistances[1] < row - i && isClearPath(row, col, row - i, col))
			maxDistances[1] = i;
		if (isValid(row, col + i, mainBoard) && maxDistances[2] < col + i && isClearPath(row, col, row, col + i))
			maxDistances[2] = i;
		if (isValid(row, col - i, mainBoard) && maxDistances[3] < col - i && isClearPath(row, col, row, col - i))
			maxDistances[3] = i;
	}

	int maxIndex = 0;
	for (int i = 1; i < 4; i++) {
		if (maxDistances[maxIndex] < maxDistances[i])
			maxIndex = i;
	}

	string command = "";

	if (maxIndex == 0) {
		command = "SILENCE S " + to_string(maxDistances[0]);
		int newPosition = myShip.getRow() + maxDistances[0];
		myShip.setRow(newPosition);
		for (int i = 0; i < maxDistances[0]; i++)
			mainBoard.setBoardValue(row + i, col, "o");
	}if (maxIndex == 1) {
		command = "SILENCE N " + to_string(maxDistances[1]);
		int newPosition = myShip.getRow() - maxDistances[1];
		myShip.setRow(newPosition);
		for (int i = 0; i < maxDistances[1]; i++)
			mainBoard.setBoardValue(row - i, col, "o");
	}if (maxIndex == 2) {
		command = "SILENCE E " + to_string(maxDistances[2]);
		int newPosition = myShip.getCol() + maxDistances[2];
		myShip.setCol(newPosition);
		for (int i = 0; i < maxDistances[2]; i++)
			mainBoard.setBoardValue(row, col + i, "o");
	}if (maxIndex == 3) {
		command = "SILENCE W " + to_string(maxDistances[3]);
		int newPosition = myShip.getCol() - maxDistances[3];
		myShip.setCol(newPosition);
		for (int i = 0; i < maxDistances[3]; i++)
			mainBoard.setBoardValue(row, col - i, "o");
	}
	return command;
}

string getTorpedoTarget(int row, int col) {
	string direction = "";
	int section = getSectorId(row, col);
	if (section == 1 || section == 2 || section == 3)
		direction = "SOUTH";
	else if (section == 4)
		direction = "EAST";
	else if (section == 6)
		direction = "WEST";
	else if (section == 5 || section > 6)
		direction = "NORTH";
	return direction;
}

string fireTorpedo(int myRow, int myCol, int oppLife, int myLife) {
	int minRow = oppRange[0];
	int maxRow = oppRange[1];
	int minCol = oppRange[2];
	int maxCol = oppRange[3];

	int minRange = 2;
	int maxRange = 5;

	if (oppLife <= 2/* && myLife > 1*/)
		minRange = 0;
	if (oppLife <= 1 /*&& myLife > 2*/)
		minRange = -1;

	string command = "";

	if (minRow == maxRow && minCol == maxCol) {
		int rowDifference = abs(myShip.getRow() - minRow);
		int colDifference = abs(myShip.getCol() - minCol);
		int differenceSum = rowDifference + colDifference;
		if (differenceSum > minRange&& differenceSum < maxRange) {
			command = "TORPEDO " + to_string(minCol) + " " + to_string(minRow) + "|";
			targetPosition[0] = minRow;
			targetPosition[1] = minCol;
		}
	}
	if (command == "") {
		//if (minRow > 0) 
		//	minRow--;
		//if (maxRow < 14)
		//	maxRow++;
		//if (minCol > 0)
		//	minCol--;
		//if (maxCol < 14)
		//	maxCol++;

		for (int row = minRow; row <= maxRow; row++) {
			for (int col = minCol; col <= maxCol; col++) {
				//int rowDifference = abs(myRow - row);
				//int colDifference = abs(myCol - col);
				int rowDifference = abs(myShip.getRow() - row);
				int colDifference = abs(myShip.getCol() - col);
				int differenceSum = rowDifference + colDifference;
				if (differenceSum > minRange&& differenceSum < maxRange) {
					command = "TORPEDO " + to_string(col) + " " + to_string(row) + "|";
					targetPosition[0] = row;
					targetPosition[1] = col;
				}
			}
		}
	}
	if (command != "") {
		torpedoFired = true;
	}
	return command;
}

string placeMine() {
	cerr << "Placing mine from location " << myShip.getRow() << " " << myShip.getCol() << endl;
	int row = myShip.getRow();
	int col = myShip.getCol();
	//north
	if (isValid(row - 1, col, mineBoard) && mineBoard.getMineCount(getSectorId(row - 1, col)) < 5) {
		mineBoard.setBoardValue(row - 1, col, "m");
		return "MINE N|";
	}
	//south
	if (isValid(row + 1, col, mineBoard) && mineBoard.getMineCount(getSectorId(row + 1, col)) < 5) {
		mineBoard.setBoardValue(row + 1, col, "m");
		return "MINE S|";
	}
	//west
	if (isValid(row, col - 1, mineBoard) && mineBoard.getMineCount(getSectorId(row, col - 1)) < 5) {
		mineBoard.setBoardValue(row, col - 1, "m");
		return "MINE W|";
	}
	//east
	if (isValid(row, col + 1, mineBoard) && mineBoard.getMineCount(getSectorId(row, col + 1)) < 5) {
		mineBoard.setBoardValue(row, col + 1, "m");
		return "MINE E|";
	}

	return "";
}

string triggerMine() {
	string command = "";
	int minRow = oppRange[0];
	int maxRow = oppRange[1];
	int minCol = oppRange[2];
	int maxCol = oppRange[3];

	cerr << "MIN/MAX ROW " << minRow << " " << maxRow << "MIN/MAX COL " << minCol << " " << maxCol << endl;

	int height = max(1, (maxRow - minRow));
	int width = max(1, (maxCol - minCol));
	int range = height * width;
	if (height == width) {
		if (minRow > 0)
			minRow--;
		if (maxRow < 14)
			maxRow++;
		if (minCol > 0)
			minCol--;
		if (maxCol < 14)
			maxCol++;
	}
	if (range <= 9) {
		for (int row = minRow; row <= maxRow; row++) {
			for (int col = minCol; col <= maxCol; col++) {
				if (mineBoard.getBoardValue(row, col) == "m" && !isNeighbor(row, col, myShip.getRow(), myShip.getCol())) {
					command = "TRIGGER " + to_string(col) + " " + to_string(row) + "|";
					mineBoard.setBoardValue(row, col, ".");
					return command;
				}
			}
		}
	}

	return command;
}

int getSectorId(int row, int col) {
	int sector = 0;
	if (col < 5 && row < 5)
		sector = 1;
	else if (col < 5 && row < 10)
		sector = 4;
	else if (col < 5 && row < 15)
		sector = 7;
	else if (col < 10 && row < 5)
		sector = 2;
	else if (col < 10 && row < 10)
		sector = 5;
	else if (col < 10 && row < 15)
		sector = 8;
	else if (col < 15 && row < 5)
		sector = 3;
	else if (col < 15 && row < 10)
		sector = 6;
	else if (col < 15 && row < 15)
		sector = 9;

	return sector;
}

void resetVisitedLocations(Board& board) {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (board.getBoardValue(i, j) == "o") {
				board.setBoardValue(i, j, ".");
			}
		}
	}
	boardWasReset = !boardWasReset;
}

int getLongestPath(int row, int col, Board board, string firstMove, queue<string>& moves, int option) {
	board.setBoardValue(row, col, "x");

	bool canGoNorth = isValid(row - 1, col, board);
	bool canGoEast = isValid(row, col + 1, board);
	bool canGoSouth = isValid(row + 1, col, board);
	bool canGoWest = isValid(row, col - 1, board);

	int x = 0, y = 0, z = 0, w = 0;

	if (firstMove != "ALL") {
		if (firstMove == "NORTH" && canGoNorth) {
			moves.push("N");
			return 1 + getLongestPath(row - 1, col, board, "ALL", moves, option);
		}
		if (firstMove == "EAST" && canGoEast) {
			moves.push("E");
			return 1 + getLongestPath(row, col + 1, board, "ALL", moves, option);
		}
		if (firstMove == "SOUTH" && canGoSouth) {
			moves.push("S");
			return 1 + getLongestPath(row + 1, col, board, "ALL", moves, option);
		}
		if (firstMove == "WEST" && canGoWest) {
			moves.push("W");
			return 1 + getLongestPath(row, col - 1, board, "ALL", moves, option);
		}
	}
	else {
		if (option == 1) {
			if (canGoNorth) {
				moves.push("N");
				return 1 + getLongestPath(row - 1, col, board, "ALL", moves, option);
			}
			if (canGoEast) {
				moves.push("E");
				return 1 + getLongestPath(row, col + 1, board, "ALL", moves, option);
			}
			if (canGoSouth) {
				moves.push("S");
				return 1 + getLongestPath(row + 1, col, board, "ALL", moves, option);
			}
			if (canGoWest) {
				moves.push("W");
				return 1 + getLongestPath(row, col - 1, board, "ALL", moves, option);
			}
		}
		if (option == 2) {
			if (canGoSouth) {
				moves.push("S");
				return 1 + getLongestPath(row + 1, col, board, "ALL", moves, option);
			}
			if (canGoWest) {
				moves.push("W");
				return 1 + getLongestPath(row, col - 1, board, "ALL", moves, option);
			}
			if (canGoNorth) {
				moves.push("N");
				return 1 + getLongestPath(row - 1, col, board, "ALL", moves, option);
			}
			if (canGoEast) {
				moves.push("E");
				return 1 + getLongestPath(row, col + 1, board, "ALL", moves, option);
			}
		}
		if (option == 3) {
			if (canGoWest) {
				moves.push("W");
				return 1 + getLongestPath(row, col - 1, board, "ALL", moves, option);
			}
			if (canGoSouth) {
				moves.push("S");
				return 1 + getLongestPath(row + 1, col, board, "ALL", moves, option);
			}
			if (canGoEast) {
				moves.push("E");
				return 1 + getLongestPath(row, col + 1, board, "ALL", moves, option);
			}
			if (canGoNorth) {
				moves.push("N");
				return 1 + getLongestPath(row - 1, col, board, "ALL", moves, option);
			}
		}
		if (option == 4) {
			if (canGoEast) {
				moves.push("E");
				return 1 + getLongestPath(row, col + 1, board, "ALL", moves, option);
			}
			if (canGoNorth) {
				moves.push("N");
				return 1 + getLongestPath(row - 1, col, board, "ALL", moves, option);
			}
			if (canGoWest) {
				moves.push("W");
				return 1 + getLongestPath(row, col - 1, board, "ALL", moves, option);
			}
			if (canGoSouth) {
				moves.push("S");
				return 1 + getLongestPath(row + 1, col, board, "ALL", moves, option);
			}
		}
	}
	return 0;
}

string getDirection(int row, int col, queue<string>& moves, int option) {
	Board pathBoard = mainBoard;

	queue<string> northMoves;
	queue<string> eastMoves;
	queue<string> southMoves;
	queue<string> westMoves;

	int northPath = getLongestPath(row, col, pathBoard, "NORTH", northMoves, option);
	int eastPath = getLongestPath(row, col, pathBoard, "EAST", eastMoves, option);
	int southPath = getLongestPath(row, col, pathBoard, "SOUTH", southMoves, option);
	int westPath = getLongestPath(row, col, pathBoard, "WEST", westMoves, option);

	vector<int> paths = { northPath, eastPath, southPath, westPath };

	int index = 0;
	int longest = 0;
	for (int i = 0; i < 4; i++) {
		if (paths[i] > longest) {
			longest = paths[i];
			index = i;
		}
	}
	if (paths[index] != 0) {
		if (index == 0) {
			moves = northMoves;
			return "N";
		}
		if (index == 1) {
			moves = eastMoves;
			return "E";
		}
		if (index == 2) {
			moves = southMoves;
			return "S";
		}
		if (index == 3) {
			moves = westMoves;
			return "W";
		}

	}
	return "SURFACE";
}