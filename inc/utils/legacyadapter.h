#ifndef LEGACYADAPTER_H
#define LEGACYADAPTER_H


#pragma once

#include "GameConfig.h"
#include "Bc.h"  // pour stGameConf

namespace Fdj::Utils {
// Deleter pour éviter les fuites mémoire des tableaux new[]
struct LegacyDeleter {
    void operator()(stGameConf* cfg) const noexcept {
        if (!cfg) return;
        delete[] cfg->limites;
        delete[] cfg->names;
        delete[] cfg->db_ref;
        // slFlt est un tableau de pointeurs -> si vous allouez des QStringList,
        // libérez-les ici avant de delete[] slFlt.
        delete[] cfg->slFlt;
        delete cfg;
    }
};

std::unique_ptr<stGameConf, LegacyDeleter> toLegacyConfig(const Fdj::Core::GameConfig& modernConfig);
}


#endif // LEGACYADAPTER_H
