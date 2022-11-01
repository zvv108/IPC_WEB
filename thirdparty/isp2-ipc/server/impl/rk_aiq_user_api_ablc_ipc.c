
#include "config.h"
#if ENABLE_ALL
#include "rk_aiq_user_api_ablc.h"
#include "../../protocol/rk_aiq_user_api_ablc_ptl.h"
#include "../../client/impl/rk_aiq_user_api_ablc.c"
XCamReturn
rk_aiq_user_api_ablc_SetAttrib_ipc(void* args){
    printf("enter rk_aiq_user_api_ablc_SetAttrib_ipc\n");
    CALL_SET_AIQ_P(rk_aiq_user_api_ablc_SetAttrib);
}
XCamReturn
rk_aiq_user_api_ablc_GetAttrib_ipc(void* args){
    CALL_GET_AIQ(rk_aiq_user_api_ablc_GetAttrib);  
}

#endif
