#ifndef _NODE_H
#define _NODE_H

class Node
{
private:
	Node* parent;
	int row, col,target_row,target_col;
	double g, h, f;

public:
	Node();
	Node(int r, int c, int tr, int tc, Node* p, double new_g);
	~Node();
	void computeH();
	void computeF() { f = g + h; }

	void setG(int value) { g = value; }
	void setH(int value) { h = value; }

	double getF() const { return f; }
	double getH() const { return h; }
	double getG() const { return g; }

	int getRow() const { return row; }
	int getCol() const { return col; }
	int getTargetRow() const { return target_row; }
	int getTargetCol() const { return target_col; }

	Node *getParent() {return parent;}


	bool operator ==(Node& other) const { return other.row == row && other.col == col; }
};

#endif