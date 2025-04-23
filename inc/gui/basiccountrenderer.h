#ifndef BASICCOUNTRENDERER_H
#define BASICCOUNTRENDERER_H
#pragma once

#include "CountRenderer.h"
#include "Bc.h" // pour ptrFn_tbl et stMkLocal


#pragma once

#include "CountRenderer.h"
#include "Bc.h" // pour BCount::ptrFn_tbl et stMkLocal

namespace Fdj::Gui {

class BasicCountRenderer : public CountRenderer {
public:
    BasicCountRenderer(BCount* instance, BCount::ptrFn_tbl fn);

    bool render(const Core::GameConfig& game,
                int zn,
                CountType type,
                int upl,
                QGridLayout* layout) const override;

private:
    BCount* m_instance;          // pointeur sur l'objet BCount pour invoquer la méthode membre
    BCount::ptrFn_tbl m_fn;      // pointeur sur la méthode membre à appeler
};

} // namespace Fdj::Gui




#endif // BASICCOUNTRENDERER_H
