#ifndef __GF_HAL_H__
#define __GF_HAL_H__

#include "gf_error.h"
#include "gf_type_define.h"

#ifdef __cplusplus
extern "C" {
#endif

gf_error_t gf_hal_init(void *dev);
gf_error_t gf_hal_exit(void *dev);
gf_error_t gf_hal_cancel(void *dev);

uint64_t gf_hal_pre_enroll(void *dev);
gf_error_t gf_hal_enroll(void *dev, const void *hat, uint32_t group_id, uint32_t timeout_sec);
gf_error_t gf_hal_post_enroll(void *dev);
gf_error_t gf_hal_authenticate(void *dev, uint64_t operation_id, uint32_t group_id);
uint64_t gf_hal_get_auth_id(void *dev);
gf_error_t gf_hal_remove(void *dev, uint32_t group_id, uint32_t finger_id);
gf_error_t gf_hal_set_active_group(void *dev, uint32_t group_id);

gf_error_t gf_hal_enumerate(void *dev, void *results, uint32_t *max_size);
gf_error_t gf_hal_irq(void);
gf_error_t gf_hal_screen_on(void);
gf_error_t gf_hal_screen_off(void);
gf_error_t gf_hal_esd_check(void);

gf_error_t gf_hal_test_cmd(void* dev, uint32_t cmdId, const uint8_t* param, uint32_t param_len);

gf_error_t gf_hal_set_safe_class(void *dev, gf_safe_class_t safe_class);
gf_error_t gf_hal_set_session_id(void *dev, uint64_t session_id) ;
gf_error_t gf_hal_get_session_id(void *dev, uint64_t *session_id);
gf_error_t gf_hal_navigate(void *dev, gf_nav_mode_t nav_mode);

gf_error_t gf_hal_enable_fingerprint_module(void *dev, uint8_t enable_flag);
gf_error_t gf_hal_enable_unlock(void *dev);
gf_error_t gf_hal_disable_unlock(void *dev);
gf_error_t gf_hal_camera_capture(void *dev);
gf_error_t gf_hal_enable_ff_feature(void *dev, uint8_t enable_flag);

gf_error_t gf_hal_start_hbd(void* dev);


/*
 * @brief
 *  user extend command call TA
 *  @param[in] buffer:command buffer
 *  @param[in] len:command buffer length
 *  @return
 *      error number from TA
 **/
gf_error_t  gf_hal_invoke_command(uint8_t *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif //__GF_HAL_H__
