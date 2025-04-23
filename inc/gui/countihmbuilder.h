#ifndef COUNTIHMBUILDER_H
#define COUNTIHMBUILDER_H


#pragma once

#include <QWidget>
#include <memory>
#include "gameconfig.h"

namespace Fdj::Gui {

enum class CountType;
using ZoneIndex = int;

class CountRenderer;

class CountIhmBuilder {
public:
    CountIhmBuilder();
    QWidget* build(const Core::GameConfig& game, CountType type, ZoneIndex zn, const CountRenderer& renderer);
};

} // namespace Fdj::Gui


#endif // COUNTIHMBUILDER_H
