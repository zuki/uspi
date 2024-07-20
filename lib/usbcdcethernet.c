//
// usbcdcethernet.c
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2017-2019  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <uspi/types.h>
#include <uspi/usbcdcethernet.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/types.h>
#include <uspi/util.h>
#include <uspi/assert.h>
#include <uspi.h>
#include <uspios.h>

typedef struct TEthernetNetworkingFunctionalDescriptor
{
    u8      bLength;
    u8      bDescriptorType;
    u8      bDescriptorSubtype;
#define ETHERNET_NETWORKING_FUNCTIONAL_DESCRIPTOR    0x0F
    u8      iMACAddress;
    u32     bmEthernetStatistics;
    u16     wMaxSegmentSize;
    u16     wNumberMCFilters;
    u8      bNumberPowerFilters;
} PACKED TEthernetNetworkingFunctionalDescriptor;

static const char FromCDCEthernet[] = "ucdceth";

// starting at 20, to be sure to not collide with smsc951x/lan7800 driver
static unsigned s_nDeviceNumber = 20;

static boolean USBCDCEthernetDeviceInitMACAddress(TUSBCDCEthernetDevice *pThis, u8 iMACAddress)
{
	assert (pThis != 0);

	TUSBString macaddr_string;
	USBString(&macaddr_string, USBFunctionGetDevice(&pThis->m_USBFunction));

	if (iMACAddress == 0
	|| !USBStringGetFromDescriptor(&macaddr_string, iMACAddress,  USBStringGetLanguageID(&macaddr_string)))
	{
		return FALSE;
	}

	const char *pMACString = USBStringGet(&macaddr_string);
	assert (pMACString != 0);

	u8 MACAddress[MAC_ADDRESS_SIZE];
	for (unsigned i = 0; i < MAC_ADDRESS_SIZE; i++)
	{
		u8 ucByte = 0;
		for (unsigned j = 0; j < 2; j++)
		{
		char chChar = *pMACString++;
		if (chChar > '9')
		{
			chChar -= 'A'-'9'-1;
		}
		chChar -= '0';

		if (!('\0' <= chChar && chChar <= '\xF'))
		{
			return FALSE;
		}

		ucByte <<= 4;
		ucByte |= (u8) chChar;
		}

		MACAddress[i] = ucByte;
	}

	MACAddressSet(pThis->m_MACAddress, MACAddress);

	return TRUE;
}

void USBCDCEthernetDevice(TUSBCDCEthernetDevice *pThis, TUSBFunction *pfunction)
{
	assert(pThis != 0);
	USBFunctionCopy(&pThis->m_USBFunction, pfunction);
	pThis->m_USBFunction.Configure = USBCDCEthernetDeviceConfigure;
	pThis->m_pEndpointBulkIn = 0;
	pThis->m_pEndpointBulkOut = 0;
}

void _USBCDCEthernetDevice(TUSBCDCEthernetDevice *pThis)
{
	assert(pThis != 0);

	if (pThis->m_pEndpointBulkOut != 0)
	{
		_USBEndpoint (pThis->m_pEndpointBulkOut);
		free (pThis->m_pEndpointBulkOut);
		pThis->m_pEndpointBulkOut = 0;
	}

	if (pThis->m_pEndpointBulkIn != 0)
	{
		_USBEndpoint (pThis->m_pEndpointBulkIn);
		free (pThis->m_pEndpointBulkIn);
		pThis->m_pEndpointBulkIn = 0;
	}

	_USBFunction (&pThis->m_USBFunction);
}

boolean USBCDCEthernetDeviceConfigure(TUSBFunction *pUSBFunction)
{
	TUSBCDCEthernetDevice *pThis = (TUSBCDCEthernetDevice *)pUSBFunction;
	assert(pThis != 0);

    	// Ethernetネットワーキング機能ディスクリプタを見つける
    	const TEthernetNetworkingFunctionalDescriptor *pEthernetDesc;
    	while ((pEthernetDesc = (TEthernetNetworkingFunctionalDescriptor *)
                USBFunctionGetDescriptor(&pThis->m_USBFunction,DESCRIPTOR_CS_INTERFACE)) != 0)
	{
		if (pEthernetDesc->bDescriptorSubtype == ETHERNET_NETWORKING_FUNCTIONAL_DESCRIPTOR)
		{
			break;
		}
	}

	if (pEthernetDesc == 0)
	{
		USBFunctionConfigurationError(&pThis->m_USBFunction, FromCDCEthernet);

		return FALSE;
	}

	// データクラスインタフェースディスクリプタを見つける
	const TUSBInterfaceDescriptor *pInterfaceDesc;
	while ((pInterfaceDesc = (TUSBInterfaceDescriptor *) USBFunctionGetDescriptor(&pThis->m_USBFunction, DESCRIPTOR_INTERFACE)) != 0)
	{
		if (pInterfaceDesc->bInterfaceClass   == 0x0A
		&& pInterfaceDesc->bInterfaceSubClass == 0x00
		&& pInterfaceDesc->bInterfaceProtocol == 0x00
		&& pInterfaceDesc->bNumEndpoints      >= 2)
		{
			break;
		}
	}

	if (pInterfaceDesc == 0)
	{
		USBFunctionConfigurationError(&pThis->m_USBFunction, FromCDCEthernet);

		return FALSE;
	}

	// MACアドレスを初期化
	if (!USBCDCEthernetDeviceInitMACAddress(pThis, pEthernetDesc->iMACAddress))
	{
		LogWrite(FromCDCEthernet, LOG_ERROR,  "Cannot get MAC address");

		return FALSE;
	}

	TString macaddr_str;
	String(&macaddr_str);
	MACAddressFormat(pThis->m_MACAddress, &macaddr_str);
	LogWrite(FromCDCEthernet, LOG_DEBUG, "MAC address is %s", (const char *) &macaddr_str);

	// エンドポイントを取得
	const TUSBEndpointDescriptor *pEndpointDesc;
	while ((pEndpointDesc = (TUSBEndpointDescriptor *) USBFunctionGetDescriptor (&pThis->m_USBFunction, DESCRIPTOR_ENDPOINT)) != 0)
	{
		if ((pEndpointDesc->bmAttributes & 0x3F) == 0x02)           // バルク転送
		{
			if ((pEndpointDesc->bEndpointAddress & 0x80) == 0x80)   // 入力パイプ
			{
				if (pThis->m_pEndpointBulkIn != 0)
				{
					USBFunctionConfigurationError (&pThis->m_USBFunction, FromCDCEthernet);

					return FALSE;
				}

				pThis->m_pEndpointBulkIn = (TUSBEndpoint *) malloc(sizeof(TUSBEndpoint));
				assert (pThis->m_pEndpointBulkIn);
				USBEndpoint2 (pThis->m_pEndpointBulkIn, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);

			}
			else    // 出力パイプ
			{
				if (pThis->m_pEndpointBulkOut != 0)
				{
					USBFunctionConfigurationError (&pThis->m_USBFunction, FromCDCEthernet);

					return FALSE;
				}

				pThis->m_pEndpointBulkOut = (TUSBEndpoint *) malloc(sizeof(TUSBEndpoint));
				assert (pThis->m_pEndpointBulkOut);
				USBEndpoint2 (pThis->m_pEndpointBulkOut, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);
			}
		}
	}

	if (pThis->m_pEndpointBulkIn  == 0 || pThis->m_pEndpointBulkOut == 0)
	{
		USBFunctionConfigurationError (&pThis->m_USBFunction, FromCDCEthernet);

		return FALSE;
	}
	// コンフィグレーションを行う
	if (!USBFunctionConfigure(&pThis->m_USBFunction))
	{
		LogWrite (FromCDCEthernet, LOG_ERROR, "Cannot set interface");

		return FALSE;
	}

	// このデバイスを登録
	TString DeviceName;
	String(&DeviceName);
	StringFormat(&DeviceName, "eth%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice(DeviceNameServiceGet(), StringGet(&DeviceName), pThis, FALSE);
	_String (&DeviceName);

	return TRUE;
}

TMACAddress *USBCDCEthernetDeviceGetMACAddress(TUSBCDCEthernetDevice *pThis)
{
	assert (pThis != 0);
    	return pThis->m_MACAddress;
}

boolean USBCDCEthernetDeviceSendFrame(TUSBCDCEthernetDevice *pThis, void *pBuffer, unsigned nLength)
{
	assert(&pThis->m_pEndpointBulkOut != 0);
	assert(pBuffer != 0);
	assert(nLength <= USPI_FRAME_BUFFER_SIZE);
	return DWHCIDeviceTransfer(USBFunctionGetHost(&pThis->m_USBFunction), pThis->m_pEndpointBulkOut,
				    pBuffer, nLength) >= 0;
}

boolean USBCDCEthernetDeviceReceiveFrame(TUSBCDCEthernetDevice *pThis, void *pBuffer, unsigned *pResultLength)
{
	assert(pThis != 0);
	assert(pThis->m_pEndpointBulkIn != 0);
	assert(pBuffer != 0);
	TUSBRequest URB;
	USBRequest(&URB, pThis->m_pEndpointBulkIn, pBuffer, USPI_FRAME_BUFFER_SIZE, 0);

	// FIXME: uspiではこの機能はなし
	// URB.SetCompleteOnNAK ();

	if (!DWHCIDeviceSubmitBlockingRequest(USBFunctionGetHost(&pThis->m_USBFunction), &URB))
	{
		return FALSE;
	}

	u32 nResultLength = USBRequestGetResultLength(&URB);
	if (nResultLength == 0)
	{
		return FALSE;
	}

	*pResultLength = nResultLength;
	return TRUE;
}
