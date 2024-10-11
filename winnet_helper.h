/*
* Author: LowBoyTeam (https://github.com/LowBoyTeam)
* License: Code Project Open License
* Disclaimer: The software is provided "as-is". No claim of suitability, guarantee, or any warranty whatsoever is provided.
* Copyright (c) 2016-2017.
*/

#ifndef _WINNET_HPP_INCLUDED_
#define _WINNET_HPP_INCLUDED_

#include <string>
#include <tchar.h>
#include <vector>
#include <map>
#include <regex>
#include <assert.h>

#include <windows.h>
#include <strsafe.h>

#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

namespace winnet
{
    const std::string HTTP_METHOD_GET = "GET";
    const std::string HTTP_METHOD_POST = "POST";
    const std::string HTTP_METHOD_PUT = "PUT";
    const std::string HTTP_METHOD_HEAD = "HEAD";
    const std::string HTTP_METHOD_DELETE = "DELETE";

    typedef std::map<std::string, std::string> HeaderFields;
    typedef std::map<std::string, std::string> Cookies;

    class CWinNet
    {
    private:
        HINTERNET m_hSession;
        HINTERNET m_hConnect;
        HINTERNET m_hRequest;

        std::string m_strHost;
        std::string m_strPath;
        std::string m_strExt;

        INTERNET_SCHEME m_nScheme;
        INTERNET_PORT m_nPort;

        BOOL m_followRedirects;
        HeaderFields m_RequestHeaders;
        std::string m_request_method;

        std::string m_proxy_info;
        std::string m_proxy_user;
        std::string m_proxy_pass;

        std::string m_basic_auth_user;
        std::string m_basic_auth_pass;

        Cookies ParseCookies(const std::string& cookies);

    public:
        CWinNet();
        ~CWinNet();

        BOOL Open(const std::string& Url, const std::string& method);
        BOOL SetTimeout(DWORD dwTimeOut);
        void SetRequestHeader(const std::string& key, const std::string& value);
        void SetHeaders(HeaderFields headers);
        void SetUserAgent(const std::string& userAgent);
        void SetCookie(const std::string& cookies);
        void SetReferer(const std::string& referer);
        void FollowRedirects(BOOL follow);
        BOOL Send(LPVOID lpPostData = NULL, DWORD dwsize = 0);
        std::vector<BYTE> GetResponseBody();
        std::string GetResponseHeaders();
        std::string GetResponseHeaderValue(const std::string& name);
        std::string GetResponseHeaderValue(int dwInfoLevel, DWORD dwIndex = NULL);
        HeaderFields GetHeaderFields();
        std::string GetLocal();
        DWORD GetStatusCode();
        std::string GetCookieStr();
        Cookies GetCookies();
        void SetBasicAuthenticator(const std::string& username, const std::string& password);
        void SetProxy(const std::string& proxy, const std::string& proxyName = "", const std::string& proxyPass = "");
    };
}

#endif // _WINNET_HPP_INCLUDED_
