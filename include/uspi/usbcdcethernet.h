// usbcdcethernet.h
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
//
#ifndef _uspi__usbcdcethernet_h
#define _uspi__usbcdcethernet_h

#include <uspi/types.h>
#include <uspi/usbfunction.h>
#include <uspi/usbendpoint.h>
#include <uspi/usbrequest.h>
#include <uspi/macaddress.h>

/// @brief CDC Ethernet構造体
typedef struct TUSBCDCEthernetDevice {
    /// @brief USB機能構造体
    TUSBFunction 	m_USBFunction;
    /// @brief バルク転送入力用パイプ
    TUSBEndpoint   	*m_pEndpointBulkIn;
    /// @brief バルク転送出力用パイプ
    TUSBEndpoint   	*m_pEndpointBulkOut;
    /// @brief MACアドレス
    TMACAddress     	*m_MACAddress;
} TUSBCDCEthernetDevice;

void USBCDCEthernetDevice(TUSBCDCEthernetDevice *pThis, TUSBFunction *pfunction);
void _USBCDCEthernetDevice(TUSBCDCEthernetDevice *pThis);
boolean USBCDCEthernetDeviceConfigure(TUSBFunction *pUSBFunction);
TMACAddress *USBCDCEthernetDeviceGetMACAddress(TUSBCDCEthernetDevice *pThis);
boolean USBCDCEthernetDeviceSendFrame(TUSBCDCEthernetDevice *pThis, void *pBuffer, unsigned length);
boolean USBCDCEthernetDeviceReceiveFrame(TUSBCDCEthernetDevice *pThis, void *pBuffer, unsigned *pResultLength);

#endif
