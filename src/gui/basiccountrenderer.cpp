#include "BasicCountRenderer.h"
#include "LegacyAdapter.h"
#include "DB_Tools.h"
#include "Bc.h"

namespace Fdj::Gui {

BasicCountRenderer::BasicCountRenderer(const BCount::ptrFn_tbl fn)
    : m_fn(fn) {}

bool BasicCountRenderer::render(const Fdj::Core::GameConfig& game,
                                int zn,
                                CountType type,
                                int upl,
                                QGridLayout* layout) const {
    auto legacyConf = Fdj::Utils::toLegacyConfig(game);
    stMkLocal prm;

    bool success = (this->*m_fn)(legacyConf.get(), prm, zn);

    if (!success) {
        QString label = QString("Upl %1").arg(upl);
        DB_Tools::DisplayError("BasicCountRenderer::render : " + label);
        return false;
    }

    // TODO : insérer les résultats dans layout à partir de prm
    return true;
}

} // namespace Fdj::Gui
