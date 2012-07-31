/* arch/arm/mach-msm/lge/lge_ats_atcmd.c
 *
 * Copyright (C) 2010 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/msm_rpcrouter.h>
#include <mach/board_lge.h>
#include "lge_ats.h"
#include "lge_ats_cmd.h"

#include <linux/syscalls.h>
#include <linux/fcntl.h>
//LG_BTUI_NV : Get_BDADDR - kukdong.bae@lge.com
#include <linux/lge_alohag_at.h>
#include <linux/delay.h>
//LG_BTUI_NV

void wirte_flight_mode(int mode)
{
	char buf[10];

	int fd = sys_open("/sys/devices/platform/autoall/flight", O_WRONLY, 0);
	
	if (fd == -1) {
        //	return -1;
    	}
	else {
		sprintf(buf, "%d", mode);

		sys_write(fd, buf, strlen(buf));

		sys_close(fd);
	}

}

int lge_ats_handle_atcmd(struct msm_rpc_server *server,
						 struct rpc_request_hdr *req, unsigned len,
						 void (*update_atcmd_state)(char *cmd, int state) )
{
	struct rpc_ats_atcmd_args *args = (struct rpc_ats_atcmd_args *)(req + 1);
	int result = HANDLE_OK;
	char ret_string[MAX_STRING_RET];
	uint32_t ret_value1 =0;
	uint32_t ret_value2 = 0;
	uint32_t at_cmd;
	uint32_t at_act;
	uint32_t at_param;

#ifdef CONFIG_LGE_BROADCAST	
	/* mtv_blt_mode : 1 mtv_av_mode : 0*/
	static int mtv_blt_mode = 0; 
	struct broadcast_mtv_sig_info tdmbdata;
#endif
	at_cmd = be32_to_cpu(args->at_cmd);
	at_act = be32_to_cpu(args->at_act);
	at_param = be32_to_cpu(args->at_param);
	printk(KERN_INFO "%s: at_cmd = %d, at_act=%d, at_param=%d:\n",
		   __func__, args->at_cmd, args->at_act,args->at_param);

	memset(server->retvalue.ret_string, 0, sizeof(server->retvalue.ret_string));

	memset (ret_string, 0, sizeof(ret_string));

	switch (at_cmd)
	{
	case ATCMD_ACS:	//31
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("acs", at_param); //state is up? down?
		break;
	case ATCMD_VLC:	//36
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("vlc", at_param); //state is up? down?
		break;
	case ATCMD_SPM:	//40
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("spm", at_param); //state is up? down?
		break;
	case ATCMD_MPT:	//43
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("mpt", at_param); //state is up? down?
		break;
	case ATCMD_FMR: //42
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("fmr", at_param); //state is up? down?
		break;
		/*[LGE_CHANGE_S][Camera][kwangsoo.park@lge.com 2010-05-26 : AT Command AT_CAM AT_AVR */
	case ATCMD_AVR:	//45
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("avr", at_param); //state is up? down?
		break;
	case ATCMD_CAM: //70
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		update_atcmd_state("cam", at_param); //state is up? down?
		break;
		/*[LGE_CHANGE_E[Camera][kwangsoo.park@lge.com 2010-05-26 : AT Command AT_CAM AT_AVR */

	case ATCMD_EMT:  // 46
		ret_value1 = external_memory_test();
		break;

	case ATCMD_FC:  // 59
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		ret_value1 = 0;
		break;

	case ATCMD_FO:  // 60
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		ret_value1 = 0;
		break;

	//LGE_UPDATE_S ins.lee@lge.com 2010-06-21, add AT%FLIHGT
	case ATCMD_FLIGHT:  // 82
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;

		wirte_flight_mode(at_param);	
		update_atcmd_state("flight", at_param); //state is up? down?
		break;
    //LGE_UPDATE_E ins.lee@lge.com 2010-06-21, add AT%FLIHGT

	case ATCMD_MMCFORMAT:  // 129
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		//LGE_UPDATE_S FS 2011-03-22
		if(!external_memory_test())
			ret_value1 = 0;
		else
		{
			update_atcmd_state("mmcformat", 0);
			update_atcmd_state("mmcformat", 1);
			update_atcmd_state("mmcformat", 9);
			ret_value1 = 1;
		}
		//LGE_UPDATE_E FS 2011-03-22
		break;

//LG_BTUI_NV : Get_BDADDR - kukdong.bae@lge.com
	case ATCMD_BTAD: //52
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		printk(KERN_INFO "[BTUI] %s: at_cmd => ATCMD_BTAD\n", __func__);
		atcmd_get_dev()->state = 1;
		update_atcmd_state("btad", at_param);
		break;
//LG_BTUI_NV

//LG_BTUI_DUT : use brcm_patchram_plus [S]
	case ATCMD_BTTM: //53
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		printk(KERN_INFO "[BTUI] %s: at_cmd => ATCMD_BTTM\n", __func__);
		update_atcmd_state("bttm", at_param);
		if(at_param==3)
			msleep(1000); // 1sec delay
		else
			msleep(5000); // 5sec delay
		break;
//LG_BTUI_DUT : use brcm_patchram_plus [E]	
#ifdef CONFIG_LGE_BROADCAST	
	case ATCMD_MTV:	//150
		if(at_act != ATCMD_ACTION)
			result = HANDLE_FAIL;
		printk(KERN_INFO "[MTV] %s: at_cmd => ATCMD_MTV\n", __func__);
		if(at_param == 71)
		{
			printk(KERN_INFO "[MTV] AV Test On process\n");
			update_atcmd_state("mtv", at_param);
			mtv_blt_mode = 0;
		}
		else if(at_param == 1)
		{	
			if(mtv_blt_mode == 1)
			{
				printk(KERN_INFO "[MTV] BLT Off process\n");
				mtv_blt_mode = 0;
				broadcast_tdmb_blt_close();
				broadcast_tdmb_blt_power_off();
			}
			else
			{
				printk(KERN_INFO "[MTV] AV Test Off process\n");
				update_atcmd_state("mtv", at_param);
			}
		}
		else
		{
			if((at_param > 71) &&(at_param <= 133))
			{
				printk(KERN_INFO "[MTV] BLT On\n");
				broadcast_tdmb_blt_power_on();
				broadcast_tdmb_blt_open();
				
				broadcast_tdmb_blt_tune_set_ch(at_param);

				mtv_blt_mode = 1;
				
			}
			else if((at_param == 2)||(at_param == 3)
				|| (at_param == 4)||(at_param == 5))
			{
				broadcast_tdmb_blt_get_sig_info((void*)&tdmbdata);
				ret_value1 = tdmbdata.msc_ber;
				ret_value2 = tdmbdata.tp_err_cnt;
				printk(KERN_INFO "[MTV] BLT get ber ber = %d, tp_err_cnt = %d\n", ret_value1, ret_value2);
			}
			else
			{
				printk(KERN_INFO "[MTV] BLT Error range\n");
				//result = HANDLE_ERROR;
			}
		}
		break;
#endif
	default :
		result = HANDLE_ERROR;
		break;
	}

	/* give to RPC server result */
	strncpy(server->retvalue.ret_string, ret_string, MAX_STRING_RET);
	server->retvalue.ret_string[MAX_STRING_RET-1] = 0;
	server->retvalue.ret_value1 = ret_value1;
	server->retvalue.ret_value2 = ret_value2;
	/////////////////////////////////////////////////////////////////
	
	if(result == HANDLE_OK)

		result = RPC_RETURN_RESULT_OK;
	else
		result = RPC_RETURN_RESULT_ERROR;

	return result;
}
