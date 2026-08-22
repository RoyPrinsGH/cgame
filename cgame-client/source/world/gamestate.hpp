#pragma once
#include "ship.hpp"

namespace world
{
    class game_state
    {
    public:
        ship m_playerShip;
        std::vector<ship> m_enemyShips;
    };
}