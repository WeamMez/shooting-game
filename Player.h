#ifndef _PLAYER_H
#define _PLAYER_H

#include <cmath>
#include <vector>
#include <queue>
#include "Room.h"
#include "main.h"
#include "Ammo.h"
#include "NodeBFS.h"
#include "Node.h"
#include "CompareNodes.h"

class Carrier;
class Group;

using namespace std;

const double MAX_DIST = sqrt(2.0) * MAX_ROOM_SIZE;
const int AMMO_WAIT = 20;
const int MAX_SPEED = 6;

enum Character
{
    COWARD,
    AGGRESSIVE,
    SUICIDAL,
    DEFENSIVE,
    CHARACTER_COUNT
};

enum PlayerMode
{
    FIGHT,
    SURVIVE,
    MODE_COUNT
};

class Player
{
private:
    int x, y;
    int granades, bullets, health;
    Character character;
    Group *myGroup, *opponent;
    //array<Room, MAX_ROOMS> *all_rooms;
    vector<Ammo> ammo_vec;
    int ammo_wait;
    PlayerMode my_mode;
    NodeBFS *dest_node;

    const int RUN_AWAY[CHARACTER_COUNT] = {50, 20, 0, 35};
    const int ROOM_SPEED[CHARACTER_COUNT] = {2, 3, 4, 2};

    //    void hurt(int strength, Player *shooter);

    bool isInRoom(Room *r) const;
    // void runAway(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);
    void moveTo(int x, int y);
    double calc_distance(Player *other) const;
    NodeBFS *BFS(bool (*condition)(const Player *, int, int, array<array<point_type, MAP_SIZE>, MAP_SIZE> *), array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);

    friend bool runAwayCondition(const Player *player, int x, int y, array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);
    friend bool opponentCondition(const Player *player, int x, int y, array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);
    void go_to_first_parent(Node *node, int speed);
    // Room *node_to_room(NodeBFS *nbfs);
    bool a_star();
    void shoot(Player *opponent_player);
    void player_add_neighbor(Node *current, priority_queue<Node *, vector<Node *>, CompareNodes> *open_queue,
                             array<array<Node *, MAP_SIZE>, MAP_SIZE> *all_mat, int row, int col);
    Room *closest_room();
    Player *closest_opponent_player();

public:
    Player(Group *myGroup, Group *opponent, Character character, int x, int y);

    void drawPlayer(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map);

    double calcGranadePower(int source_x, int source_y);
    double calcBulletPower(int source_x, int source_y);
    double dist(int x, int y) const;
    int getX() const { return x; }
    int getY() const { return y; }
    Group *getGroup() const { return myGroup; }
    void moveByDelta(int xMove, int yMove);
    void move(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);

    int getGranades() const { return granades; }
    int getBullets() const { return bullets; }
    int getHealth() const { return health; }
    Room *getMyRoom() const;
    Group *getOpponent() const { return opponent; }

    void addGranade(int addition)
    {
        if (health)
            granades = min(MAX_GRANADES, granades + addition);
    };
    void addBullet(int addition)
    {
        if (health)
            bullets = min(MAX_BULLETS, bullets + addition);
    };
    void addHealth(int addition)
    {
        if (health)
            health = min(MAX_HEALTH, health + addition);
    };

    void hurtByGranade(int source_x, int source_y);
    void hurtByBullet(int source_x, int source_y); // {if (health && shooter != myGroup) health = min(0, health - BULLET_STRENGTH);}

    bool useGranade(Player *dest);
    bool useBullet(Player *dest);

    void survive(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);
    void fight(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map);

    void informHealth();
    void informGranades();
    void informBullets();
};

#endif