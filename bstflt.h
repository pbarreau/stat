#ifndef BSTFLT_H
#define BSTFLT_H

#include <QFlags>
#include <QMetaType>
#include <QString>

#include <QList>
#include <QItemSelection>
#include <QItemSelectionModel>

/// -------------
namespace Bp {
 /// https://wiki.qt.io/QFlags_tutorial
 /// https://www.qtcentre.org/threads/49096-Use-QFlags
 /// https://stackoverflow.com/questions/43478059/how-to-you-use-operator-int-of-qflags

 enum F_Flt{
  noFlt       = 0x0000,
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

 enum E_Clk{
  clkStop,
  clkStart
 };

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

 enum E_Col{
  noCol = -1,
  colId	=	0,
  colColor=1,
  colDate=1,
  colGenZs=1,  /// Colonne depart de zone def dans Tbl lst gene
  colJour=2,
  colTxt=2,
  colTotalv2=3,
  colVisual=3,
  colTirZs=3,  /// Colonne depart de zone def dans Tbl Tirages
  colEc=4,
  colTotalv0=4,
  colTotalv1=10
 };

 enum E_Ugm{
  ugmColZn=1,
  ugmColChk=6
 };

 enum E_Ico{
  icoFlt,    /// Show filtering
  icoRaz,    /// Raz selection
  icoShow,   /// visu rapide
  icoConfig, /// config courante
  icoNext,	 /// show next of filtering
  icoShowAll,
  icoShowChk,
  icoShowUhk
 };

 enum E_Tbv{
  noTbvSet,
  tbvElm_v1,
  tbvElm_v2,
  tbvOth
 };

 struct Btn{
  QString name;
  QString tooltips;
  E_Ico value;
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
 eCountUpl,     /// Uplets (2 ou 3)
 eCountEnd
}etCount;


typedef struct _stTbFiltres{
 QString tb_flt;     /// Nom de la table filtre
 QString tb_ref;   /// Nom de la table de reference tirages
 Bp::E_Sta sta;     /// Precision sur le code retour
 Bp::F_Flts b_flt; /// valeur flt
 int dbt;   /// total recupere de la base par read
 int id;    /// key table id
 int zne;         /// Zone id
 etCount typ;     /// Type de filtre
 int lgn;   /// ligne tbv en cours
 int col;   /// colonne
 int val;        /// Valeur a lgn & col
 int pri;   /// priorite
}stTbFiltres;

#endif // BSTFLT_H
