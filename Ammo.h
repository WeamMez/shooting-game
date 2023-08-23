#ifndef _AMMO_H
#define _AMMO_H

class Player;

const double STEP = 2.0;
class Ammo
{
public:
    enum Ammo_type {BULLET, GRANNADE};
    Ammo(Ammo_type type, Player *source, const Player *dest);
    bool move(); // Returns if arrived to destination
    void draw();
    double getX() {return curr_x;}
    double getY() {return curr_y;}
    void hurt(Player *p);

private:
    Ammo_type type;
    double source_x, source_y;
    double curr_x, curr_y;
    double dest_x, dest_y;
    double quotient, step_x, step_y;
};

#endif