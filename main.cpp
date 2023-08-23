#include "main.h"
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include <queue>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include "Room.h"
#include "CompareNodes.h"
#include "Player.h"
#include "Carrier.h"
#include "Group.h"

using namespace std;

static array<Room, MAX_ROOMS> main_rooms;
static array<array<point_type, MAP_SIZE>, MAP_SIZE> main_map;
static array<array<point_type, MAP_SIZE>, MAP_SIZE> blank_map;

static array<Group, 2> groups = {1, 2};

static int winner_group = 0;

bool IsOverlapping(int cx, int cy, int h, int w, int room_index)
{
    int i, dx, dy;
    for (i = 0; i < room_index; i++)
    {
        dx = fabs(cx - main_rooms[i].getCenterCol());
        dy = fabs(cy - main_rooms[i].getCenterRow());
        if (dx < w / 2 + main_rooms[i].getWidth() / 2 + 4 && dy < h / 2 + main_rooms[i].getHeight() / 2 + 4)
            return true;
    }
    return false;
}

void DrawRooms()
{
    for (int i = 0; i < MAX_ROOMS; i++)
    {
        for (int x = main_rooms[i].getCenterCol() - main_rooms[i].getWidth() / 2; x < main_rooms[i].getCenterCol() + main_rooms[i].getWidth() / 2; x++)
            for (int y = main_rooms[i].getCenterRow() - main_rooms[i].getHeight() / 2; y < main_rooms[i].getCenterRow() + main_rooms[i].getHeight() / 2; y++)
                main_map[y][x] = point_type::ROOM;
    }
}

void draw_circ(Node *center)
{
    for (int i = -TUNNEL_WIDTH; i <= TUNNEL_WIDTH; i++)
        for (int j = -sqrt(TUNNEL_WIDTH * TUNNEL_WIDTH - i * i); j <= sqrt(TUNNEL_WIDTH * TUNNEL_WIDTH - i * i); j++)
        {
            main_map[center->getRow() + i][center->getCol() + j] = point_type::TUNNEL;
        }
}

void draw_with_direction(Node *n)
{
    static axis last_axis = axis::HORIZONTAL;
    static bool was_last_in_room = true;
    static vector<Node *> corners;

    if (n && n->getParent() && main_map[n->getRow()][n->getCol()] != point_type::ROOM && main_map[n->getRow()][n->getCol()] != point_type::TUNNEL)
    {
        if (n->getParent()->getRow() == n->getRow())
        {
            // change is vertical
            for (int i = -TUNNEL_WIDTH; i <= TUNNEL_WIDTH; i++)
                main_map[n->getRow() + i][n->getCol()] = point_type::TUNNEL;

            if (last_axis == axis::HORIZONTAL)
                corners.push_back(n);

            last_axis = axis::VERTICAL;
        }
        else if (n->getParent()->getCol() == n->getCol())
        {
            // change is horizontal
            for (int i = -TUNNEL_WIDTH; i <= TUNNEL_WIDTH; i++)
                main_map[n->getRow()][n->getCol() + i] = point_type::TUNNEL;

            if (last_axis == axis::VERTICAL)
                corners.push_back(n);

            last_axis = axis::HORIZONTAL;
        }

        if (was_last_in_room || main_map[n->getParent()->getRow()][n->getParent()->getCol()] == point_type::ROOM)
            corners.push_back(n);
        was_last_in_room = false;
    }
    else if ((n || !n->getParent()) && !was_last_in_room)
    {
        while (corners.size())
        {
            draw_circ(corners[0]);
            corners.erase(corners.begin());
        }

        draw_circ(n);

        was_last_in_room = true;
    }
}

void restore_path(Node *n)
{
    while (n != nullptr)
    {
        // cout << n->getRow() << ", " << n->getCol() << endl;
        // draw_circ(n);
        draw_with_direction(n);
        n = n->getParent();
    }
}

void AddNeighbor(Node *current, priority_queue<Node *, vector<Node *>, CompareNodes> *open_queue,
                 array<array<Node *, MAP_SIZE>, MAP_SIZE> *all_mat, int row, int col)
{
    Node *neighbor;
    double cost;

    if (main_map[current->getRow() + row][current->getCol() + col] == point_type::BACKGROUND)
        cost = WALL_COST;
    else
        cost = SPACE_COST;
    neighbor = new Node(current->getRow() + row, current->getCol() + col, current->getTargetRow(), current->getTargetCol(), current, current->getG() + cost);

    if ((*all_mat)[neighbor->getRow()][neighbor->getCol()])
        return;

    open_queue->push(neighbor);
    (*all_mat)[neighbor->getRow()][neighbor->getCol()] = neighbor;
}

// A*
void DigTunnel(int startIndex, int endIndex)
{
    priority_queue<Node *, vector<Node *>, CompareNodes> open_queue;
    array<array<Node *, MAP_SIZE>, MAP_SIZE> all_mat; // for saving the same entities as open_queue + closed_vec but here we can search.

    for (int i = 0; i < MAP_SIZE; i++)
        all_mat[i].fill(nullptr);

    Node *current = new Node(main_rooms[startIndex].getCenterRow(),
                             main_rooms[startIndex].getCenterCol(), main_rooms[endIndex].getCenterRow(),
                             main_rooms[endIndex].getCenterCol(), nullptr, 0);
    open_queue.push(current);
    all_mat[current->getRow()][current->getCol()] = current;

    // cout << "digging" << endl;
    while (!open_queue.empty())
    {
        current = open_queue.top();
        if (current->getRow() == main_rooms[endIndex].getCenterRow() &&
            current->getCol() == main_rooms[endIndex].getCenterCol())
        {
            restore_path(current);

            for (int i = 0; i < MAP_SIZE; i++)
                for (int j = 0; j < MAP_SIZE; j++)
                    if (all_mat[i][j])
                        delete all_mat[i][j];

            return;
        }

        open_queue.pop();

        if (current->getRow() > 0)
            AddNeighbor(current, &open_queue, &all_mat, -1, 0);
        if (current->getRow() < MAP_SIZE - 1)
            AddNeighbor(current, &open_queue, &all_mat, 1, 0);
        if (current->getCol() > 0)
            AddNeighbor(current, &open_queue, &all_mat, 0, -1);
        if (current->getCol() < MAP_SIZE - 1)
            AddNeighbor(current, &open_queue, &all_mat, 0, 1);
    }

    for (int i = 0; i < MAP_SIZE; i++)
        for (int j = 0; j < MAP_SIZE; j++)
            if (all_mat[i][j])
                delete all_mat[i][j];
}

void CreateTunnels()
{
    for (int i = 0; i < MAX_ROOMS; i++)
        for (int j = 0; j < MAX_ROOMS; j++)
            if (i != j)
                DigTunnel(i, j);
}

void InitRooms()
{
    int i, h, w, cx, cy;
    for (i = 0; i < MAX_ROOMS; i++)
    {
        do
        {
            h = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE);
            w = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE);
            cx = WINDOW_PADDING + h / 2 + rand() % (MAP_SIZE - 2 * WINDOW_PADDING - h);
            cy = WINDOW_PADDING + h / 2 + rand() % (MAP_SIZE - 2 * WINDOW_PADDING - h);
        } while (IsOverlapping(cx, cy, h, w, i));
        // set the parameters
        main_rooms[i].setCenterRow(cy);
        main_rooms[i].setCenterCol(cx);
        main_rooms[i].setHeight(h);
        main_rooms[i].setWidth(w);
    }

    CreateTunnels();
    DrawRooms();

    for (int i = 0; i < MAP_SIZE; i++)
        for (int j = 0; j < MAP_SIZE; j++)
            blank_map[i][j] = main_map[i][j];
}

void init()
{
    srand(time(0));
    glClearColor(255, 255, 255, 0);
    gluOrtho2D(0, MAP_SIZE, 0, MAP_SIZE);

    int room_count = 0;
    for (int i = 0; i < groups.size(); i++)
    {
        for (int j = 0; j < PLAYERS_IN_GROUP; j++)
        {
            groups[i].addPlayer(new Player(&groups[i], &groups[1 - i], static_cast<Character>(rand() % Character::CHARACTER_COUNT), main_rooms[room_count].getCenterCol(), main_rooms[room_count].getCenterRow()));
            room_count++;
        }

        groups[i].setCarrier(new Carrier(&groups[i], &groups[1 - i], main_rooms[room_count].getCenterCol(), main_rooms[room_count].getCenterRow()));
        room_count++;
    }
}

void draw_life(Player *p)
{
    if (p->getHealth() <= 0)
        return;
    const GLubyte HEALTH_COLOR[3] = {0, 255, 0};
    const GLubyte NEG_HEALTH_COLOR[3] = {255, 0, 0};
    const int HEALTH_LENGTH = 20;
    const int HP_IN_PIXEL = MAX_HEALTH / HEALTH_LENGTH;
    const int HEALTH_THICKNESS = 3;
    const int HEALTH_ABOVE = 4;

    int start_x = p->getX() - HEALTH_LENGTH / 2;
    int start_y = p->getY() + PLAYER_WIDTH + HEALTH_ABOVE;

    int health_pixels = p->getHealth() / HP_IN_PIXEL;
    int finish_x = start_x + health_pixels;
    int finish_y = start_y + HEALTH_THICKNESS;

    glColor3ubv(HEALTH_COLOR);
    glRecti(start_x, start_y, finish_x, finish_y);
    if (p->getHealth() < MAX_HEALTH)
    {
        glColor3ubv(NEG_HEALTH_COLOR);
        glRecti(finish_x, start_y, start_x + HEALTH_LENGTH, finish_y);
    }
}

void display()
{
    if (winner_group == 0)
    {
        array<array<point_type, MAP_SIZE>, MAP_SIZE> old_map;

        // reset map
        for (int i = 0; i < MAP_SIZE; i++)
            for (int j = 0; j < MAP_SIZE; j++)
            {
                old_map[i][j] = main_map[i][j];
                main_map[i][j] = blank_map[i][j];
            }

        for (int i = 0; i < groups.size(); i++)
        {
            groups[i].drawGroup(&main_map);
        }

        for (int i = 0; i < groups.size(); i++)
            groups[i].move(&old_map);

        const GLubyte point_colors[POINT_TYPE_COUNT][COLOR_CHANNELS] =
            {
                {255, 255, 255}, // WHITE
                {0, 0, 0},       // BLACK
                {0, 0, 0},       // also BLACK
                {255, 255, 0},   // YELLOW
                {255, 255, 153}, // LIGHT YELLOW
                {255, 0, 0},     // RED
                {250, 128, 114}, // SALMON
                {255, 128, 0},   // ORANGE
                {255, 128, 64},  // PINK
                {255, 255, 255}  // WHITE
            };
        glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

        glPointSize(1);
        for (int row = 0; row < MAP_SIZE; row++)
            for (int col = 0; col < MAP_SIZE; col++)
            {
                glColor3ubv(point_colors[main_map[row][col]]);
                glBegin(GL_QUADS);
                glVertex2d(col, row);
                glVertex2d(col + 1, row);
                glVertex2d(col + 1, row + 1);
                glVertex2d(col, row + 1);
                glEnd();
            }

        for (int i = 0; i < groups.size(); i++)
        {
            bool all_zero = true;
            for (int j = 0; j < PLAYERS_IN_GROUP; j++)
            {
                draw_life((*groups[i].getPlayers())[j]);
                if ((*groups[i].getPlayers())[j]->getHealth() > 0)
                    all_zero = false;
            }
            if (all_zero)
            {
                winner_group = 2 - i;
            }
        }
    }
    else
    {
        // GAME OVER
        glRasterPos2i(MAP_SIZE / 2 - 50, MAP_SIZE / 2); // clean frame buffer
        glColor3ub(0, 0, 0);
        glRecti(0, 0, MAP_SIZE, MAP_SIZE);
        glColor3ub(255, 255, 255);
        for (char c : "Game Over, winner is group ")
            glutBitmapCharacter(&glutBitmapHelvetica18, c);
        glutBitmapCharacter(&glutBitmapHelvetica18, winner_group + '0');
    }

    glutSwapBuffers(); // show all
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("AI Project");

    srand(time(0));
    InitRooms();
    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}

array<array<point_type, MAP_SIZE>, MAP_SIZE> *getMap()
{
    return &main_map;
}

array<Room, MAX_ROOMS> *getMainRooms()
{
    return &main_rooms;
}