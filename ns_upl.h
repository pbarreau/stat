#ifndef NS_UPL_H
#define NS_UPL_H

#include <QString>

typedef enum _E_Calcul
{
 E_CalNotDef,     /// Pas Definit
 E_CalNotSet,     /// Non traite
 E_CalPending,    /// Sur liste des prochains
 E_CalStarted,    /// Calcul encours
 E_CalReady,      /// Calcul disponible
 E_CalSkip,       /// Ne Pas faire de calcul
 E_CalPaused,     /// Calcul mis en pause
 E_CalInterrupted,/// Calcul interrompu
 E_CalTerminated,  /// Calcul termine
 E_CalEOL
}etCal;

typedef enum _E_CalTirages
{
 E_LstBle,  /// Liste des boules
 E_LstUpl,	/// Liste des uplets apartir des boules
 E_LstTirUpl,	/// Liste des tirages ayant ces uplets (J0)
 E_LstUplTot,	/// Total de chacun des uplets
 E_LstBleNot,	/// Ensemble complementaire des boules ( refa uplet 1, 2, 3)
 E_LstTirUplNext, /// Liste des tirages apres les uplets initiaux
 E_LstBleNext, /// Liste des boules jour  != 0
 E_LstUplNot,	/// Uplet 1,2,3 cree apartir de l'ensemble complementaire
 E_LstUplTotNot, /// Total de chacun des uplets
 E_LstUplNext, /// Liste des uplets apartir des boules next day
 E_LstUplTotNext, /// Total de chacun des uplets
 E_LstCal,        /// Fin des calculs possible
 E_LstShowCal,    /// Requete montrant les calculs
 E_LstShowUnion,   /// Requete synthese de chacque boule
 E_LstShowNotInUnion, /// Requete ensemble complementaire
 E_LstShowUplLst
 ///E_LstCalUsr,
 ///E_LstCalFdj,
}etLst;
#ifndef QT_NO_DEBUG
extern const QString TXT_SqlStep[E_LstCal];
#endif

#define C_TOT_CAL E_LstCal

typedef enum{
 E_CountToSet,    /// Pas de definition
 E_CountElm,      /// Comptage des boules de zones
 E_CountCmb,      /// ... des combinaisons
 E_CountGrp,       /// ... des regroupements
 E_CountBrc,      /// ... des barycentres
 E_CountUpl,     /// Uplets (2 ou 3)
 E_CountEnd
}etCount;

#endif // NS_UPL_H
