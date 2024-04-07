#pragma once

struct TSquare {
	bool isMonster;
	bool isWall;
	int i;
	int j;
};

enum class TDirection {
	NORTH,
	WEST,
	SOUTH,
	EAST,
};

class Player
{
public:
	TSquare* square;
	TDirection direction;

public:
	void turnLeft();
	void turnRight();
};

