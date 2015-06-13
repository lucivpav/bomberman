#include "bonus.h"

Bonus::Bonus(Block::Type type)
    :mType(type)
{
}

Block::Type Bonus::type() const
{
    return mType;
}
