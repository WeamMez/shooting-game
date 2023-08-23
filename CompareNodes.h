#ifndef _COMPARE_NODES_H
#define _COMPARE_NODES_H

#include "Node.h"
class CompareNodes
{
public:
	CompareNodes();
	~CompareNodes();

	bool operator() (Node* pn1, Node* pn2) {
		return pn1->getF() > pn2->getF();
	}
};

#endif