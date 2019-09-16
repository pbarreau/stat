#ifndef PROPERTIES_H
#define PROPERTIES_H

#define BMAX_2(a,b) (((a)>(b))?(a):(b))
#define BMIN_2(a,b) (((a)<(b))?(a):(b))

#ifdef QT_NO_DEBUG
#define SET_DBG_LIVE    false
#endif

#define SET_RUN_CHKP    true  /// RUN CHECK POINT

#define SET_DBG_LEV1    false
#define SET_DBG_LEV2    false
#define SET_DBG_LEV3    true

#define TRY_CODE_NEW    1

#define SET_DBG_QLV1    true
#define SET_RUN_QLV1    true


#define SET_QRY_ACT1    false

#define SET_QRY_DBG1    true
#define SET_QRY_DBG2    false

#define CNP_SHOW_MSG    true

#endif // PROPERTIES_H
