#include "Group.h"
#include "Player.h"
#include "Carrier.h"
#include <iostream>

Group::Group(int groupNo):
groupNo(groupNo) {};

Group::~Group()
{
    delete carrier;
    for (Player *p : players)
        delete p;
}

void Group::addPlayer(Player *p)
{
    players.push_back(p);
}

void Group::drawGroup(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map) const
{
    for (int i = 0; i < players.size(); i++)
    {
        players[i]->drawPlayer(map);
    }
    this->carrier->drawCarrier(map);
}

void Group::move(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    for (int i = 0; i < players.size(); i++)
    {
        players[i]->move(old_map);
    }
    carrier->move();
}