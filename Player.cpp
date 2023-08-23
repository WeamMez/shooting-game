#include "Player.h"
#include <array>
// #include <queue>
#include "Group.h"
#include "Carrier.h"
#include <iostream>
#include <limits>

using namespace std;

Player::Player(Group *myGroup, Group *opponent, Character character, int x, int y) : myGroup(myGroup), opponent(opponent), character(character), x(x), y(y), health(MAX_HEALTH), granades(MAX_GRANADES), bullets(MAX_BULLETS), dest_node(nullptr), my_mode(PlayerMode::FIGHT)
{
    ammo_wait = 0;
}

bool in_room(int x, int y, Room *room)
{
    return (abs(x - room->getCenterCol()) <= room->getWidth() / 2) && (abs(y - room->getCenterRow()) <= room->getHeight() / 2);
}

bool Player::isInRoom(Room *r) const
{
    return in_room(this->getX(), this->getY(), r);
}

void Player::moveByDelta(int xMove, int yMove)
{
    if (health)
    {
        x += xMove;
        y += yMove;
    }
}

void Player::moveTo(int x, int y)
{
    if (health)
    {
        this->x = x;
        this->y = y;
    }

    // cout << "Player moved to (" << x << ", " << y << ")." << endl;
}

bool Player::useGranade(Player *dest)
{
    if (health && granades && !ammo_wait)
    {
        granades--;
        ammo_vec.push_back(Ammo(Ammo::Ammo_type::GRANNADE, this, dest));
        ammo_wait = AMMO_WAIT;
        return true;
    }
    if (!granades)
        return false;
    return true;
}

bool Player::useBullet(Player *dest)
{
    if (health && bullets && !ammo_wait)
    {
        bullets--;
        ammo_vec.push_back(Ammo(Ammo::Ammo_type::BULLET, this, dest));
        ammo_wait = AMMO_WAIT;
        return true;
    }
    if (!bullets)
        return false;
    return true;
}

Room *Player::getMyRoom() const
{
    array<Room, MAX_ROOMS> *all_rooms = getMainRooms();
    for (int i = 0; i < all_rooms->size(); i++)
        if (isInRoom(&(*all_rooms)[i]))
            return &(*all_rooms)[i];
    return nullptr;
}

bool runAwayCondition(const Player *player, int x, int y, array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    if ((*old_map)[y][x] != point_type::ROOM)
        return false;

    Room *myRoom = player->getMyRoom();
    vector<Player *> *opponent_players = player->getOpponent()->getPlayers();
    for (Player *op : (*opponent_players))
    {
        Room *op_room = op->getMyRoom();
        if (op_room && op_room->isInRoom(x, y))
            return false;
    }
    return true;
}

void addBFS(int row, int col, queue<NodeBFS *> *bfs_queue, array<array<bool, MAP_SIZE>, MAP_SIZE> *visited_mat, NodeBFS *parent, array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    if (row > 0 && col > 0 && row < MAP_SIZE && col < MAP_SIZE && ((*old_map)[row][col] != point_type::BACKGROUND || (*old_map)[row][col] == point_type::TUNNEL) && !(*visited_mat)[row][col])
    {
        // if ((*getMap())[row][col] == point_type::TUNNEL)
        // cout << "TUNNEL at " << row << "\t" << col << endl;
        (*visited_mat)[row][col] = true;
        (*bfs_queue).push(new NodeBFS(row, col)); //, parent));
    }
}

NodeBFS *Player::BFS(bool (*condition)(const Player *, int, int, array<array<point_type, MAP_SIZE>, MAP_SIZE> *), array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{

    NodeBFS *target = nullptr;
    // Room *myRoom = getMyRoom();
    queue<NodeBFS *> bfs_queue;
    array<array<bool, MAP_SIZE>, MAP_SIZE> visited_mat;

    for (int i = 0; i < MAP_SIZE; i++)
        visited_mat[i].fill(false);

    visited_mat[y][x] = true;
    bfs_queue.push(new NodeBFS(y, x)); //, nullptr));
    while (!bfs_queue.empty())
    {
        NodeBFS *n = bfs_queue.front();
        bfs_queue.pop();
        if ((*condition)(this, n->getCol(), n->getRow(), old_map))
        {
            target = n;
            break;
        }

        addBFS(n->getRow() + 1, n->getCol(), &bfs_queue, &visited_mat, n, old_map);
        addBFS(n->getRow() - 1, n->getCol(), &bfs_queue, &visited_mat, n, old_map);
        addBFS(n->getRow(), n->getCol() + 1, &bfs_queue, &visited_mat, n, old_map);
        addBFS(n->getRow(), n->getCol() - 1, &bfs_queue, &visited_mat, n, old_map);

        delete n;
    }

    while (!bfs_queue.empty())
    {
        delete bfs_queue.front();
        bfs_queue.pop();
    }

    return target;
}

double Player::dist(int ox, int oy) const
{
    double xDist = ox - this->x;
    double yDist = oy - this->y;
    double distance = sqrt(xDist * xDist + yDist * yDist);
    return distance;
}

double Player::calc_distance(Player *other) const
{
    return dist(other->x, other->y);
}

/*
void Player::hurt(int strength, Player *shooter)
{
    if (health && shooter->getGroup() != myGroup)
    {
        double distance = calc_distance(shooter);
        double power = distance / MAX_DIST;
        health = MAX(0, health - strength * power);
    }
}
*/

void Player::hurtByGranade(int source_x, int source_y)
{
    // Granades power fade away like d^2
    health = MAX(0, health - calcGranadePower(source_x, source_y));
}

void Player::hurtByBullet(int source_x, int source_y)
{
    health = MAX(0, health - calcBulletPower(source_x, source_y));
}

void Player::drawPlayer(array<array<point_type, MAP_SIZE>, MAP_SIZE> *map)
{
    if (health > 0)
        for (int i = -PLAYER_WIDTH; i < PLAYER_WIDTH; i++)
        {
            for (int j = -PLAYER_WIDTH; j < PLAYER_WIDTH; j++)
            {
                (*map)[y + i][x + j] = this->getGroup()->getNo() == 1 ? point_type::PLAYER_GROUP_1 : point_type::PLAYER_GROUP_2;
            }
        }
    for (Ammo a : ammo_vec)
    {
        a.draw();
    }
}

double Player::calcGranadePower(int source_x, int source_y)
{
    double distance = dist(source_x, source_y);
    double power = 1 - distance / MAX_DIST;
    // Granades power fade away like d^2
    return (GRANADE_STRENGTH * power * power);
}

double Player::calcBulletPower(int source_x, int source_y)
{
    double distance = dist(source_x, source_y);
    double power = 1 - distance / MAX_DIST;
    // Granades power fade away like d
    return (BULLET_STRENGTH * power);
}

void Player::survive(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    informHealth();
    bool op_in_room = false;
    if (dest_node)
    {
        vector<Player *> *opponent_players = opponent->getPlayers();
        for (Player *op : *opponent_players)
        {
            Room *op_room = op->getMyRoom();
            if (op_room && op_room->isInRoom(dest_node->getCol(), dest_node->getRow()))
            {
                op_in_room = true;
                break;
            }
        }
    }

    if (op_in_room || !dest_node) // If there is an opponent in the destination *OR there is no destination*
        dest_node = BFS(&runAwayCondition, old_map);

    // Room *my_room = getMyRoom();
    // if (!my_room || !my_room->isInRoom(dest_node->getCol(), dest_node->getRow()))
    a_star();
}

bool opponentCondition(const Player *player, int x, int y, array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    const int PLAYER_DIST = 10;
    // IF THE OPPONENT IS IN A TUNNEL WE DON'T CHASE HIM!
    /*     array<Room, MAX_ROOMS> *rooms = getMainRooms();
        bool is_in_room = false;
        for (int i = 0; i < rooms->size(); i++)
            if (in_room(x, y, &(*rooms)[i]))
            {
                is_in_room = true;
                break;
            }
        if (!is_in_room)
            return false; */

    // WE DON'T KNOW WHERE IS THE OPPONENT, WE HAVE TO SEARCH IN THE ROOMS BY FOOT
    vector<Player *> *opponent_players = player->getOpponent()->getPlayers();
    for (Player *op : *opponent_players)
    {
        if (op->getMyRoom() && op->getHealth() > 0 && abs(op->getX() - x) <= PLAYER_WIDTH && abs(op->getY() - y) <= PLAYER_WIDTH)
            return true;
    }
    return false;
    /* return (((*old_map)[y][x] == point_type::PLAYER_GROUP_1 && player->getGroup()->getNo() == 2) ||
           ((*old_map)[y][x] == point_type::PLAYER_GROUP_2 && player->getGroup()->getNo() == 1)); */
}

void Player::move(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    if (health > 0)
    {
        if (my_mode == PlayerMode::SURVIVE)
        {
            // cout << myGroup->getNo() << "survive" << endl;
            survive(old_map);
            if (health > RUN_AWAY[character])
            {
                my_mode = PlayerMode::FIGHT;
                dest_node = BFS(&opponentCondition, old_map);
            }
        }
        else // FIGHT MODE
        {
            // cout << myGroup->getNo() << "fight" << endl;
            fight(old_map);
            if (health <= RUN_AWAY[character])
            {
                my_mode = PlayerMode::SURVIVE;
                dest_node = BFS(&runAwayCondition, old_map);
            }
        }
    }
    for (vector<Ammo>::iterator iter = ammo_vec.begin(); iter < ammo_vec.end(); iter++)
    {
        if (iter->move())
        {
            for (Player *op : *(opponent->getPlayers()))
                if (op->dist(iter->getX(), iter->getY()) <= 2 * STEP)
                {
                    iter->hurt(op);
                    break;
                }
            ammo_vec.erase(iter);
        }
    }
    if (ammo_wait > 0)
        ammo_wait--;
}

void Player::shoot(Player *opponent_player)
{
    if (opponent_player->calcBulletPower(this->x, this->y) >= opponent_player->calcGranadePower(this->x, this->y))
    {
        if (!useBullet(opponent_player) && health)
        {
            informBullets();
            useGranade(opponent_player);
        }
    }
    else
    {
        if (!useGranade(opponent_player) && health)
        {
            informGranades();
            useBullet(opponent_player);
        }
    }
}

void Player::player_add_neighbor(Node *current, priority_queue<Node *, vector<Node *>, CompareNodes> *open_queue,
                                 array<array<Node *, MAP_SIZE>, MAP_SIZE> *all_mat, int row, int col)
{
    const int BG_COST = MAP_SIZE * MAP_SIZE;

    Node *neighbor;
    double cost;

    array<array<point_type, MAP_SIZE>, MAP_SIZE> *main_map = getMap();

    if ((*main_map)[current->getRow() + row][current->getCol() + col] == point_type::ROOM ||
        (*main_map)[current->getRow() + row][current->getCol() + col] == point_type::TUNNEL ||
        abs(y - (current->getRow() + row)) <= PLAYER_WIDTH && abs(x - (current->getCol() + col)) <= PLAYER_WIDTH)
        cost = SPACE_COST;
    else
        cost = BG_COST * SPACE_COST;
    neighbor = new Node(current->getRow() + row, current->getCol() + col, current->getTargetRow(), current->getTargetCol(), current, current->getG() + cost);

    if ((*all_mat)[neighbor->getRow()][neighbor->getCol()])
        delete neighbor;
    else
    {
        open_queue->push(neighbor);
        (*all_mat)[neighbor->getRow()][neighbor->getCol()] = neighbor;
    }
}

void Player::go_to_first_parent(Node *node, int speed)
{
    Node *valid_parent = node;
    for (int i = 0; i < speed; i++) // Check if there are enough valid parent levels for move in desired speed
    {
        valid_parent = valid_parent->getParent();
        if (!valid_parent)
        {
            moveTo(node->getCol(), node->getRow());
            return;
        }
    }

    while (valid_parent->getParent())
    {
        valid_parent = valid_parent->getParent();
        node = node->getParent();
    }

    moveTo(node->getCol(), node->getRow());
}

bool Player::a_star()
{
    priority_queue<Node *, vector<Node *>, CompareNodes> open_queue;
    array<array<Node *, MAP_SIZE>, MAP_SIZE> all_mat;

    for (int i = 0; i < MAP_SIZE; i++)
        all_mat[i].fill(nullptr);

    Node *current = new Node(y, x, dest_node->getRow(), dest_node->getCol(), nullptr, 0);
    open_queue.push(current);
    all_mat[current->getRow()][current->getCol()] = current;

    while (!open_queue.empty())
    {
        current = open_queue.top();
        if (current->getRow() == dest_node->getRow() && current->getCol() == dest_node->getCol())
        {
            if (getMyRoom())
                go_to_first_parent(current, ROOM_SPEED[character]);
            else
                go_to_first_parent(current, MAX_SPEED - ROOM_SPEED[character]);

            for (int i = 0; i < MAP_SIZE; i++)
                for (int j = 0; j < MAP_SIZE; j++)
                    if (all_mat[i][j])
                        delete all_mat[i][j];
            return true;
        }

        open_queue.pop();

        if (current->getRow() > 0)
            player_add_neighbor(current, &open_queue, &all_mat, -1, 0);
        if (current->getRow() < MAP_SIZE - 1)
            player_add_neighbor(current, &open_queue, &all_mat, 1, 0);
        if (current->getCol() > 0)
            player_add_neighbor(current, &open_queue, &all_mat, 0, -1);
        if (current->getCol() < MAP_SIZE - 1)
            player_add_neighbor(current, &open_queue, &all_mat, 0, 1);
    }

    for (int i = 0; i < MAP_SIZE; i++)
        for (int j = 0; j < MAP_SIZE; j++)
            if (all_mat[i][j])
                delete all_mat[i][j];
    return false;
}

Room *Player::closest_room()
{
    double curr_max_dist = MAP_SIZE * MAP_SIZE;
    Room *closest = nullptr;
    array<Room, MAX_ROOMS> *all_rooms = getMainRooms();
    for (int i = 0; i < all_rooms->size(); i++)
    {
        Room *curr = &(*all_rooms)[i];
        double room_dist = dist(curr->getCenterCol(), curr->getCenterRow());
        if (room_dist < curr_max_dist)
        {
            curr_max_dist = room_dist;
            closest = curr;
        }
    }

    return closest;
}

Player *Player::closest_opponent_player()
{
    double curr_max_dist = MAP_SIZE * MAP_SIZE;
    Player *closest = nullptr;
    vector<Player *> *all_op = opponent->getPlayers();
    for (int i = 0; i < all_op->size(); i++)
    {
        Player *curr = (*all_op)[i];
        if (curr->getHealth() <= 0)
            continue;
        double op_dist = dist(curr->getX(), curr->getY());
        if (op_dist < curr_max_dist)
        {
            curr_max_dist = op_dist;
            closest = curr;
        }
    }

    return closest;
}

void Player::fight(array<array<point_type, MAP_SIZE>, MAP_SIZE> *old_map)
{
    // If there is a player in destination, run BFS again
    bool op_in_room = false;
    for (Player *op : *opponent->getPlayers())
        if (op->getMyRoom())
        {
            op_in_room = true;
            break;
        }
    // if (!op_in_room)
    //     return;

    if (op_in_room && (!dest_node))
    {
        dest_node = BFS(&opponentCondition, old_map);
    }

    Room *my_room = getMyRoom();
    if (!my_room && dest_node)
    {
        a_star();
    }
    else if (my_room)
    {
        vector<Player *> *opponent_players = opponent->getPlayers();
        Player *op_in_my_room = nullptr;
        for (Player *op : (*opponent_players))
            if (op->isInRoom(my_room) && op->getHealth() > 0)
            {
                op_in_my_room = op;
                break;
            }

        if (op_in_my_room)
        {
            shoot(op_in_my_room);
        }
        else if (dest_node && (x != dest_node->getCol() || y != dest_node->getRow()))
        {
            a_star();
        }
        else
        {
            if (dest_node)
                delete dest_node;
            Room *dest_room = closest_opponent_player()->closest_room();
            dest_node = new NodeBFS(dest_room->getCenterRow(), dest_room->getCenterCol());
        }
    }
    else
    { // No dest_node and no my_room
        Room *dest_room = closest_opponent_player()->closest_room();
        dest_node = new NodeBFS(dest_room->getCenterRow(), dest_room->getCenterCol());
    }

    /* Room *myRoom = getMyRoom();

    if (!myRoom)
    {
        BFS(&opponentCondition, old_map);
        return;
    } // Don't fight in tunnels

    BFS(&opponentCondition, old_map); */
}

void Player::informHealth()
{
    myGroup->getCarrier()->setTarget(this, CARRYING::C_MEDICINE);
}

void Player::informGranades()
{
    myGroup->getCarrier()->setTarget(this, CARRYING::C_GRANADE);
}

void Player::informBullets()
{
    myGroup->getCarrier()->setTarget(this, CARRYING::C_BULLET);
}