/*
* Copyright (c) 2017-2019, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file     codechal_mmc_decode_vc1_g12.cpp
//! \brief    Impelements the public interface for Gen12 CodecHal Media Memory Compression
//!

#include "codechal_mmc_decode_vc1_g12.h"

CodechalMmcDecodeVc1G12::CodechalMmcDecodeVc1G12(
    CodechalHwInterface    *hwInterface,
    void *standardState):
    CodecHalMmcStateG12(hwInterface)
{
    CODECHAL_DECODE_FUNCTION_ENTER;

    m_vc1State = (CodechalDecodeVc1 *)standardState;
    CODECHAL_HW_ASSERT(m_vc1State);

    m_hwInterface = hwInterface;
    CODECHAL_HW_ASSERT(m_hwInterface);

    InitDecodeMmcEnable(hwInterface);

    m_mmcExtensionEnabled = true;

    m_vc1MmcExt = MOS_New(CodechalMmcDecodeVc1G12Ext, hwInterface, this);

    CODECHAL_HW_ASSERT(m_vc1MmcExt);
}

CodechalMmcDecodeVc1G12::~CodechalMmcDecodeVc1G12()
{
    if (m_vc1MmcExt != nullptr)
    {
        MOS_Delete(m_vc1MmcExt);
    }
}

MOS_STATUS CodechalMmcDecodeVc1G12::SetPipeBufAddr(
    PMHW_VDBOX_PIPE_BUF_ADDR_PARAMS pipeBufAddrParams,
    PMOS_COMMAND_BUFFER cmdBuffer)
{
    MOS_STATUS eStatus = MOS_STATUS_SUCCESS;

    CODECHAL_DECODE_FUNCTION_ENTER;

    if (m_mmcEnabled)
    {
        pipeBufAddrParams->bMmcEnabled = true;
        if (m_vc1State->m_deblockingEnabled)
        {
            CODECHAL_DECODE_CHK_STATUS_RETURN(m_osInterface->pfnGetMemoryCompressionMode(m_osInterface,
                &m_vc1State->m_destSurface.OsResource,
                &pipeBufAddrParams->PostDeblockSurfMmcState));
        }
        else
        {
            CODECHAL_DECODE_CHK_STATUS_RETURN(m_osInterface->pfnGetMemoryCompressionMode(m_osInterface,
                &m_vc1State->m_destSurface.OsResource,
                &pipeBufAddrParams->PreDeblockSurfMmcState));
        }
    }
    else
    {
        pipeBufAddrParams->bMmcEnabled             = false;
        pipeBufAddrParams->PreDeblockSurfMmcState  = MOS_MEMCOMP_DISABLED;
        pipeBufAddrParams->PostDeblockSurfMmcState = MOS_MEMCOMP_DISABLED;
    }

    CODECHAL_DEBUG_TOOL(
        m_vc1State->m_destSurface.MmcState = m_vc1State->m_deblockingEnabled ? pipeBufAddrParams->PostDeblockSurfMmcState : pipeBufAddrParams->PreDeblockSurfMmcState;)

    return eStatus;
}

MOS_STATUS CodechalMmcDecodeVc1G12::CopyAuxSurfForSkip(
    PMOS_COMMAND_BUFFER cmdBuffer,
    PMOS_RESOURCE       srcResource,
    PMOS_RESOURCE       destResource)
{
    MOS_STATUS eStatus = MOS_STATUS_SUCCESS;

    CODECHAL_DECODE_FUNCTION_ENTER;

    if (m_vc1MmcExt != nullptr)
    {
        CODECHAL_DECODE_CHK_STATUS_RETURN(m_vc1MmcExt->CopyAuxSurfForSkip(
            cmdBuffer,
            srcResource,
            destResource));
    }

    return eStatus;
}
