#include <stdexcept>
#include "BcUpl.h"
#include "LegacyAdapter.h"


namespace Fdj::Utils {

std::unique_ptr<stGameConf> toLegacyConfig(const Fdj::Core::GameConfig& modernConfig) {
    auto legacy = std::make_unique<stGameConf>();
    legacy->znCount = modernConfig.getZoneCount();

    for (int i = 0; i < legacy->znCount && i < C_MAX_UPL; ++i) {
        legacy->limites[i].win = modernConfig.getLimitForZone(i);
        // Étendre ici si d'autres champs sont requis
    }

    return legacy;
}

} // namespace Fdj::Utils
