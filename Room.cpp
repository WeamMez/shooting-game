#include "Room.h"



Room::Room()
{
}


Room::~Room()
{
}

bool Room::isInRoom(int x, int y)
{
    return (x < getCenterCol() + getWidth() / 2 &&
            x > getCenterCol() - getWidth() / 2 &&
            y < getCenterRow() + getHeight() / 2 &&
            y > getCenterRow() - getHeight() / 2);
}
