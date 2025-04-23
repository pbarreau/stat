#ifndef COUNTRENDERER_H
#define COUNTRENDERER_H


#pragma once

#include "GameConfig.h"
#include <QGridLayout>

namespace Fdj::Gui {

enum class CountType;

class CountRenderer {
public:
    virtual ~CountRenderer();

    virtual bool render(const Fdj::Core::GameConfig& game,
                        int zn,
                        CountType type,
                        int upl,
                        QGridLayout* layout) const = 0;
};

} // namespace Fdj::Gui


#endif // COUNTRENDERER_H
