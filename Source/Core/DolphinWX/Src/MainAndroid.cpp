// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Common.h"
#include "FileUtil.h"

#include "Core.h"
#include "Host.h"
#include "CPUDetect.h"
#include "Thread.h"

#include "PowerPC/PowerPC.h"
#include "HW/Wiimote.h"

#include "VideoBackendBase.h"
#include "ConfigManager.h"
#include "LogManager.h"
#include "BootManager.h"
#include "OnScreenDisplay.h"

// Banner loading
#include "Filesystem.h"
#include "BannerLoader.h"
#include "VolumeCreator.h"

#include "Android/ButtonManager.h"

#include <jni.h>
#include <android/log.h>
#include <android/native_window_jni.h>
ANativeWindow* surf;
int g_width, g_height;
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Dolphinemu", __VA_ARGS__))

void Host_NotifyMapLoaded() {}
void Host_RefreshDSPDebuggerWindow() {}

void Host_ShowJitResults(unsigned int address){}

Common::Event updateMainFrameEvent;
void Host_Message(int Id)
{
}

void* Host_GetRenderHandle()
{
	return surf;
}

void* Host_GetInstance() { return NULL; }

void Host_UpdateTitle(const char* title){};

void Host_UpdateLogDisplay(){}

void Host_UpdateDisasmDialog(){}

void Host_UpdateMainFrame()
{
}

void Host_UpdateBreakPointView(){}

bool Host_GetKeyState(int keycode)
{
	return false;
}

void Host_GetRenderWindowSize(int& x, int& y, int& width, int& height)
{
	x = SConfig::GetInstance().m_LocalCoreStartupParameter.iRenderWindowXPos;
	y = SConfig::GetInstance().m_LocalCoreStartupParameter.iRenderWindowYPos;
	width = g_width; 	
	height = g_height; 
}

void Host_RequestRenderWindowSize(int width, int height) {}
void Host_SetStartupDebuggingParameters()
{
}

bool Host_RendererHasFocus()
{
	return true;
}

void Host_ConnectWiimote(int wm_idx, bool connect) {}

void Host_SetWaitCursor(bool enable){}

void Host_UpdateStatusBar(const char* _pText, int Filed){}

void Host_SysMessage(const char *fmt, ...)
{
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsnprintf(msg, 512, fmt, list);
	va_end(list);

	size_t len = strlen(msg);
	if (msg[len - 1] != '\n') {
		msg[len - 1] = '\n';
		msg[len] = '\0';
	}
	LOGI(msg);
}

void Host_SetWiiMoteConnectionState(int _State) {}

void OSDCallbacks(u32 UserData)
{
	switch(UserData)
	{
		case 0: // Init
			ButtonManager::Init();
		break;
		case 1: // Draw
			ButtonManager::DrawButtons();
		break;
		case 2: // Shutdown
			ButtonManager::Shutdown();
		break;
		default:
			WARN_LOG(COMMON, "Error, wrong OSD type");
		break;
	}
}

#define DVD_BANNER_WIDTH 96
#define DVD_BANNER_HEIGHT 32
std::vector<std::string> m_volume_names;
std::vector<std::string> m_names;

bool LoadBanner(std::string filename, u32 *Banner)
{
	DiscIO::IVolume* pVolume = DiscIO::CreateVolumeFromFilename(filename);

	if (pVolume != NULL)
	{
		bool bIsWad = false;
		if (DiscIO::IsVolumeWadFile(pVolume))
			bIsWad = true;

		m_volume_names = pVolume->GetNames();

		// check if we can get some info from the banner file too
		DiscIO::IFileSystem* pFileSystem = DiscIO::CreateFileSystem(pVolume);

		if (pFileSystem != NULL || bIsWad)
		{
			DiscIO::IBannerLoader* pBannerLoader = DiscIO::CreateBannerLoader(*pFileSystem, pVolume);

			if (pBannerLoader != NULL)
				if (pBannerLoader->IsValid())
				{
					m_names = pBannerLoader->GetNames();
					if (pBannerLoader->GetBanner(Banner))
						return true;
				}
		}
	}

	return false;
}
std::string GetName(std::string filename)
{
	if (!m_names.empty())
		return m_names[0];

	if (!m_volume_names.empty())
		return m_volume_names[0];
	// No usable name, return filename (better than nothing)
	std::string name;
	SplitPath(filename, NULL, &name, NULL);

	return name;
}


#ifdef __cplusplus
extern "C"
{
#endif
JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_NativeGLSurfaceView_UnPauseEmulation(JNIEnv *env, jobject obj)
{
	PowerPC::Start();
}
JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_NativeGLSurfaceView_PauseEmulation(JNIEnv *env, jobject obj) 
{
	PowerPC::Pause();
}

JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_NativeGLSurfaceView_StopEmulation(JNIEnv *env, jobject obj) 
{
	PowerPC::Stop();
}
JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_DolphinEmulator_onTouchEvent(JNIEnv *env, jobject obj, jint Action, jfloat X, jfloat Y)
{
	ButtonManager::TouchEvent(Action, X, Y);
}


JNIEXPORT jintArray JNICALL Java_org_dolphinemu_dolphinemu_GameListItem_GetBanner(JNIEnv *env, jobject obj, jstring jFile)
{
	const char *File = env->GetStringUTFChars(jFile, NULL);
	jintArray Banner = env->NewIntArray(DVD_BANNER_WIDTH * DVD_BANNER_HEIGHT);
	u32 uBanner[DVD_BANNER_WIDTH * DVD_BANNER_HEIGHT];
	if (LoadBanner(File, uBanner))
	{
		env->SetIntArrayRegion(Banner, 0, DVD_BANNER_WIDTH * DVD_BANNER_HEIGHT, (jint*)uBanner);
	}
	env->ReleaseStringUTFChars(jFile, File);
	return Banner;
}
JNIEXPORT jstring JNICALL Java_org_dolphinemu_dolphinemu_GameListItem_GetTitle(JNIEnv *env, jobject obj, jstring jFile)
{
	const char *File = env->GetStringUTFChars(jFile, NULL);
	std::string Name = GetName(File);
	m_names.clear();
	m_volume_names.clear();

	env->ReleaseStringUTFChars(jFile, File);
	return env->NewStringUTF(Name.c_str());
}
JNIEXPORT jstring JNICALL Java_org_dolphinemu_dolphinemu_GameListView_GetConfig(JNIEnv *env, jobject obj, jstring jKey, jstring jValue, jstring jDefault)
{
	IniFile ini;
	ini.Load(File::GetUserPath(F_DOLPHINCONFIG_IDX));
	const char *Key = env->GetStringUTFChars(jKey, NULL);
	const char *Value = env->GetStringUTFChars(jValue, NULL);
	const char *Default = env->GetStringUTFChars(jDefault, NULL);
	
	std::string value;
	
	ini.Get(Key, Value, &value, Default);

	env->ReleaseStringUTFChars(jKey, Key);
	env->ReleaseStringUTFChars(jValue, Value);
	env->ReleaseStringUTFChars(jDefault, Default);

	return env->NewStringUTF(value.c_str());
}
JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_GameListView_SetConfig(JNIEnv *env, jobject obj, jstring jKey, jstring jValue, jstring jDefault)
{
	IniFile ini;
	ini.Load(File::GetUserPath(F_DOLPHINCONFIG_IDX));
	const char *Key = env->GetStringUTFChars(jKey, NULL);
	const char *Value = env->GetStringUTFChars(jValue, NULL);
	const char *Default = env->GetStringUTFChars(jDefault, NULL);
	
	ini.Set(Key, Value, Default);
	ini.Save(File::GetUserPath(F_DOLPHINCONFIG_IDX));

	env->ReleaseStringUTFChars(jKey, Key);
	env->ReleaseStringUTFChars(jValue, Value);
	env->ReleaseStringUTFChars(jDefault, Default);
}

JNIEXPORT void JNICALL Java_org_dolphinemu_dolphinemu_NativeGLSurfaceView_main(JNIEnv *env, jobject obj, jstring jFile, jobject _surf, jint _width, jint _height)
{
	surf = ANativeWindow_fromSurface(env, _surf);
	g_width = (int)_width;
	g_height = (int)_height;

	// Install our callbacks
	OSD::AddCallback(OSD::OSD_INIT, OSDCallbacks, 0);
	OSD::AddCallback(OSD::OSD_ONFRAME, OSDCallbacks, 1);
	OSD::AddCallback(OSD::OSD_SHUTDOWN, OSDCallbacks, 2);

	LogManager::Init();
	SConfig::Init();
	VideoBackend::PopulateList();
	VideoBackend::ActivateBackend(SConfig::GetInstance().m_LocalCoreStartupParameter.m_strVideoBackend);
	WiimoteReal::LoadSettings();

	const char *File = env->GetStringUTFChars(jFile, NULL);
	// No use running the loop when booting fails
	if ( BootManager::BootCore( File ) )
		while (PowerPC::GetState() != PowerPC::CPU_POWERDOWN)
			updateMainFrameEvent.Wait();

	WiimoteReal::Shutdown();
	VideoBackend::ClearList();
	SConfig::Shutdown();
	LogManager::Shutdown();
}
	  
#ifdef __cplusplus
}
#endif
