#ifndef NS_UPL_H
#define NS_UPL_H

typedef enum _eCalcul
{
 eCalNotDef,     /// Pas Definit
 eCalNotSet,     /// Non traite
 eCalPending,    /// Sur liste des prochains
 eCalStarted,    /// Calcul encours
 eCalReady,      /// Calcul disponible
 eCalSkip,       /// Ne Pas faire de calcul
 eCalPaused,     /// Calcul mis en pause
 eCalInterrupted,/// Calcul interrompu
 eCalTerminated,  /// Calcul termine
 eCalEOL
}eUpl_Cal;

#endif // NS_UPL_H
