#include "Ammo.h"
#include "main.h"
#include "Player.h"
#include <cmath>
#include <climits>

Ammo::Ammo(Ammo_type type, Player *source, const Player *dest) : type(type), source_x(source->getX()), source_y(source->getY()), dest_x(dest->getX()), dest_y(dest->getY())
{
    curr_x = source_x;
    curr_y = source_y;
    if (dest_y - curr_y != 0)
        quotient = (dest_x - curr_x) / (dest_y - curr_y);
    else
        quotient = MAP_SIZE * MAP_SIZE;
    step_y = STEP / sqrt(quotient * quotient + 1);
    if (dest_y < curr_y)
        step_y *= -1;

    step_x = step_y * quotient;
}

bool Ammo::move()
{
    curr_x += step_x;
    curr_y += step_y;
    return ((step_x > 0 && curr_x >= dest_x) || (step_x <= 0 && curr_x <= dest_x)) && ((step_y > 0 && curr_y >= dest_y) || (step_y <= 0 && curr_y <= dest_y));
}

void Ammo::draw()
{
    array<array<point_type, MAP_SIZE>, MAP_SIZE> *map = (getMap());
    (*map)[curr_y][curr_x] = (type == Ammo_type::GRANNADE) ? point_type::GRANADE : point_type::BULLET;
}

void Ammo::hurt(Player *p)
{
    if (this->type == Ammo_type::BULLET)
        p->hurtByBullet(source_x, source_y);
    else
        p->hurtByGranade(source_x, source_y);
}