#ifndef GAMECONFIG_H
#define GAMECONFIG_H


#pragma once

#include <vector>

namespace Fdj::Core {

struct ZoneLimit {
    int win = 0;
};

class GameConfig {
public:
    explicit GameConfig(int zoneCount);

    int getZoneCount() const;
    int getLimitForZone(int zoneIndex) const;
    void setLimitForZone(int zoneIndex, int winLimit);

private:
    std::vector<ZoneLimit> zoneLimits;
};

} // namespace Fdj::Core


#endif // GAMECONFIG_H
