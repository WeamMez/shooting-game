#ifndef _ROOM_H
#define _ROOM_H

class Room
{
private:
	int centerRow, centerCol, height,width;

public:
	Room();
	~Room();

	int getCenterRow() { return centerRow; }
	int getCenterCol() { return centerCol; }
	int getHeight() { return height; }
	int getWidth() { return width; }
	void setCenterRow(int r) { centerRow = r; }
	void setCenterCol(int c) { centerCol = c; }
	void setHeight(int h) { height = h; }
	void setWidth(int w) { width = w; }
	
	bool isInRoom(int x, int y);
};

#endif