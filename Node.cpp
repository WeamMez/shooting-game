#include "Node.h"
#include <math.h>



Node::Node()
{
	g = 0;
	parent = nullptr;

}

Node::Node(int r, int c, int tr, int tc, Node* p, double new_g)
{
	row = r;
	col = c;
	target_row = tr;
	target_col = tc;
	parent = p;
	g = new_g;
	computeH();
	computeF();
}

Node::~Node()
{
}

// Manhattan Distance
void Node::computeH()
{
	h = fabs(row - target_row) + fabs(col - target_col);
}
