#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {

    ifstream ifs(fileName);
    if(!ifs.is_open()) {
        cout << "Open file" << endl;
        return nullptr;
    }
    char** map;
    int playerRow;
    int playerCol; 

    //Taking inputs for height and width
    ifs >> maxRow;
    if (ifs.fail() || maxRow > 999999 || maxRow <= 0) {
        cout << "Error fail maxRow" << endl;
        return nullptr;
    }
    ifs >> maxCol;
    if (ifs.fail() || maxCol > 999999 || maxCol <= 0) {
        cout << "Error fail maxCol" << endl;
        return nullptr;
    }

    //Storing player row and player col
    ifs >> playerRow;
    if (ifs.fail() || playerRow >= maxRow  || playerRow < 0) {
        cout << "Error fail playerRow" << endl;
        return nullptr;
    }
    ifs >> playerCol;
    if (ifs.fail() || playerCol >= maxCol || playerCol < 0) {
        cout << "Error fail playerCol" << endl;
        return nullptr;
    }

    //CHecking number of spaces in not greater than INT_32
    if (maxRow > INT32_MAX/maxCol) {
        cout << "Error fail INT_32" << endl;
        return nullptr;
    }

    //Creating the map
    map = createMap(maxRow, maxCol);

    //Checking if map is created properly
    if (map == nullptr) {
        cout << "Error fail nullptr" << endl;
        return nullptr;
    }

    //Reading in the inputs from the file
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            ifs >> map[row][col];
            if (map[row][col] != TILE_OPEN &&
                map[row][col] != TILE_TREASURE && 
                map[row][col] != TILE_PILLAR &&
                map[row][col] != TILE_AMULET &&  
                map[row][col] != TILE_DOOR && 
                map[row][col] != TILE_EXIT &&
                map[row][col] != TILE_MONSTER) {
                deleteMap(map, maxRow);
                return nullptr;
               }
            if (ifs.fail()) {
                cout << "Error fail" << endl;
                deleteMap(map, maxRow);
                return nullptr;
            }
        }
    }

    //Checking if too many inputs are present
    string whitespace;
    ifs >> whitespace;

    for (unsigned int i=0; i<whitespace.size(); i++) {
        if (isspace(whitespace.at(i))==0) {
            cout << "Error fail too big" << endl;
            deleteMap(map, maxRow);
            return nullptr;
        }
    }

    if (!ifs.eof()) {
        cout << "Error fail too big" << endl;
        deleteMap(map, maxRow);
        return nullptr;
    }

    bool hasExit = false;
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            if (map[row][col] == TILE_EXIT || map[row][col] == TILE_DOOR) {
                hasExit = true;
            }
        }
    }

    if (!hasExit) {
        deleteMap(map, maxRow);
            return nullptr;
    }

    //Updating player's position and in the map
    player.col = playerCol;
    player.row = playerRow;
    map[playerRow][playerCol] = TILE_PLAYER;
    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    //up movement
    if (input == 'w') {
        nextRow = nextRow - 1;
        return;
    }

    //down movement
    if (input == 's') {
        nextRow = nextRow + 1;
        return;
    }

    //right movement
    if (input == 'd') {
        nextCol++;
        return;
    }

    //left movement
    if (input == 'a') {
        nextCol--;
        return;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {

    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    char** map = new char*[maxRow];
    for (int row = 0; row < maxRow; row++) {
        map[row] = new char[maxCol];
    }
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            map[row][col] = TILE_OPEN;
        }
    }
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    
    if (map == nullptr) {
        maxRow = 0;
        return;
    }

    if (maxRow == 0) {
        map = nullptr;
        return;
    }

    for (int row = 0; row < maxRow; row++) {
        delete[] map[row];
    }
    delete[] map;
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {

    //Checking for invalid conditions
    //Checking if map is null, maxRow is greater than 0 and not crossing in bounds when multiples
    //Same with maxCol
    if (map == nullptr || maxRow == 0 || maxCol == 0 || INT32_MAX/2 < maxCol || INT32_MAX/2 < maxRow) {
        return nullptr;
    }

    //Creating a new map with double the dimensions
    char** newMap = new char*[maxRow*2];
    for (int row = 0; row < maxRow*2; row++) {
        newMap[row] = new char[maxCol*2];
    }

    /*DEBUGGING COUT
    for (int row = 0; row < maxRow*2; row++) {
        for (int col = 0; col < maxCol*2; col++) {
            newMap[row][col] = TILE_OPEN;
            
        }  
    }

    for (int row = 0; row < maxRow*2; row++) {
        for (int col = 0; col < maxCol*2; col++) {
            cout << newMap[row][col];
        }
        cout << endl;
    }
    */

    //Copying over to subArray A
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            newMap[row][col] = map[row][col];
        }
    }

    //Copying over to subArray B
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            //Checking if player is in tile
            if (map[row][col] == TILE_PLAYER) {
                newMap[row][col + maxCol] = TILE_OPEN;
                continue;
            }
            newMap[row][col + maxCol] = map[row][col];
        }
    }

    //Copying over to subArray C
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            //Checking if player is in tile
            if (map[row][col] == TILE_PLAYER) {
                newMap[row + maxRow][col] = TILE_OPEN;
                continue;
            }
            newMap[row + maxRow][col] = map[row][col];
        }
    }

    //Copying over to subArray D
    for (int row = 0; row < maxRow; row++) {
        for (int col = 0; col < maxCol; col++) {
            //Checking if player is in tile
            if (map[row][col] == TILE_PLAYER) {
                newMap[row + maxRow][col + maxCol] = TILE_OPEN;
                continue;
            }
            newMap[row + maxRow][col + maxCol] = map[row][col];
        }
    }
 
    //Deallocating memory
    int orgRow = maxRow;
    deleteMap(map, maxRow);

    //Updating variables
    maxRow = orgRow*2;
    maxCol *= 2;
    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    
    int cur_status;
    int prevRow = player.row;
    int prevCol = player.col;
    //Out of bounds
    if ( nextRow >= maxRow || nextCol >= maxCol || nextCol < 0 || nextRow < 0) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }
    //Pillar or monster check
    if ( map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER) {
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    //When tile is a treasure
    if ( map[nextRow][nextCol] == TILE_TREASURE) {
        player.row = nextRow;
        player.col = nextCol;
        player.treasure += 1;
        cur_status = STATUS_TREASURE;
    }

    //When tile is an amulet
    else if (map[nextRow][nextCol] == TILE_AMULET) {
        player.row = nextRow;
        player.col = nextCol;
        cur_status =  STATUS_AMULET;
    }

    //When tile is a door
    else if (map[nextRow][nextCol] == TILE_DOOR) {
        player.row = nextRow;
        player.col = nextCol;
        cur_status = STATUS_LEAVE;
    }

    //When tile is an exit
    else if (map[nextRow][nextCol] == TILE_EXIT) {

        //When treasure is 0
        if (player.treasure == 0) {
            nextRow = player.row;
            nextCol = player.col;
            return STATUS_STAY;
        }

        //When tresure is not 0
        player.row = nextRow;
        player.col = nextCol;
        cur_status = STATUS_ESCAPE;
    }
    else if (map[nextRow][nextCol] == TILE_OPEN) {
        player.row = nextRow;
        player.col = nextCol;
        cur_status = STATUS_MOVE;
    }
    map[nextRow][nextCol] = TILE_PLAYER;
    map[prevRow][prevCol] = TILE_OPEN;
    return cur_status;

}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    //Storing player's current location
    int cur_col = player.col;
    int cur_row = player.row;

    //Checking above the player position
    for (int row = cur_row - 1; row >= 0; row--) {

        //Breaking if obstacle is reached
        if (map[row][cur_col] == TILE_PILLAR) {
            break;
        }

        //Moving monster towards player
        if ( map[row][cur_col] == TILE_MONSTER) {
            map[row][cur_col] = TILE_OPEN;
            map[row+1][cur_col] = TILE_MONSTER;
        }
    }

    //Checking below the player position
    for (int row = cur_row + 1; row < maxRow; row++) {

        //Breaking if obstacle is reached
        if (map[row][cur_col] == TILE_PILLAR) {
            break;
        }

        //Moving monster towards player
        if (map[row][cur_col] == TILE_MONSTER) {
            map[row][cur_col] = TILE_OPEN;
            map[row-1][cur_col] = TILE_MONSTER;
        }
    }

    //Checking left of the player position
    for (int col = cur_col - 1; col >= 0; col--) {

        //Breaking if obstacle is reached
        if (map[cur_row][col] == TILE_PILLAR) {
            break;
        }

        //Moving monster towards player
        if ( map[cur_row][col] == TILE_MONSTER) {
            map[cur_row][col] = TILE_OPEN;
            map[cur_row][col + 1] = TILE_MONSTER;
        }
    }

    //Checking right of the player position
    for (int col = cur_col + 1; col < maxCol; col++) {

        //Breaking if obstacle is reached
        if (map[cur_row][col] == TILE_PILLAR) {
            break;
        }

        //Moving monster towards player
        if (map[cur_row][col] == TILE_MONSTER) {
            map[cur_row][col] = TILE_OPEN;
            map[cur_row][col - 1] = TILE_MONSTER;
        }
    }

    return map[cur_row][cur_col] == TILE_MONSTER;
}
