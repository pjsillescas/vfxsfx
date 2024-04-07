#include "Player.h"


void Player::turnLeft()
{
	switch (direction)
	{
	case TDirection::NORTH:
		direction = TDirection::WEST;
		break;
	case TDirection::WEST:
		direction = TDirection::SOUTH;
		break;
	case TDirection::SOUTH:
		direction = TDirection::EAST;
		break;
	case TDirection::EAST:
	default:
		direction = TDirection::NORTH;
		break;
	}
}

void Player::turnRight()
{
	switch (direction)
	{
	case TDirection::NORTH:
		direction = TDirection::EAST;
		break;
	case TDirection::WEST:
		direction = TDirection::NORTH;
		break;
	case TDirection::SOUTH:
		direction = TDirection::WEST;
		break;
	case TDirection::EAST:
	default:
		direction = TDirection::SOUTH;
		break;
	}
}