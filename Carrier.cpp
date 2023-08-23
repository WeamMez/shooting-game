#include "Carrier.h"
#include "CompareNodes.h"
#include <cmath>
#include "Player.h"
#include "Group.h"

Carrier::Carrier(Group *myGroup, Group *opponent, int x, int y) : myGroup(myGroup), opponent(opponent), x(x), y(y), medicine(MAX_HEALTH), granades(MAX_GRANADES), bullets(MAX_BULLETS), target(nullptr){};

void Carrier::AddNeighbor(Node *current, priority_queue<Node *, vector<Node *>, CompareNodes> *open_queue, array<array<Node *, MAP_SIZE>, MAP_SIZE> *all_mat, int row, int col)
{
    if (current->getRow() + row < 0 || current->getRow() + row > MAP_SIZE || current->getCol() + col < 0 || current->getCol() + col > MAP_SIZE)
        return;
    if (((*getMap())[current->getRow() + row][current->getCol() + col] == point_type::ROOM ||
         (*getMap())[current->getRow() + row][current->getCol() + col] == point_type::TUNNEL ||
         (abs(current->getRow() + row - target->getY()) <= PLAYER_WIDTH && abs(current->getCol() + col - target->getX()) <= PLAYER_WIDTH) ||
         (abs(current->getRow() + row - y) <= PLAYER_WIDTH && abs(current->getCol() + col - x) <= PLAYER_WIDTH)) &&
        !(*all_mat)[current->getRow() + row][current->getCol() + col])
    {
        (*all_mat)[current->getRow() + row][current->getCol() + col] = new Node(current->getRow() + row, current->getCol() + col, current->getTargetRow(), current->getTargetCol(), current, current->getG() + SPACE_COST);
        open_queue->push((*all_mat)[current->getRow() + row][current->getCol() + col]);
    };
}

void Carrier::restore_path(Node *n)
{
    Node *shifted = n;
    for (int i = 0; i < CARRIER_SPEED; i++)
    {
        n = n->getParent();
        if (!n)
        {
            moveTo(shifted->getCol(), shifted->getRow());
            return;
        }
    }

    while (n->getParent())
    {
        n = n->getParent();
        shifted = shifted->getParent();
    }
    moveTo(shifted->getCol(), shifted->getRow());
}

void Carrier::navigationToPlayer()
{
    // A* to player.
    int tx = target->getX(), ty = target->getY();
    priority_queue<Node *, vector<Node *>, CompareNodes> open_queue;
    array<array<Node *, MAP_SIZE>, MAP_SIZE> all_mat;
    for (int i = 0; i < MAP_SIZE; i++)
        all_mat[i].fill(nullptr);

    Node *current = new Node(y, x, ty, tx, nullptr, 0);
    open_queue.push(current);
    all_mat[current->getRow()][current->getCol()] = current;

    while (!open_queue.empty())
    {
        current = open_queue.top();
        if (abs(current->getRow() - ty) <= PLAYER_WIDTH &&
            abs(current->getCol() - tx) <= PLAYER_WIDTH)
        {
            restore_path(current);

            for (int i = 0; i < MAP_SIZE; i++)
                for (int j = 0; j < MAP_SIZE; j++)
                    if (all_mat[i][j])
                        delete all_mat[i][j];

            return;
        }

        open_queue.pop();

        AddNeighbor(current, &open_queue, &all_mat, -1, 0);
        AddNeighbor(current, &open_queue, &all_mat, 1, 0);
        AddNeighbor(current, &open_queue, &all_mat, 0, -1);
        AddNeighbor(current, &open_queue, &all_mat, 0, 1);
    }

    for (int i = 0; i < MAP_SIZE; i++)
        for (int j = 0; j < MAP_SIZE; j++)
            if (all_mat[i][j])
                delete all_mat[i][j];
}

void Carrier::moveTo(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Carrier::giveMedicine(Player *player)
{
    if (this->medicine > 0)
    {
        int toGive = MIN(MAX_HEALTH / 2, this->medicine);
        player->addHealth(toGive);
        this->medicine -= toGive;
    }

    target = nullptr;
}

void Carrier::giveBullets(Player *player)
{
    if (this->bullets > 0)
    {
        int toGive = MIN(MAX_BULLETS / 2, this->bullets);
        player->addBullet(toGive);
        this->bullets -= toGive;
    }

    target = nullptr;
}

void Carrier::giveGranades(Player *player)
{
    if (this->granades > 0)
    {
        int toGive = MIN(MAX_GRANADES / 2, this->granades);
        player->addGranade(toGive);
        this->granades -= toGive;
    }

    target = nullptr;
}

void Carrier::setTarget(Player *p, CARRYING carry)
{
    switch (carry)
    {
    case CARRYING::C_BULLET:
        if (bullets == 0)
            return;
        break;
    case CARRYING::C_GRANADE:
        if (granades == 0)
            return;
        break;
    case CARRYING::C_MEDICINE:
        if (medicine == 0)
            return;
        break;
    }
    if (!target)
    {
        target = p;
        carrying = carry;
    }
}

void Carrier::move()
{
    if (target && target->getHealth() > 0)
    {
        navigationToPlayer();
        if (abs(y - target->getY()) <= PLAYER_WIDTH &&
            abs(x - target->getX()) <= PLAYER_WIDTH)
        {
            switch (carrying)
            {
            case CARRYING::C_MEDICINE:
                giveMedicine(target);
                break;
            case CARRYING::C_BULLET:
                giveBullets(target);
                break;
            case CARRYING::C_GRANADE:
                giveGranades(target);
                break;
            default:
                break;
            }
        }
    }
}

void Carrier::drawCarrier(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map)
{
    for (int i = -PLAYER_WIDTH; i < PLAYER_WIDTH; i++)
        for (int j = -(PLAYER_WIDTH - abs(i)); j < PLAYER_WIDTH - abs(i); j++)
        {
            (*map)[y + i][x + j] = this->getGroup()->getNo() == 1 ? point_type::CARRIER_GROUP_1 : point_type::CARRIER_GROUP_2;
        }
}