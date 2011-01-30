#include "../StdAfx.h"
#include "LuaManager.h"
#include "../Rainmeter.h"

#include "lua_measure.h"
#include "lua_meter.h"
#include "lua_group.h"
#include "lua_meterwindow.h"
#include "lua_rainmeter.h"
#include "lua_meterstring.h"
#include "lua_meterimage.h"

#include "lua_rainmeter_ext.h"

int LuaManager::m_RefCount = 0;
lua_State* LuaManager::m_pState = 0;

void LuaManager::Init()
{
	if (m_pState == 0)
	{
		// initialize Lua 
		m_pState = lua_open();

		//load Lua base libraries
		luaL_openlibs(m_pState);

		tolua_measure_open(m_pState);
		tolua_group_open(m_pState);
		tolua_meter_open(m_pState);
		tolua_meterwindow_open(m_pState);
		tolua_rainmeter_open(m_pState);
		tolua_meter_string_open(m_pState);
		//tolua_meter_image_open(m_pState);

		luaopen_rainmeter_ext(m_pState);
	}

	++m_RefCount;
}

void LuaManager::CleanUp()
{
	if (m_RefCount > 0)
	{
		--m_RefCount;
	}

	if (m_RefCount == 0 && m_pState != 0)
	{
		lua_close(m_pState);
		m_pState = 0;
	}
}

void LuaManager::ReportErrors(lua_State * L)
{
	LuaLog(LOG_ERROR, "Script: %s", lua_tostring(L, -1));
	lua_pop(L, 1);
} 

void LuaManager::LuaLog(int nLevel, const char* format, ... )
{
	char buffer[4096];
	va_list args;
	va_start( args, format );

	_invalid_parameter_handler oldHandler = _set_invalid_parameter_handler(RmNullCRTInvalidParameterHandler);
	_CrtSetReportMode(_CRT_ASSERT, 0);

	errno = 0;
	_vsnprintf_s( buffer, _TRUNCATE, format, args );
	if (errno != 0)
	{
		_snprintf_s(buffer, _TRUNCATE, "Script: LuaLog internal error: %s", format);
	}

	_set_invalid_parameter_handler(oldHandler);

#ifndef _DEBUG
	// Forcing output to the Debug Output window!
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
#endif

	std::wstring str = ConvertToWide(buffer);

	LSLog(nLevel, L"Rainmeter", str.c_str());
	va_end(args);
}