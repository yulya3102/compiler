#pragma once

struct top
{
    bool operator==(top) const
    {
        return true;
    }

    bool operator!=(top) const
    {
        return false;
    }
};
