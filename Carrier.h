#ifndef _CARRIER_H
#define _CARRIER_H

#include "Room.h"
#include "main.h"
#include "Node.h"
#include "CompareNodes.h"
#include <queue>
#include <array>
#include <vector>

//#define CARRIER_WIDTH 4

class Player;
class Group;

enum CARRYING
{
    C_MEDICINE,
    C_GRANADE,
    C_BULLET
};

const int CARRIER_SPEED = 3;

class Carrier
{
private:
    int x, y;
    Group *myGroup, *opponent;
    int medicine, granades, bullets;
    CARRYING carrying; // The object requsted by the player
    Player *target;

    void navigationToPlayer();
    void restore_path(Node *n);
    void AddNeighbor(Node *current, priority_queue<Node *, vector<Node *>, CompareNodes> *open_queue, array<array<Node *, MAP_SIZE>, MAP_SIZE> *all_mat, int row, int col);
    void moveTo(int x, int y);
public:
    Carrier(Group *myGroup, Group *opponent, int x, int y);
    ~Carrier() {}

    void giveMedicine(Player *player);
    void giveBullets(Player *player);
    void giveGranades(Player *player);

    void setTarget(Player *p, CARRYING carry);

    Group *getGroup() {return myGroup;}

    void move();
    void drawCarrier(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map);
};

#endif