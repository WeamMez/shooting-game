//#pragma once

#ifndef _MAIN_H
#define _MAIN_H

#include "Room.h"
#include <array>

#define COLOR_CHANNELS 3
#define MAX_ROOMS 10
#define MAP_SIZE 400
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_PADDING 20
#define MIN_ROOM_SIZE 50
#define MAX_ROOM_SIZE 100
#define TUNNEL_WIDTH 12
#define WALL_COST 20.0
#define SPACE_COST 1.0
#define PLAYERS_IN_GROUP 2
#define CARRIERS_IN_GROUP 1
#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)<(y))?(y):(x))
#define MAX_GRANADES 20
#define MAX_BULLETS 80
#define MAX_HEALTH 100

#define GRANADE_STRENGTH 10
#define BULLET_STRENGTH 5

#define PLAYER_WIDTH 3

//#define DEBUG

enum point_type
{
    BACKGROUND,
    ROOM,
    TUNNEL,
    PLAYER_GROUP_1,
    CARRIER_GROUP_1,
    PLAYER_GROUP_2,
    CARRIER_GROUP_2,
    GRANADE,
    BULLET,
    MEDICINE,
    POINT_TYPE_COUNT
};

enum axis
{
    HORIZONTAL,
    VERTICAL
};

using namespace std;

array<Room, MAX_ROOMS> *getMainRooms();
array<array<point_type, MAP_SIZE>, MAP_SIZE> *getMap();

#endif