#ifndef NS_UPL_H
#define NS_UPL_H

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

#endif // NS_UPL_H
