/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/mc_config/mss_eff_config/mss_throttle_to_power.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2015                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
// $Id: mss_throttle_to_power.C,v 1.18 2014/11/06 21:07:04 pardeik Exp $
// $Source: /afs/awd/projects/eclipz/KnowledgeBase/.cvsroot/eclipz/chips/
//          centaur/working/procedures/ipl/fapi/mss_throttle_to_power.C,v $
//------------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2011
// *! All Rights Reserved -- Property of IBM
// *! ***  ***
//------------------------------------------------------------------------------
// *! TITLE       : mss_throttle_to_power
// *! DESCRIPTION : see additional comments below
// *! OWNER NAME  : Michael Pardeik   Email: pardeik@us.ibm.com
// *! BACKUP NAME : Jacob Sloat       Email: jdsloat@us.ibm.com
// *! ADDITIONAL COMMENTS :
//
// applicable CQ component memory_screen
//
// DESCRIPTION:
// The purpose of this procedure is to set the power attributes for each dimm
// and channel pair
//
//------------------------------------------------------------------------------
// Don't forget to create CVS comments when you check in your changes!
//------------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version:|  Author: |  Date:  | Comment:
//---------|----------|---------|-----------------------------------------------
//   1.18  | pardeik  |06-NOV-14| removed string in trace statement
//         |          |         | changed FAPI_IMP to FAPI_INF
//   1.17  | pardeik  |16-OCT-14| removed l_dimm_power_array_integer to fix
//         |          |         |  cronus 64bit compile error
//   1.16  | pardeik  |15-OCT-14| remove attr writing for ATTR_MSS_DIMM_MAXPOWER
//   1.15  | pardeik  |27-AUG-14| use new power curve uplift attribute for idle 
//   1.14  | pardeik  |21-MAY-14| Fixed power calculations
//   1.13  | jdsloat  |10-MAR-14| Edited comments
//   1.12  | pardeik  |06-JAN-14| added dimm power curve uplift from MRW
//         |          |         | use max utiliation from MRW for MAX_UTIL
//   1.11  | pardeik  |13-NOV-13| changed MAX_UTIL from 75 to 56.25
//   1.10  | bellows  |19-SEP-13| fixed possible buffer overrun found by stradale
//   1.9   | pardeik  |04-DEC-12| update lines to have a max width of 80 chars
//         |          |         | added FAPI_ERR before return code lines
//         |          |         | made trace statements for procedures FAPI_IMP
//   1.8   | pardeik  |25-OCT-12| updated FAPI_ERR sections, added CQ component
//         |          |         | comment line
//   1.7   | pardeik  |19-OCT-12| use ATTR_MSS_CHANNEL_PAIR_MAXPOWER instead of
//         |          |         | ATTR_MSS_CHANNEL_MAXPOWER
//         | pardeik  |19-OCT-12| multiple throttle N values by 4 to get dram
//         |          |         | utilization
//   1.6   | pardeik  |11-OCT-12| updated to use new throttle attributes, made
//         |          |         | function mss_throttle_to_power_calc
//   1.5   | pardeik  |10-APR-12| power calculation updates and fixes
//   1.5   | pardeik  |10-APR-12| power calculation updates and fixes
//   1.4   | pardeik  |04-APR-12| moved cdimm power calculation to end of
//         |          |         | section instead of having it in multiple
//         |          |         | places
//   1.3   | pardeik  |04-APR-12| use else if instead of if after checking
//         |          |         | throttle denominator to zero
//   1.2   | pardeik  |03-APR-12| use mba target intead of mbs, added cdimm
//         |          |         | power calculation for half of cdimm
//   1.1   | pardeik  |01-APR-11| Updated to use attributes and fapi functions
//         |          |         | to loop through ports/dimms
//         | pardeik  |01-DEC-11| First Draft.


//------------------------------------------------------------------------------
//  My Includes
//------------------------------------------------------------------------------
#include <mss_throttle_to_power.H>

//------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------
#include <fapi.H>


extern "C" {

    using namespace fapi;


//------------------------------------------------------------------------------
// Funtions in this file
//------------------------------------------------------------------------------
    fapi::ReturnCode mss_throttle_to_power(const fapi::Target & i_target_mba);

    fapi::ReturnCode mss_throttle_to_power_calc
      (
       const fapi::Target &i_target_mba,
       uint32_t i_throttle_n_per_mba,
       uint32_t i_throttle_n_per_chip,
       uint32_t i_throttle_d,
       float &channel_pair_power
       );


//------------------------------------------------------------------------------
// @brief mss_throttle_to_power(): This function will get the throttle
// attributes and call another function to determine the dimm and channel pair
// power based on those throttles
//
// @param[in]	const fapi::Target &i_target_mba:  MBA Target
//
// @return fapi::ReturnCode
//------------------------------------------------------------------------------

    fapi::ReturnCode mss_throttle_to_power(const fapi::Target & i_target_mba)
    {
	fapi::ReturnCode rc = fapi::FAPI_RC_SUCCESS;

	FAPI_INF("*** Running mss_throttle_to_power ***");

	uint32_t throttle_n_per_mba;
	uint32_t throttle_n_per_chip;
	uint32_t throttle_d;
	float channel_pair_power;

// Get input attributes
	rc = FAPI_ATTR_GET(ATTR_MSS_MEM_THROTTLE_NUMERATOR_PER_MBA,
			   &i_target_mba, throttle_n_per_mba);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MSS_MEM_THROTTLE_NUMERATOR_PER_MBA");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MSS_MEM_THROTTLE_NUMERATOR_PER_CHIP,
			   &i_target_mba, throttle_n_per_chip);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MSS_MEM_THROTTLE_NUMERATOR_PER_CHIP");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MSS_MEM_THROTTLE_DENOMINATOR,
			   &i_target_mba, throttle_d);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MSS_MEM_THROTTLE_DENOMINATOR");
	    return rc;
	}

// Call function mss_throttle_to_power_calc
	rc = mss_throttle_to_power_calc(
					i_target_mba,
					throttle_n_per_mba,
					throttle_n_per_chip,
					throttle_d,
					channel_pair_power
					);
	if (rc)
	{
	    FAPI_ERR("Error (0x%x) calling mss_throttle_to_power_calc", static_cast<uint32_t>(rc));
	    return rc;
	}

	FAPI_INF("*** mss_throttle_to_power COMPLETE ***");
	return rc;

    }



//------------------------------------------------------------------------------
// @brief mss_throttle_to_power_calc(): This function will calculate the dimm
// and channel pair power and update attributes with the power values
//
// @param[in]   const fapi::Target &i_target_mba:  MBA Target
// @param[in]   uint32_t i_throttle_n_per_mba:  Throttle value for
//              cfg_nm_n_per_mba
// @param[in]   uint32_t i_throttle_n_per_chip:  Throttle value for
//              cfg_nm_n_per_chip
// @param[in]	uint32_t i_throttle_d:  Throttle value for cfg_nm_m
// @param[out]  float &o_channel_pair_power:  channel pair power at these
//              throttle settings
//
// @return fapi::ReturnCode
//------------------------------------------------------------------------------

    fapi::ReturnCode mss_throttle_to_power_calc
      (
       const fapi::Target &i_target_mba,
       uint32_t i_throttle_n_per_mba,
       uint32_t i_throttle_n_per_chip,
       uint32_t i_throttle_d,
       float &o_channel_pair_power
       )
    {
	fapi::ReturnCode rc = fapi::FAPI_RC_SUCCESS;

	FAPI_INF("*** Running mss_throttle_to_power_calc ***");

	const uint8_t MAX_NUM_PORTS = 2;
	const uint8_t MAX_NUM_DIMMS = 2;

	uint32_t l_power_slope_array[MAX_NUM_PORTS][MAX_NUM_DIMMS];
	uint32_t l_power_int_array[MAX_NUM_PORTS][MAX_NUM_DIMMS];
	uint8_t l_dimm_ranks_array[MAX_NUM_PORTS][MAX_NUM_DIMMS];
	uint8_t l_port;
	uint8_t l_dimm;
	float l_dimm_power_array[MAX_NUM_PORTS][MAX_NUM_DIMMS];
	float l_utilization;
	float l_channel_power_array[MAX_NUM_PORTS];
	uint32_t l_channel_power_array_integer[MAX_NUM_PORTS];
	uint32_t l_channel_pair_power_integer;
	float l_uplift;
	uint8_t l_power_curve_percent_uplift;
	uint8_t l_power_curve_percent_uplift_idle;
	uint32_t l_max_dram_databus_util;
	uint8_t num_mba_with_dimms;
	uint8_t custom_dimm;
	fapi::Target target_chip;
	std::vector<fapi::Target> target_mba_array;
	std::vector<fapi::Target> target_dimm_array;
	uint8_t mba_index;
	uint8_t l_utilization_idle = 0;


// get input attributes
	rc = FAPI_ATTR_GET(ATTR_MRW_MAX_DRAM_DATABUS_UTIL,
			   NULL, l_max_dram_databus_util);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MRW_MAX_DRAM_DATABUS_UTIL");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MRW_DIMM_POWER_CURVE_PERCENT_UPLIFT,
			   NULL, l_power_curve_percent_uplift);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MRW_DIMM_POWER_CURVE_PERCENT_UPLIFT");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MRW_DIMM_POWER_CURVE_PERCENT_UPLIFT_IDLE,
			   NULL, l_power_curve_percent_uplift_idle);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MRW_DIMM_POWER_CURVE_PERCENT_UPLIFT_IDLE");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MSS_POWER_SLOPE,
			   &i_target_mba, l_power_slope_array);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MSS_POWER_SLOPE");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_MSS_POWER_INT,
			   &i_target_mba, l_power_int_array);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_MSS_POWER_INT");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_EFF_NUM_RANKS_PER_DIMM,
			   &i_target_mba, l_dimm_ranks_array);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_EFF_NUM_RANKS_PER_DIMM");
	    return rc;
	}
	rc = FAPI_ATTR_GET(ATTR_EFF_CUSTOM_DIMM, &i_target_mba, custom_dimm);
	if (rc) {
	    FAPI_ERR("Error getting attribute ATTR_EFF_CUSTOM_DIMM");
	    return rc;
	}

// Maximum theoretical data bus utilization (percent of max) (for ceiling)
// Comes from MRW value in c% - convert to %
	float MAX_UTIL = (float) l_max_dram_databus_util / 100;

// get number of mba's  with dimms for a CDIMM
	if (custom_dimm == fapi::ENUM_ATTR_EFF_CUSTOM_DIMM_YES)
	{
// Get Centaur target for the given MBA
	    rc = fapiGetParentChip(i_target_mba, target_chip);
	    if (rc) {
		FAPI_ERR("Error calling fapiGetParentChip");
		return rc;
	    }
// Get MBA targets from the parent chip centaur
	    rc = fapiGetChildChiplets(target_chip,
				      fapi::TARGET_TYPE_MBA_CHIPLET,
				      target_mba_array,
				      fapi::TARGET_STATE_PRESENT);
	    if (rc) {
		FAPI_ERR("Error calling fapiGetChildChiplets");
		return rc;
	    }
	    num_mba_with_dimms = 0;
	    for (mba_index=0; mba_index < target_mba_array.size(); mba_index++)
	    {
		rc = fapiGetAssociatedDimms(target_mba_array[mba_index],
					    target_dimm_array,
					    fapi::TARGET_STATE_PRESENT);
		if (rc) {
		    FAPI_ERR("Error calling fapiGetAssociatedDimms");
		    return rc;
		}
		if (target_dimm_array.size() > 0)
		{
		    num_mba_with_dimms++;
		}
	    }

	}
	else
	{
// ISDIMMs, set to a value of one since they are handled on a per MBA basis
	    num_mba_with_dimms = 1;
	}

// add up the power from all dimms for this MBA (across both channels) using the
// throttle values
	o_channel_pair_power = 0;
	l_channel_pair_power_integer = 0;
	for (l_port = 0; l_port < MAX_NUM_PORTS; l_port++)
	{
	    l_channel_power_array[l_port] = 0;
	    l_channel_power_array_integer[l_port] = 0;
	    for (l_dimm=0; l_dimm < MAX_NUM_DIMMS; l_dimm++)
	    {
// default dimm power is zero (used for dimms that are not physically present)
		l_dimm_power_array[l_port][l_dimm] = 0;
		l_utilization = 0;
// See if there are any ranks present on the dimm (configured or deconfigured)
		if (l_dimm_ranks_array[l_port][l_dimm] > 0)
		{
// N/M throttling has the dimm0 and dimm1 throttles the same for DIMM level
// throttling, which we plan to use
// MBA or chip level throttling could limit the commands to a dimm (used along
// with the dimm level throttling)
// If MBA/chip throttle is less than dimm throttle, then use MBA/chip throttle
// If MBA/chip throttle is greater than dimm throttle, then use the dimm
// throttle
// If either of these are above the MAX_UTIL, then use MAX_UTIL
// Get power from each dimm here
// Note that the MAX_UTIL effectively is the percent of maximum bandwidth for
// that dimm

		    if (i_throttle_d == 0)
		    {
// throttle denominator is zero (N/M throttling disabled), set dimm power to the
// maximum
			FAPI_DBG("N/M Throttling is disabled (M=0).  Use Max DIMM Power");
			l_dimm_power_array[l_port][l_dimm] =
			  (l_power_slope_array[l_port][l_dimm] *
			   ((float)MAX_UTIL / 100) +
			   l_power_int_array[l_port][l_dimm]);
			l_utilization = (float)MAX_UTIL;
		    }
		    else if (
			     (
			      ((float)i_throttle_n_per_mba * 100 * 4) /
			      i_throttle_d)
			     >
			     (((float)i_throttle_n_per_chip / num_mba_with_dimms * 100 * 4) /
			      i_throttle_d)
			     )
		    {
// limited by the mba/chip throttles (ie.  cfg_nm_n_per_chip)
			if ((((float)i_throttle_n_per_chip / num_mba_with_dimms * 100 * 4) /
			     i_throttle_d) > MAX_UTIL)
			{
// limited by the maximum utilization
			    l_dimm_power_array[l_port][l_dimm] =
			      (l_power_slope_array[l_port][l_dimm] *
			       ((float)MAX_UTIL / 100) +
			       l_power_int_array[l_port][l_dimm]);
			    l_utilization = (float)MAX_UTIL;
			}
			else
			{
// limited by the per chip throttles
			    l_dimm_power_array[l_port][l_dimm] =
			      (l_power_slope_array[l_port][l_dimm] *
			       (((float)i_throttle_n_per_chip / num_mba_with_dimms * 4)
				/ i_throttle_d) +
			       l_power_int_array[l_port][l_dimm]);
			    l_utilization = (((float)i_throttle_n_per_chip / num_mba_with_dimms *
					      100 * 4) / i_throttle_d);
			}
		    }
		    else
		    {
// limited by the per mba throttles (ie.  cfg_nm_n_per_mba)
			if ((((float)i_throttle_n_per_mba * 100 * 4) /
			     i_throttle_d) > MAX_UTIL)
			{
// limited by the maximum utilization
			    l_dimm_power_array[l_port][l_dimm] =
			      (l_power_slope_array[l_port][l_dimm] *
			       ((float)MAX_UTIL / 100) +
			       l_power_int_array[l_port][l_dimm]);
			    l_utilization = (float)MAX_UTIL;
			}
			else
			{
// limited by the per mba throttles
// multiply by number of dimms on port since other dimm has same throttle value
			    l_dimm_power_array[l_port][l_dimm] =
			      (l_power_slope_array[l_port][l_dimm] *
			       (((float)i_throttle_n_per_mba * 4) /
				i_throttle_d) +
			       l_power_int_array[l_port][l_dimm]);
			    l_utilization =
			      (((float)i_throttle_n_per_mba * 100 * 4) /
			       i_throttle_d);
			}
		    }
		}
// Get dimm power in integer format (add on 1 since value will get truncated)
// Include any system uplift here too
		l_uplift = ( (l_power_curve_percent_uplift - l_power_curve_percent_uplift_idle) / (MAX_UTIL - l_utilization_idle) ) * l_utilization + l_power_curve_percent_uplift_idle;
		if (l_dimm_power_array[l_port][l_dimm] > 0)
		{
		    l_dimm_power_array[l_port][l_dimm] =
		      l_dimm_power_array[l_port][l_dimm]
		      * (1 + l_uplift / 100);
		}
// calculate channel power by adding up the power of each dimm
		l_channel_power_array[l_port] = l_channel_power_array[l_port] +
		  l_dimm_power_array[l_port][l_dimm];
		FAPI_DBG("[P%d:D%d][CH Util %4.2f/%4.2f][Slope:Int %d:%d][UpliftPercent %4.2f (min/max %d/%d)][Power %4.2f cW]", l_port, l_dimm, l_utilization, MAX_UTIL, l_power_slope_array[l_port][l_dimm], l_power_int_array[l_port][l_dimm], l_uplift, l_power_curve_percent_uplift_idle, l_power_curve_percent_uplift, l_dimm_power_array[l_port][l_dimm]);
	    }
	    FAPI_DBG("[P%d][Power %4.2f cW]", l_port, l_channel_power_array[l_port]);
	}
// get the channel pair power for this MBA (add on 1 since value will get
// truncated)
	for (l_port = 0; l_port < MAX_NUM_PORTS; l_port++)
	{
	    o_channel_pair_power = o_channel_pair_power +
	      l_channel_power_array[l_port];
	    if (l_channel_power_array_integer[l_port] > 0)
	    {
		l_channel_power_array_integer[l_port] =
		  (int)l_channel_power_array[l_port] + 1;
	    }
	}
	FAPI_DBG("Channel Pair Power %4.2f cW]", o_channel_pair_power);

	if (o_channel_pair_power > 0)
	{
	    l_channel_pair_power_integer = (int)o_channel_pair_power + 1;
	}
//------------------------------------------------------------------------------
// Update output attributes

// Removed updating ATTR_MSS_DIMM_MAXPOWER for SW282712

	rc = FAPI_ATTR_SET(ATTR_MSS_CHANNEL_PAIR_MAXPOWER,
			   &i_target_mba, l_channel_pair_power_integer);
	if (rc) {
	    FAPI_ERR("Error writing attribute ATTR_MSS_CHANNEL_PAIR_MAXPOWER");
	    return rc;
	}

	FAPI_INF("*** mss_throttle_to_power_calc COMPLETE ***");
	return rc;
    }


} //end extern C
