#include "core/GameConfig.h"
#include <stdexcept>

namespace Fdj::Core {

GameConfig::GameConfig(int zoneCount) : zoneLimits(zoneCount) {}

int GameConfig::getZoneCount() const {
    return static_cast<int>(zoneLimits.size());
}

int GameConfig::getLimitForZone(int zoneIndex) const {
    if (zoneIndex < 0 || zoneIndex >= getZoneCount()) {
        throw std::out_of_range("Invalid zone index");
    }
    return zoneLimits[zoneIndex].win;
}

void GameConfig::setLimitForZone(int zoneIndex, int winLimit) {
    if (zoneIndex < 0 || zoneIndex >= getZoneCount()) {
        throw std::out_of_range("Invalid zone index");
    }
    zoneLimits[zoneIndex].win = winLimit;
}

} // namespace Fdj::Core
