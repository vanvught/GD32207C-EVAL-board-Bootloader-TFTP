/**
 * @file remoteconfig.h
 *
 */
/* Copyright (C) 2019-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef REMOTECONFIG_H_
#define REMOTECONFIG_H_

#include <cstdint>

#if defined (NODE_ARTNET_MULTI)
# define NODE_ARTNET
#endif

#if defined (NODE_E131_MULTI)
# define NODE_E131
#endif

#if defined (OUTPUT_DMX_PIXEL_MULTI)
# define OUTPUT_DMX_PIXEL
#endif

#if defined (OUTPUT_DMX_SEND_MULTI)
# define OUTPUT_DMX_SEND
#endif

#include "configstore.h"

#if !defined(DISABLE_TFTP)
# include "tftp/tftpfileserver.h"
#endif

#if defined(ENABLE_HTTPD)
# include "httpd/httpd.h"
#endif

#include "network.h"

namespace remoteconfig {
namespace udp {
static constexpr auto BUFFER_SIZE = 1024;
} // namespace udp

enum class Node {
	ARTNET,
	E131,
	OSC,
	LTC,
	OSC_CLIENT,
	RDMNET_LLRP_ONLY,
	SHOWFILE,
	MIDI,
	DDP,
	BOOTLOADER_TFTP,
	LAST
};
enum class Output {
	DMX,
	RDM,
	MONITOR,
	PIXEL,
	TIMECODE,
	OSC,
	CONFIG,
	STEPPER,
	PLAYER,
	ARTNET,
	SERIAL,
	RGBPANEL,
	LAST
};

enum class HandleMode {
	TXT
#if !defined(DISABLE_BIN)
	, BIN
#endif
};

enum {
	DISPLAY_NAME_LENGTH = 24,
	ID_LENGTH = (32 + remoteconfig::DISPLAY_NAME_LENGTH + 2) // +2, comma and \n
};

enum class TxtFile {
	RCONFIG,
	NETWORK,
	ARTNET,
	E131,
	OSC_SERVER,
	PARAMS,
	DEVICES,
	LTC,
	TCNET,
	OSC_CLIENT,
	DISPLAY,
	LTCDISPLAY,
	MONITOR,
	SPARKFUN,
	MOTOR0,
	MOTOR1,
	MOTOR2,
	MOTOR3,
	MOTOR4,
	MOTOR5,
	MOTOR6,
	MOTOR7,
	SHOW,
	SERIAL,
	GPS,
	RGBPANEL,
	DDPDISP,
	LAST
};
}  // namespace remoteconfig

class RemoteConfig {
public:
	RemoteConfig(remoteconfig::Node tType, remoteconfig::Output tMode, uint32_t nOutputs = 0);
	~RemoteConfig();

	const char *GetStringNode() const;

	const char *GetStringOutput() const;

	uint8_t GetOutputs() const {
		return s_RemoteConfigListBin.nActiveOutputs;
	}

	const char *GetDisplayName() const {
		return s_RemoteConfigListBin.aDisplayName;
	}

	void SetDisable(bool bDisable = true);
	bool GetDisable() const {
		return m_bDisable;
	}

	void SetDisableWrite(bool bDisableWrite = true) {
		m_bDisableWrite = bDisableWrite;
	}
	bool GetDisableWrite() const {
		return m_bDisableWrite;
	}

	void SetEnableReboot(bool bEnableReboot = true) {
		m_bEnableReboot = bEnableReboot;
	}
	bool IsEnableReboot() const {
		return m_bEnableReboot;
	}

	void SetEnableUptime(bool bEnableUptime = true) {
		m_bEnableUptime = bEnableUptime;
	}
	bool IsEnableUptime() const {
		return m_bEnableUptime;
	}

	void SetDisplayName(const char *pDisplayName);

	void SetEnableFactory(bool bEnableFactory) {
		m_bEnableFactory = bEnableFactory;
	}
	bool GetEnableFactory() const {
		return m_bEnableFactory;
	}

	bool IsReboot() const {
		return m_bIsReboot;
	}

	void Reboot() {
		HandleReboot();
	}

#if !defined(DISABLE_TFTP)
	void TftpExit();
#endif

	int32_t GetIndex(const void *p, uint32_t& nLength);

	uint32_t HandleGet(void *pBuffer, uint32_t nBufferLength);
	void HandleSet(void *pBuffer, uint32_t nBufferLength);

	void Run();

	static RemoteConfig *Get() {
		return s_pThis;
	}

private:
	void HandleReboot();
	void HandleFactory();
	void HandleList();
	void HandleUptime();
	void HandleVersion();

	void HandleGetNoParams() {
		HandleGet(nullptr, 0);
	}

	void HandleGetRconfigTxt(uint32_t& nSize);
	void HandleGetNetworkTxt(uint32_t& nSize);

#if defined (DISPLAY_UDF)
	void HandleGetDisplayTxt(uint32_t& nSize);
#endif

#if defined (NODE_ARTNET)
	void HandleGetArtnetTxt(uint32_t& nSize);
#endif

#if defined (NODE_E131)
	void HandleGetE131Txt(uint32_t& nSize);
#endif

#if defined (NODE_OSC_SERVER)
	void HandleGetOscTxt(uint32_t& nSize);
#endif

#if defined (NODE_LTC_SMPTE)
	void HandleGetLtcTxt(uint32_t& nSize);
	void HandleGetLdisplayTxt(uint32_t& nSize);
	void HandleGetTCNetTxt(uint32_t& nSize);
	void HandleGetGpsTxt(uint32_t& nSize);
#endif

#if defined (NODE_OSC_CLIENT)
	void HandleGetOscClntTxt(uint32_t& nSize);
#endif

#if defined (NODE_SHOWFILE)
	void HandleGetShowTxt(uint32_t& nSize);
#endif

#if defined (NODE_DDP_DISPLAY)
	void HandleGetDdpDisplayTxt(uint32_t& nSize);
#endif

#if defined (OUTPUT_DMX_SEND)
	void HandleGetParamsTxt(uint32_t& nSize);
#endif

#if defined (OUTPUT_DMX_PIXEL) || (OUTPUT_DMX_TLC59711)
	void HandleGetDevicesTxt(uint32_t& nSize);
#endif

#if defined (OUTPUT_DMX_MONITOR)
	void HandleGetMonTxt(uint32_t& nSize);
#endif

#if defined (OUTPUT_DMX_STEPPER)
	void HandleGetSparkFunTxt(uint32_t& nSize);
	void HandleGetMotorTxt(uint32_t nMotorIndex, uint32_t& nSize);
	void HandleGetMotor0Txt(uint32_t& nSize) {
		HandleGetMotorTxt(0, nSize);
	}
	void HandleGetMotor1Txt(uint32_t& nSize) {
		HandleGetMotorTxt(1, nSize);
	}
	void HandleGetMotor2Txt(uint32_t& nSize) {
		HandleGetMotorTxt(2, nSize);
	}
	void HandleGetMotor3Txt(uint32_t& nSize) {
		HandleGetMotorTxt(3, nSize);
	}
	void HandleGetMotor4Txt(uint32_t& nSize) {
		HandleGetMotorTxt(4, nSize);
	}
	void HandleGetMotor5Txt(uint32_t& nSize) {
		HandleGetMotorTxt(5, nSize);
	}
	void HandleGetMotor6Txt(uint32_t& nSize) {
		HandleGetMotorTxt(6, nSize);
	}
	void HandleGetMotor7Txt(uint32_t& nSize) {
		HandleGetMotorTxt(7, nSize);
	}
#endif

#if defined (OUTPUT_DMX_SERIAL)
	void HandleGetSerialTxt(uint32_t& nSize);
#endif

#if defined (OUTPUT_RGB_PANEL)
	void HandleGetRgbPanelTxt(uint32_t& nSize);
#endif

#if !defined(DISABLE_BIN)
	void HandleStoreSet() {
		m_tHandleMode = remoteconfig::HandleMode::BIN;
		HandleSet(nullptr, 0);
	}
#endif

	void HandleSetRconfig();
	void HandleSetNetworkTxt();

#if defined (DISPLAY_UDF)
	void HandleSetDisplayTxt();
#endif

#if defined (NODE_ARTNET)
	void HandleSetArtnetTxt();
#endif

#if defined (NODE_E131)
	void HandleSetE131Txt();
#endif

#if defined (NODE_OSC_SERVER)
	void HandleSetOscTxt();
#endif

#if defined (NODE_LTC_SMPTE)
	void HandleSetLtcTxt();
	void HandleSetLdisplayTxt();
	void HandleSetTCNetTxt();
	void HandleSetGpsTxt();
#endif

#if defined (NODE_OSC_CLIENT)
	void HandleSetOscClientTxt();
#endif

#if defined(NODE_SHOWFILE)
	void HandleSetShowTxt();
#endif

#if defined (NODE_DDP_DISPLAY)
	void HandleSetDdpDisplayTxt();
#endif

#if defined (OUTPUT_DMX_SEND)
	void HandleSetParamsTxt();
#endif

#if defined (OUTPUT_DMX_PIXEL) || (OUTPUT_DMX_TLC59711)
	void HandleSetDevicesTxt();
#endif

#if defined (OUTPUT_DMX_MONITOR)
	void HandleSetMonTxt();
#endif

#if defined (OUTPUT_DMX_SERIAL)
	void HandleSetSerialTxt();
#endif

#if defined (OUTPUT_RGB_PANEL)
	void HandleSetRgbPanelTxt();
#endif

#if defined (OUTPUT_DMX_STEPPER)
	void HandleSetSparkFunTxt();
	void HandleSetMotorTxt(uint32_t nMotorIndex);
	void HandleSetMotor0Txt() {
		HandleSetMotorTxt(0);
	}
	void HandleSetMotor1Txt() {
		HandleSetMotorTxt(1);
	}
	void HandleSetMotor2Txt() {
		HandleSetMotorTxt(2);
	}
	void HandleSetMotor3Txt() {
		HandleSetMotorTxt(3);
	}
	void HandleSetMotor4Txt() {
		HandleSetMotorTxt(4);
	}
	void HandleSetMotor5Txt() {
		HandleSetMotorTxt(5);
	}
	void HandleSetMotor6Txt() {
		HandleSetMotorTxt(6);
	}
	void HandleSetMotor7Txt() {
		HandleSetMotorTxt(7);
	}
#endif

	void HandleDisplaySet();
	void HandleDisplayGet();
#if !defined(DISABLE_BIN)
	void HandleStoreGet();
#endif
	void HandleTftpSet();
	void HandleTftpGet();

private:
	remoteconfig::Node m_tNode;
	remoteconfig::Output m_tOutput;
	uint32_t m_nActiveOutputs;

	struct Commands {
		void (RemoteConfig::*pHandler)();
		const char *pCmd;
		const uint16_t nLength;
		const bool bGreaterThan;
	};

	static const Commands s_GET[];
	static const Commands s_SET[];

	struct Txt {
		void (RemoteConfig::*GetHandler)(uint32_t& nSize);
		void (RemoteConfig::*SetHandler)();
		const char *pFileName;
		const uint8_t nFileNameLength;
		const configstore::Store nStore;
	};

	static const Txt s_TXT[];

	struct ListBin {
		uint8_t aMacAddress[network::MAC_SIZE];
		uint8_t nNode;
		uint8_t nOutput;
		uint8_t nActiveOutputs;
		char aDisplayName[remoteconfig::DISPLAY_NAME_LENGTH];
	}__attribute__((packed));

	static ListBin s_RemoteConfigListBin;

	bool m_bDisable { false };
	bool m_bDisableWrite { false };
	bool m_bEnableReboot { false };
	bool m_bEnableUptime { false };
	bool m_bEnableFactory { false };

	bool m_bIsReboot { false };

	int32_t m_nHandle { -1 };
	uint32_t m_nIPAddressFrom { 0 };
	uint16_t m_nBytesReceived { 0 };

	remoteconfig::HandleMode m_tHandleMode { remoteconfig::HandleMode::TXT };

#if !defined(DISABLE_TFTP)
	TFTPFileServer *m_pTFTPFileServer { nullptr };
	uint8_t *m_pTFTPBuffer { nullptr };
	bool m_bEnableTFTP { false };
#endif

#if !defined(DISABLE_BIN)
	static uint8_t s_StoreBuffer[remoteconfig::udp::BUFFER_SIZE];
#endif

#if defined(ENABLE_HTTPD)
	HttpDaemon m_HttpDaemon;
#endif

	static char *s_pUdpBuffer;

	static RemoteConfig *s_pThis;
};

#endif /* REMOTECONFIG_H_ */
