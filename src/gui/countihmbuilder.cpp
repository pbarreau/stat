#include "CountIhmBuilder.h"
#include "BcUpl.h"
#include "GameConfig.h"
#include "CountRenderer.h"
#include "BView_1.h"
#include <QTabWidget>
#include <QGridLayout>
#include <QString>
#include "LegacyAdapter.h"

namespace Fdj::Gui {

CountIhmBuilder::CountIhmBuilder() = default;

QWidget* CountIhmBuilder::build(const Core::GameConfig& game, CountType type, ZoneIndex zn, const CountRenderer& renderer) {
    auto* tabWidget = new QTabWidget();
    int maxWin = game.getLimitForZone(zn);
    int uplCount = std::min(maxWin, static_cast<int>(C_MAX_UPL));

    for (int g_id = C_MIN_UPL; g_id <= uplCount; ++g_id) {
        auto legacyConf = Fdj::Utils::toLegacyConfig(game);
        BView_1* view = new BView_1(legacyConf.get(), zn, static_cast<etCount>(type));
        auto* layout = new QGridLayout();

        bool success = renderer.render(game, zn, type, g_id, layout);
        if (!success) {
            delete view;
            continue; // ou return nullptr; si échec fatal souhaité
        }

        view->setLayout(layout);
        QString tabName = QString("Upl %1").arg(g_id);
        tabWidget->addTab(view, tabName);
    }

    return tabWidget;
}

} // namespace Fdj::Gui
