#ifndef _NODE_BFS_H
#define _NODE_BFS_H

class NodeBFS
{
private:
	// NodeBFS* parent;
	int row, col;

public:
	// NodeBFS();
	NodeBFS(int r, int c);//, NodeBFS* p);
	// ~NodeBFS();

	int getRow() const { return row; }
	int getCol() const { return col; }

	// NodeBFS *getParent() {return parent;}


	bool operator ==(NodeBFS& other) const { return other.row == row && other.col == col; }
};

#endif