﻿#include "stdafx.h"
#include "SettingsHelper.h"
#include "TrafficMonitor.h"

CSettingsHelper::CSettingsHelper()
    : CIniHelper(theApp.m_config_path)
{
}

CSettingsHelper::CSettingsHelper(const std::wstring& file_path)
    : CIniHelper(file_path)
{
}

void CSettingsHelper::SaveFontData(const wchar_t* AppName, const FontInfo& font)
{
    WriteString(AppName, L"font_name", wstring(font.name));
    WriteInt(AppName, L"font_size", font.size);
    bool style[4];
    style[0] = font.bold;
    style[1] = font.italic;
    style[2] = font.underline;
    style[3] = font.strike_out;
    WriteBoolArray(AppName, L"font_style", style, 4);
}

void CSettingsHelper::LoadFontData(const wchar_t* AppName, FontInfo& font, const FontInfo& default_font) const
{
    font.name = GetString(AppName, L"font_name", default_font.name).c_str();
    font.size = GetInt(AppName, L"font_size", default_font.size);
    bool style[4];
    GetBoolArray(AppName, L"font_style", style, 4);
    font.bold = style[0];
    font.italic = style[1];
    font.underline = style[2];
    font.strike_out = style[3];
}

void CSettingsHelper::LoadMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::set<CommonDisplayItem>& all_items, std::map<CommonDisplayItem, COLORREF>& text_colors, COLORREF default_color)
{
    CString default_str;
    default_str.Format(_T("%d"), default_color);
    wstring str{ default_str.GetString() };
    _GetString(AppName, KeyName, str);
    std::vector<wstring> split_result;
    CCommon::StringSplit(str, L',', split_result);
    size_t index = 0;
    const std::set<CommonDisplayItem>& items{ all_items.empty() ? theApp.m_plugins.AllDisplayItemsWithPlugins() : all_items };
    for (auto iter = items.begin(); iter != items.end(); ++iter)
    {
        if (index < split_result.size())
            text_colors[*iter] = _wtoi(split_result[index].c_str());
        else if (!split_result.empty())
            text_colors[*iter] = _wtoi(split_result[0].c_str());
        else
            text_colors[*iter] = default_color;
        index++;
    }
}

void CSettingsHelper::SaveMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, COLORREF>& text_colors)
{
    CString str;
    for (auto iter = text_colors.begin(); iter != text_colors.end(); ++iter)
    {
        CString tmp;
        tmp.Format(_T("%d,"), iter->second);
        str += tmp;
    }
    _WriteString(AppName, KeyName, wstring(str));
}

void CSettingsHelper::LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, const wchar_t* default_str)
{
    wstring str{ default_str };
    _GetString(AppName, KeyName, str);
    std::vector<wstring> split_result;
    CCommon::StringSplit(str, L',', split_result);
    size_t index = 0;
    COLORREF default_color = _wtoi(default_str);
    for (auto iter = theApp.m_plugins.AllDisplayItemsWithPlugins().begin(); iter != theApp.m_plugins.AllDisplayItemsWithPlugins().end(); ++iter)
    {
        if (index < split_result.size())
            text_colors[*iter].label = _wtoi(split_result[index].c_str());
        else if (!split_result.empty())
            text_colors[*iter].label = _wtoi(split_result[0].c_str());
        else
            text_colors[*iter].label = default_color;

        if (index + 1 < split_result.size())
            text_colors[*iter].value = _wtoi(split_result[index + 1].c_str());
        else if (split_result.size() > 1)
            text_colors[*iter].value = _wtoi(split_result[1].c_str());
        else
            text_colors[*iter].value = default_color;
        index += 2;
    }
}

void CSettingsHelper::LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, COLORREF default_color)
{
    CString default_str;
    default_str.Format(_T("%d"), default_color);
    LoadTaskbarWndColors(AppName, KeyName, text_colors, default_str.GetString());
}

void CSettingsHelper::SaveTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, TaskbarItemColor>& text_colors)
{
    CString str;
    for (auto iter = text_colors.begin(); iter != text_colors.end(); ++iter)
    {
        CString tmp;
        tmp.Format(_T("%d,%d,"), iter->second.label, iter->second.value);
        str += tmp;
    }
    _WriteString(AppName, KeyName, wstring(str));
}

void CSettingsHelper::LoadDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window) const
{
    //读取一个显示文本设置
    auto getDisplayString = [&](const wchar_t* key_name, DisplayItem display_item, const wchar_t* default_str) {
        std::wstring str{ default_str };
        bool exist = GetString(AppName, key_name, str);
        //主窗口只读取配置文件中存在的项，任务栏窗口读取所有项
        if (!is_main_window || exist)
            disp_str.Get(display_item) = str;
    };

    getDisplayString(L"up_string", TDI_UP, is_main_window ? CCommon::LoadText(IDS_UPLOAD_DISP, L": $").GetString() : L"↑: $");
    getDisplayString(L"down_string", TDI_DOWN, is_main_window ? CCommon::LoadText(IDS_DOWNLOAD_DISP, L": $").GetString() : L"↓: $");
    getDisplayString(L"total_speed_string", TDI_TOTAL_SPEED, _T("↑↓: $"));
    getDisplayString(L"cpu_string", TDI_CPU, L"CPU: $");
    getDisplayString(L"cpu_freq_string", TDI_CPU_FREQ, CCommon::LoadText(IDS_CPU_FREQ, _T(": $")));
    getDisplayString(L"memory_string", TDI_MEMORY, CCommon::LoadText(IDS_MEMORY_DISP, _T(": $")));
    getDisplayString(L"gpu_string", TDI_GPU_USAGE, CCommon::LoadText(IDS_GPU_DISP, _T(": $")));
    getDisplayString(L"cpu_temp_string", TDI_CPU_TEMP, L"CPU: $");
    getDisplayString(L"gpu_temp_string", TDI_GPU_TEMP, CCommon::LoadText(IDS_GPU_DISP, _T(": $")));
    getDisplayString(L"hdd_temp_string", TDI_HDD_TEMP, CCommon::LoadText(IDS_HDD_DISP, _T(": $")));
    getDisplayString(L"main_board_temp_string", TDI_MAIN_BOARD_TEMP, CCommon::LoadText(IDS_MAINBOARD_DISP, _T(": $")));
    getDisplayString(L"hdd_string", TDI_HDD_USAGE, CCommon::LoadText(IDS_HDD_DISP, _T(": $")));
    getDisplayString(L"today_traffic_string", TDI_TODAY_TRAFFIC, CCommon::LoadText(IDS_TRAFFIC_USED, _T(": $")));
}

void CSettingsHelper::SaveDisplayStr(const wchar_t* AppName, const DispStrings& disp_str)
{
    //写入一个显示文本设置
    auto writeDisplayString = [&](const wchar_t* key_name, DisplayItem display_item) {
        if (disp_str.GetAllItems().find(display_item) != disp_str.GetAllItems().end())
        {
            //仅当该显示文本项目存在时才写入
            WriteString(AppName, key_name, disp_str.GetConst(display_item));
        }
    };

    writeDisplayString(_T("up_string"), TDI_UP);
    writeDisplayString(_T("down_string"), TDI_DOWN);
    writeDisplayString(_T("total_speed_string"), TDI_TOTAL_SPEED);
    writeDisplayString(_T("cpu_string"), TDI_CPU);
    writeDisplayString(_T("memory_string"), TDI_MEMORY);
    writeDisplayString(_T("gpu_string"), TDI_GPU_USAGE);
    writeDisplayString(_T("cpu_temp_string"), TDI_CPU_TEMP);
    writeDisplayString(_T("cpu_freq_string"), TDI_CPU_FREQ);
    writeDisplayString(_T("gpu_temp_string"), TDI_GPU_TEMP);
    writeDisplayString(_T("hdd_temp_string"), TDI_HDD_TEMP);
    writeDisplayString(_T("main_board_temp_string"), TDI_MAIN_BOARD_TEMP);
    writeDisplayString(_T("hdd_string"), TDI_HDD_USAGE);
    writeDisplayString(_T("today_traffic_string"), TDI_TODAY_TRAFFIC);
}

void CSettingsHelper::LoadPluginDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window)
{
    for (const auto& plugin : theApp.m_plugins.GetPluginItems())
    {
        std::wstring str{ plugin->GetItemLableText() };
        bool exist = GetString(AppName, plugin->GetItemId(), str);
        //主窗口只读取配置文件中存在的项，任务栏窗口读取所有项
        if (!is_main_window || exist)
            disp_str.Load(plugin->GetItemId(), str);
    }
}

void CSettingsHelper::SavePluginDisplayStr(const wchar_t* AppName, const DispStrings& disp_str)
{
    for (const auto& plugin : theApp.m_plugins.GetPluginItems())
    {
        if (disp_str.GetAllItems().find(plugin) != disp_str.GetAllItems().end())
            WriteString(AppName, plugin->GetItemId(), disp_str.GetConst(plugin));
    }
}
