#ifndef BASICCOUNTRENDERER_H
#define BASICCOUNTRENDERER_H
#pragma once

#include "CountRenderer.h"
#include "Bc.h" // pour ptrFn_tbl et stMkLocal

namespace Fdj::Gui {
//struct stMkLocal;

//using  ptrFn_tbl = bool (BCount::*)(const stGameConf*, const stMkLocal, const int);

/**
 * @brief Implémentation concrète de CountRenderer
 *        qui encapsule une fonction utilisateur de type ptrFn_tbl.
 *
 * Utilisé pour afficher les résultats dans les onglets de CountIhmBuilder.
 */
class BasicCountRenderer : public CountRenderer {
public:
    /**
     * @brief Constructeur
     * @param fn Pointeur vers une fonction de génération de tableau (héritée de fdj-initial)
     */
    explicit BasicCountRenderer(const BCount::ptrFn_tbl fn);

    /**
     * @brief Injecte une vue de résultats dans le layout donné
     * @param game Configuration modernisée du jeu
     * @param zn Index de la zone concernée
     * @param type Type de calcul à effectuer
     * @param upl Niveau de regroupement (upl)
     * @param layout Layout Qt à remplir avec les résultats
     * @return true si le rendu a réussi, false sinon
     */
    bool render(const Fdj::Core::GameConfig& game,
                int zn,
                CountType type,
                int upl,
                QGridLayout* layout) const override;

private:
    BCount::ptrFn_tbl m_fn;
};

} // namespace Fdj::Gui


#endif // BASICCOUNTRENDERER_H
