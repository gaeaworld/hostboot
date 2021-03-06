/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/hwpf/fapi/fapiTarget.H $                      */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2011,2014              */
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
// $Id: fapiTarget.H,v 1.9 2014/02/26 14:51:05 mjjones Exp $
// $Source: /afs/awd/projects/eclipz/KnowledgeBase/.cvsroot/eclipz/hwpf/working/fapi/fapiTarget.H,v $

/**
 *  @file fapiTarget.H
 *
 *  @brief Defines the Target class that is a generic target of a Hardware
 *         Procedure operation.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created. Based on Hlava prototype
 *                          mjjones     06/29/2011  Removed incorrect MSB from 2 enums
 *                          mjjones     07/05/2011  Removed const from handle
 *                          mjjones     08/29/2011  Updated toString function
 *                          mjjones     09/12/2011  Added isChip and isChiplet
 *                          mjjones     02/07/2012  Remove MBS_CHIPLET
 *                                                  Add XBUS_ENDPOINT ABUS_ENDPOINT
 *                          mjjones     02/21/2012  Add high performance toEcmdString
 *                          mjjones     02/24/2014  Add isChip/Chiplet using types
 *                                                  Add isPhysParentChild 
 */

#ifndef FAPITARGET_H_
#define FAPITARGET_H_

#include <stdint.h>
#include <stddef.h>

namespace fapi
{

/**
 * @brief Enumeration of target types values (bitmask values)
 */
enum TargetType
{
    TARGET_TYPE_NONE          = 0x00000000,
    TARGET_TYPE_SYSTEM        = 0x00000001,
    TARGET_TYPE_DIMM          = 0x00000002,
    TARGET_TYPE_PROC_CHIP     = 0x00000004,
    TARGET_TYPE_MEMBUF_CHIP   = 0x00000008,
    TARGET_TYPE_EX_CHIPLET    = 0x00000010,
    TARGET_TYPE_MBA_CHIPLET   = 0x00000020,
    TARGET_TYPE_MCS_CHIPLET   = 0x00000040,
    TARGET_TYPE_XBUS_ENDPOINT = 0x00000080,
    TARGET_TYPE_ABUS_ENDPOINT = 0x00000100,
    TARGET_TYPE_L4            = 0x00000200,
};

/**
 * @brief Typedef used when passing multiple TargetType values
 */
typedef uint32_t TargetTypes_t;

/**
 * @brief Enumeration of target state values (bitmask values)
 */
enum TargetState
{
    TARGET_STATE_PRESENT    = 0x00000001,
    TARGET_STATE_FUNCTIONAL = 0x00000002,
};

/**
 * @brief Typedef used when passing multiple TargetState values
 */
typedef uint32_t TargetStates_t;

/**
 * @brief ECMD constants
 */
const uint32_t MAX_ECMD_STRING_LEN = 64;

/**
 * @class Target
 *
 * This class provides a generic Target of a Hardware Procedure Operation.
 *
 * A Target contains a void * pointer to a handle which is only meaningful to
 * platform code.
 *
 * A Target object is copyable and assignable. Therefore, it cannot be
 * subclassed.
 *
 * A Target object is not thread safe, multiple threads must not use the same
 * Target object concurrently.
 */
class Target
{
public:

    /**
     * @brief Default constructor
     */
    Target();

    /**
     * @brief Constructor
     *
     * @param[in] i_type    Target type
     * @param[in] i_pHandle Pointer to platform specific Target handle
     */
    Target(const TargetType i_type,
           void * i_pHandle);

    /**
     * @brief Copy Constructor
     *
     * @param[in] i_right Reference to Target to copy
     */
    Target(const Target & i_right);

    /**
     * @brief Destructor
     */
    ~Target();

    /**
     * @brief Assignment Operator.
     *
     * @param[in] i_right Reference to Target to assign from.
     *
     * @return Reference to 'this' Target
     */
    Target & operator=(const Target & i_right);

    /**
     * @brief Equality Comparison Operator
     *
     * @param[in] i_right Reference to Target to compare.
     *
     * @return bool. True if equal.
     */
    bool operator==(const Target & i_right) const;

    /**
     * @brief Inequality Comparison Operator
     *
     * @param[in] i_right Reference to Target to compare.
     *
     * @return bool. True if not equal.
     */
    bool operator!=(const Target & i_right) const;

    /**
     * @brief Get the handle pointer.
     *
     * The handle is only meaningful to platform code.
     *
     * @return Handle_t. The handle.
     */
    void * get() const { return iv_pHandle; }

    /**
     * @brief Set the handle. Platform using Handle_t as handle
     *
     * The handle is only meaningful to platform code.
     *
     * @param[in] i_pHandle Pointer to platform specific handle
     */
    void set(void * i_pHandle);

    /**
     * @brief Get the target type
     *
     * @return The type of target represented by this target
     */
    TargetType getType() const { return iv_type; }

    /**
     * @brief Set the target type
     *
     * @param[in] i_type The type of target represented by this target
     */
    void setType(const TargetType i_type) { iv_type = i_type; }

    /**
     * @brief Returns if a target type is a chip
     *
     * @param[in] i_type Target Type
     *
     * @return boolean. true if chip else false
     */
    static bool isChip(const TargetType i_type);

    /**
     * @brief Returns if the target is a chip
     *
     * @return boolean. true if chip else false
     */
    bool isChip() const
    {
        return isChip(iv_type);
    }

    /**
     * @brief Returns if a target type is a chiplet
     *
     * @param[in] i_type Target Type
     *
     * @return boolean. true if chip else false
     */
    static bool isChiplet(const TargetType i_type);

    /**
     * @brief Returns if the target is a chiplet
     *
     * This includes all chip units (e.g. XBUS, ABUS)
     *
     * @return boolean. true if chiplet else false
     */
    bool isChiplet() const
    {
        return isChiplet(iv_type);
    }

    /**
     * @brief Returns if a pair of target types is a physical parent/child
     *
     * This is true if the parent type is a chip and the child type is a
     * chiplet that belongs to the chip (e.g. PROC_CHIP->EX_CHIPLET)
     *
     * @param[in] i_parentType Parent Target Type
     * @param[in] i_childType  Child Target Type
     *
     * @return boolean.
     */
    static bool isPhysParentChild(const TargetType i_parentType,
                                  const TargetType i_childType);

    /**
     * @brief Convert a target to an ecmd-format target string
     *
     * This is used by HWP/FAPI code to trace the target.
     *
     * @return Pointer to NULL terminated character string. The caller must use
     *         the string before the Target object is destructed.
     */
    const char * toEcmdString() const;

private:

    /**
     * @brief Convert a target to an ecmd-format target string
     *
     * This is a private function called by toEcmdString to load the ecmd-format
     * string into the Target object. Subsequent calls of toEcmdString are high
     * performance and just return a pointer to the internal string.
     *
     * @note Implemented by platform code
     *
     * @param[out] o_ecmdString. Reference to a character array of length
     *                           MAX_ECMD_STRING_LEN. This is filled in with the
     *                           null terminated ECMD string.
     */
    void toString(char (&o_ecmdString)[MAX_ECMD_STRING_LEN]) const;

    /**
     * @brief Compare the handle
     *
     * @note Implemented by platform code because only platform code knows the
     *       type pointed to by iv_pHandle to compare
     *
     * @param[in] i_right Reference to Target to compare handle to
     *
     * @return bool. True if the same
     */
    bool compareHandle(const Target & i_right) const;

    /**
     * @brief Copy the handle
     *
     * @note Implemented by platform code because only platform code knows the
     *       type pointed to by iv_pHandle to copy
     *
     * @param[in] i_right Reference to Target to copy handle from
     */
    void copyHandle(const Target & i_right);

    /**
     * @brief Delete the handle
     *
     * @note Implemented by platform code because only platform code knows the
     *       type to delete and if it should actually be deleted
     */
    void deleteHandle();

    // Type of target
    TargetType iv_type;

    // Pointer to platform specific Target Handle
    void * iv_pHandle;

    // Pointer to the ecmd-format string representing the Target
    mutable char * iv_pEcmdString;
};

}

#endif // FAPITARGET_H_
