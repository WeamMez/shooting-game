#ifndef _GROUP_H
#define _GROUP_H

#include <vector>

#include "main.h"

using namespace std;

class Player;
class Carrier;

class Group
{
private:
    int groupNo;
    vector<Player *> players;
    Carrier *carrier;

public:
    Group(int groupNo);

    ~Group();

    void addPlayer(Player *p);
    void setCarrier(Carrier *c) {carrier = c;}

    int getNo() const {return groupNo;}
    vector<Player *> *getPlayers() {return &players;}
    Carrier *getCarrier() {return carrier;}

    void drawGroup(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map) const;

    void move(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);
};

#endif