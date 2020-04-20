#ifndef BSTFLT_H
#define BSTFLT_H

#include <QFlags>
#include <QMetaType>
#include <QString>

/// -------------
namespace Bp {
 /// https://wiki.qt.io/QFlags_tutorial
 /// https://www.qtcentre.org/threads/49096-Use-QFlags
 /// https://stackoverflow.com/questions/43478059/how-to-you-use-operator-int-of-qflags

 enum Filtering{
  isNotSet  = 0x0000,
  isLastTir	= 0x0001,
  isPrevTir	=	0x0002,
  isWanted	=	0x0004,
  isFiltred	= 0x0008,
  isNotSeen	=	0x0010,
  isSeenBfr	=	0x0020,
  isSeenAft	=	0x0040
 };
 Q_DECLARE_FLAGS(Filterings, Filtering)

 enum FltLayer{
  lyFiltred,
  lyLastTir,
  lyPrevTir,
  lyNotSeen,
  lyend
 };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Bp::Filterings)
Q_DECLARE_METATYPE(Bp::Filterings)
/// -------------


typedef enum{
 eCountToSet,    /// Pas de definition
 eCountElm,      /// Comptage des boules de zones
 eCountCmb,      /// ... des combinaisons
 eCountGrp,       /// ... des regroupements
 eCountBrc,      /// ... des barycentres
 eCountEnd
}etCount;


typedef struct _stTbFiltres{
 QString tbName; /// Nom de la table filtre
 bool isPresent; /// La structure a des infos correctes
 int id;
 int zne;         /// Zone id
 etCount typ;   /// Type de filtre
 int lgn;
 int col;
 int val;        /// Valeur a lgn & col
 int pri;
 int flt;        /// bit field
 Bp::Filterings b_flt;
}stTbFiltres;

#endif // BSTFLT_H
