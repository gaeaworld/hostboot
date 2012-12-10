/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/common/framework/resolution/prdfCaptureResolution.H $ */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2006,2012              */
/*                                                                        */
/* p1                                                                     */
/*                                                                        */
/* Object Code Only (OCO) source materials                                */
/* Licensed Internal Code Source Materials                                */
/* IBM HostBoot Licensed Internal Code                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* Origin: 30                                                             */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

#include <iipResolution.h>
#include <prdfExtensibleChip.H>

namespace PRDF
{

/**
 * @class CaptureResolution
 * @brief Adds a capture-group to the capture data when hitting an error.
 */
class CaptureResolution : public Resolution
{
    public:
        CaptureResolution() : iv_chip(NULL), iv_captureGroup(0) {};

        /** @fn CaptureResolution
         *  @brief Typical constructor.
         *
         *  @param i_chip - Extensible chip object to capture from.
         *  @param i_group - Capture-group to gather.
         */
        CaptureResolution(ExtensibleChip * i_chip,
                              uint32_t i_captureGroup)
                                    : iv_chip(i_chip),
                                      iv_captureGroup(i_captureGroup) {};

        /** @fn Resolve
         *  @brief Perform capture.
         */
        virtual int32_t Resolve(STEP_CODE_DATA_STRUCT & i_error);

        /**
         *  Comparison operator to aid in ResolutionFactory.
         */
        bool operator==(const CaptureResolution & i_rhs)
                { return ((iv_chip == i_rhs.iv_chip) &&
                          (iv_captureGroup == i_rhs.iv_captureGroup));
                };
    private:

            /** The chip to capture from */
        ExtensibleChip * iv_chip;
            /** The group id to capture */
        uint32_t iv_captureGroup;

};

} // end namespace PRDF
