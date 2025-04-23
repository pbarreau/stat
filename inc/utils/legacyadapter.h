#ifndef LEGACYADAPTER_H
#define LEGACYADAPTER_H


#pragma once

#include "GameConfig.h"
#include "Bc.h"  // pour stGameConf

namespace Fdj::Utils {
std::unique_ptr<stGameConf> toLegacyConfig(const Fdj::Core::GameConfig& modernConfig);
}


#endif // LEGACYADAPTER_H
