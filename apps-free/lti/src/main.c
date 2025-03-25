/**
 * $Id$
 *
 * @brief Red Pitaya LTI main module.
 *
 * @Author Jure Menart <juremenart@gmail.com>
 * @Author Dashpi <dashpi46@gmail.com>
 *         
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "main.h"
#include "version.h"
#include "worker.h"
#include "fpga_lti.h"
#include "generate_basic.h"
#include "calib.h"

/* Describe app. parameters with some info/limitations */
static rp_app_params_t rp_main_params[PARAMS_NUM+1] = {
    { /* xmin - currently ignored */ 
        "xmin", -1000000, 0, 1, -10000000, +10000000 },
    { /* xmax - currently ignored   */ 
        "xmax", +1000000, 0, 1, -10000000, +10000000 },
    { /* freq_range::
       *    0 - 62.5 [MHz]
       *    1 - 7.8 [MHz]
       *    2 - 976 [kHz]
       *    3 - 61 [kHz]
       *    4 - 7.6 [kHz]
       *    5 - 953 [Hz] */
        "freq_range", 3, 1, 0,         0,         5 },
    { /* freq_unit:
       *    0 - [Hz]
       *    1 - [kHz]
       *    2 - [MHz]   */
        "freq_unit",  0, 0, 1,         0,         2 },
    { /* en_avg_at_dec:
		   *    0 - disable
		   *    1 - enable */
	"en_avg_at_dec", 1, 0, 1,      0,         1 },
    { /* LTI coeff */
        "lti_b0", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_b1", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_b2", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_b3", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_b4", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_b5", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_a1", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_a2", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_a3", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_a4", 0, 1, 0, -1e9, 1e9 },
    { /* LTI coeff */
        "lti_a5", 0, 1, 0, -1e9, 1e9 },	
	
    { /* Must be last! */
        NULL, 0.0, -1, -1, 0.0, 0.0 }
};

/* params initialized */
static int params_init = 0;

rp_calib_params_t rp_main_calib_params;

const char *rp_app_desc(void)
{
    return (const char *)"Red Pitaya LTI Workbench application.\n";
}

int rp_app_init(void)
{
    fprintf(stderr, "Loading LTI version %s-%s.\n", VERSION_STR, REVISION_STR);

    
    rp_default_calib_params(&rp_main_calib_params);
    if(rp_read_calib_params(&rp_main_calib_params) < 0) {
        fprintf(stderr, "rp_read_calib_params() failed, using default"
                " parameters\n");
    }    
    
    if(rp_lti_worker_init() < 0) {
        return -1;
    }
    if(generate_init(&rp_main_calib_params) < 0) {
        return -1;
    }
    
    rp_set_params(&rp_main_params[0], PARAMS_NUM);

    rp_lti_worker_change_state(rp_lti_auto_state);

    return 0;
}

int rp_app_exit(void)
{
    fprintf(stderr, "Unloading LTI version %s-%s.\n", VERSION_STR, REVISION_STR);

    rp_lti_worker_exit();
    generate_exit();
    
    return 0;
}

int rp_set_params(rp_app_params_t *p, int len)
{
    int i;
    int fpga_update = 1;
    int params_change = 0;

 
    
    if(len > PARAMS_NUM) {
        fprintf(stderr, "Too much parameters, max=%d\n", PARAMS_NUM);
        return -1;
    }

    for(i = 0; i < len; i++) {
        int p_idx = -1;
        int j = 0;

        /* Search for correct parameter name in defined parameters */
        while(rp_main_params[j].name != NULL) {
            int p_strlen = strlen(p[i].name);

            if(p_strlen != strlen(rp_main_params[j].name)) {
                j++;
                continue;
            }
            if(!strncmp(p[i].name, rp_main_params[j].name, p_strlen)) {
                p_idx = j;
                break;
            }
            j++;
        }

        if(p_idx == -1) {
            fprintf(stderr, "Parameter %s not found, ignoring it\n", p[i].name);
            continue;
        }

        if(rp_main_params[p_idx].read_only) {
            continue;
        }

        if(rp_main_params[p_idx].value != p[i].value) {
            params_change = 1;
            if(rp_main_params[p_idx].fpga_update)
                fpga_update = 1;
        }
        if(rp_main_params[p_idx].min_val > p[i].value) {
            fprintf(stderr, "Incorrect parameters value: %f (min:%f), "
                    " correcting it\n", p[i].value, 
                    rp_main_params[p_idx].min_val);
            p[i].value = rp_main_params[p_idx].min_val;
        } else if(rp_main_params[p_idx].max_val < p[i].value) {
            fprintf(stderr, "Incorrect parameters value: %f (max:%f), "
                    " correcting it\n", p[i].value, 
                    rp_main_params[p_idx].max_val);
            p[i].value = rp_main_params[p_idx].max_val;
        }

        rp_main_params[p_idx].value = p[i].value;
    }

    if(params_change || (params_init == 0)) {
        params_init = 1;

        rp_main_params[FREQ_UNIT_PARAM].value =             
            lti_fpga_cnv_freq_range_to_unit(
                                         rp_main_params[FREQ_RANGE_PARAM].value);

        rp_lti_worker_update_params((rp_app_params_t *)&rp_main_params[0],
                                       fpga_update);
    }

    return 0;
}

/* Returned vector must be free'd externally! */
int rp_get_params(rp_app_params_t **p)
{
    rp_app_params_t *p_copy = NULL;
    
    int i;

    p_copy = (rp_app_params_t *)malloc((PARAMS_NUM+1) * sizeof(rp_app_params_t));
    if(p_copy == NULL)
        return -1;

    for(i = 0; i < PARAMS_NUM; i++) {
        int p_strlen = strlen(rp_main_params[i].name);
        p_copy[i].name = (char *)malloc(p_strlen+1);
        strncpy((char *)&p_copy[i].name[0], &rp_main_params[i].name[0], 
                p_strlen);
        p_copy[i].name[p_strlen]='\0';

        p_copy[i].value       = rp_main_params[i].value;
        p_copy[i].fpga_update = rp_main_params[i].fpga_update;
        p_copy[i].read_only   = rp_main_params[i].read_only;
        p_copy[i].min_val     = rp_main_params[i].min_val;
        p_copy[i].max_val     = rp_main_params[i].max_val;
    }
    p_copy[PARAMS_NUM].name = NULL;

    *p = p_copy;
    return PARAMS_NUM;
}

int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    int ret_val;

 

    if(*s == NULL)
        return -1;

    *sig_num = LTI_OUT_SIG_NUM;
    *sig_len = LTI_OUT_SIG_LEN;

    ret_val = rp_lti_get_signals(s);

    /* Old signal */
    if(ret_val < 0) {
        return -1;
    }




    return 0;
}

// This function enables direct signal generator FPGA register access from worker.c

void dir_gen_set(int ch, int param, int value)
{ 
  dir_gen_fpga_set(ch, param, value); 
}





int rp_create_signals(float ***a_signals)
{
    int i;
    float **s;

    s = (float **)malloc(LTI_OUT_SIG_NUM * sizeof(float *));
    if(s == NULL) {
        return -1;
    }
    for(i = 0; i < LTI_OUT_SIG_NUM; i++)
        s[i] = NULL;

    for(i = 0; i < LTI_OUT_SIG_NUM; i++) {
        s[i] = (float *)malloc(LTI_OUT_SIG_LEN * sizeof(float));
        if(s[i] == NULL) {
            rp_cleanup_signals(a_signals);
            return -1;
        }
        memset(&s[i][0], 0, LTI_OUT_SIG_LEN * sizeof(float));
    }
    *a_signals = s;

    return 0;
}

void rp_cleanup_signals(float ***a_signals)
{
    int i;
    float **s = *a_signals;

    if(s) {
        for(i = 0; i < LTI_OUT_SIG_NUM; i++) {
            if(s[i]) {
                free(s[i]);
                s[i] = NULL;
            }
        }
        free(s);
        *a_signals = NULL;
    }
}
