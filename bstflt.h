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

 enum F_Flt{
  noFlt  = 0x0000,
  fltTirLast	= 0x0001,
  fltTirPrev	=	0x0002,
  fltWanted   = 0x0004,
  fltSelected	= 0x0008,
  fltFiltred	= 0x0010,
  fltSeenNot	=	0x0020,
  fltSeenBfr	=	0x0040,
  fltSeenAft	=	0x0080
 };
 Q_DECLARE_FLAGS(F_Flts, F_Flt)

 enum E_Sta{
  noSta,
  Ok_Query,
  Ok_Result,
  isOkFld_pri,
  isOkFld_flt,
  isFalse,
  isAbsent,
  isErrFld_pri,
  isErrFld_flt,
  Er_Db,
  Er_Query,
  Er_Result,
  lyend
 };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Bp::F_Flts)
Q_DECLARE_METATYPE(Bp::F_Flts)
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
 QString tbName;     /// Nom de la table filtre
 Bp::E_Sta sta;     /// Precision sur le code retour
 Bp::F_Flts b_flt;
 int db_total;   /// total recupere de la base par read
 int tst_total;
 int id;
 int zne;         /// Zone id
 etCount typ;   /// Type de filtre
 int lgn;
 int col;
 int val;        /// Valeur a lgn & col
 int pri;
}stTbFiltres;

#endif // BSTFLT_H
