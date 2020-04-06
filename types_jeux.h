#ifndef TYPES_JEUX_H
#define TYPES_JEUX_H

namespace NE_FDJ{
  typedef enum _les_jeux_a_tirages
  {
	fdj_none,   /// aucun type defini
	fdj_loto,   /// jeu : loto
	fdj_euro,   /// jeu : euromillion
	fdj_fini    /// fin de la liste des jeux possibles
  }etFdjType;
}

#endif // TYPES_JEUX_H
