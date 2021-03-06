/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/mnfgtools/prdfMfgThresholdFile.C $          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2009,2014              */
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

#include <prdfMfgThresholdFile.H>
#include <prdfGlobal.H>
#include <prdfAssert.h>
#include <prdfMfgSync.H>
#include <prdfErrlUtil.H>
#include <prdfPlatServices.H>
#include <prdfTrace.H>

namespace PRDF
{

void MfgThresholdFile::setup()
{
    syncFromFsp();
}

void MfgThresholdFile::syncFromFsp()
{
    #define FUNC "[MfgThresholdFile::syncFromFsp]"
    PRDF_ENTER(FUNC);

    do
    {
        if ( !PlatServices::mfgMode() )
        {
            PRDF_TRAC(" no-op since not in MFG mode");
            break;
        }

        errlHndl_t l_err = getMfgSync().syncMfgThresholdFromFsp();
        if (l_err)
        {
            PRDF_ERR(FUNC" failed to sync from the FSP");
            PRDF_COMMIT_ERRL(l_err, ERRL_ACTION_REPORT);
            break;
        }

    } while(0);

    PRDF_EXIT(FUNC);
    #undef FUNC
}

void MfgThresholdFile::packThresholdDataIntoBuffer(
                             uint8_t* & o_buffer,
                             uint32_t i_sizeOfBuf)
{
    #define FUNC "[MfgThresholdFile::packThresholdDataIntoBuffer]"

    PRDF_ERR(FUNC" not used in hostboot");

    #undef FUNC
}


} // end namespace PRDF
