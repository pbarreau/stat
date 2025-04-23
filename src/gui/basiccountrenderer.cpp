#include "BasicCountRenderer.h"
#include "LegacyAdapter.h"
#include "DB_Tools.h"
#include <QString>

namespace Fdj::Gui {

BasicCountRenderer::BasicCountRenderer(BCount* instance, BCount::ptrFn_tbl fn)
    : m_instance(instance), m_fn(fn) {}

bool BasicCountRenderer::render(const Core::GameConfig& game,
                                int zn,
                                CountType type,
                                int upl,
                                QGridLayout* layout) const {
    auto legacyConf = Utils::toLegacyConfig(game);
    BCount::stMkLocal prm;

    bool success = (m_instance->*m_fn)(legacyConf.get(), prm, zn);


    // TODO : ajouter l'affichage dans layout si nécessaire
    return true;
}

} // namespace Fdj::Gui
