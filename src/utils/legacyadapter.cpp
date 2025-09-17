#include <stdexcept>
#include "BcUpl.h"
#include "LegacyAdapter.h"

#if 0
namespace Fdj { namespace Utils {

std::unique_ptr<stGameConf> toLegacyConfig(const Fdj::Core::GameConfig& modernConfig) {
    auto legacy = std::make_unique<stGameConf>();
    legacy->znCount = modernConfig.getZoneCount();

    for (int i = 0; i < legacy->znCount && i < C_MAX_UPL; ++i) {
        legacy->limites[i].win = modernConfig.getLimitForZone(i);
        // Étendre ici si d'autres champs sont requis
    }

    return legacy;
}

} } // namespace Fdj::Utils

#endif

#include "game.h"      // assure les définitions stGameConf / stParam_1 / C_MAX_UPL
#include <algorithm>   // std::min

namespace Fdj { namespace Utils {


std::unique_ptr<stGameConf, LegacyDeleter>
toLegacyConfig(const Fdj::Core::GameConfig& modernConfig)
{
    // value-init pour mettre tous les champs à 0/nullptr
    auto legacy = std::unique_ptr<stGameConf, LegacyDeleter>(new stGameConf{});

    int znCount = modernConfig.getZoneCount();
    if (znCount < 0) znCount = 0;
    if (znCount > C_MAX_UPL) znCount = C_MAX_UPL;
    legacy->znCount = znCount;

    // Allouer les tableaux (zero-initialisés)
    legacy->limites = new stParam_1[znCount]{};   // requis pour ta boucle
    legacy->names   = new stParam_2[znCount]{};   // si tu les remplis plus tard
    legacy->db_ref  = new stParam_3[znCount]{};
    legacy->slFlt   = new QStringList*[znCount]{}; // pointeurs initialisés à nullptr

    // Remplissage des zones (exemple : seul 'win' ici)
    for (int i = 0; i < znCount; ++i) {
        legacy->limites[i].win = modernConfig.getLimitForZone(i);
        // TODO: remplir les autres champs si nécessaires:
        // legacy->limites[i].col = ...
        // legacy->limites[i].len = ...
        // legacy->limites[i].min = ...
        // legacy->limites[i].max = ...
        // legacy->names[i]       = ...
        // legacy->db_ref[i]      = ...
    }

    return legacy;
}

}} // namespace
