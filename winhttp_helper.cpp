#include "winhttp_helper.h"

namespace winhttp
{
    CWinHttp::CWinHttp()
        : m_hSession(NULL),
          m_hConnect(NULL),
          m_hRequest(NULL),
          m_nScheme(INTERNET_SCHEME_HTTP),
          m_nPort(0),
          m_followRedirects(FALSE)
    {
    }

    CWinHttp::~CWinHttp()
    {
        if (m_hRequest)
        {
            WinHttpCloseHandle(m_hRequest);
            m_hRequest = NULL;
        }
        if (m_hConnect)
        {
            WinHttpCloseHandle(m_hConnect);
            m_hConnect = NULL;
        }
        if (m_hSession)
        {
            WinHttpCloseHandle(m_hSession);
            m_hSession = NULL;
        }
    }

    Cookies CWinHttp::ParseCookies(const std::string& cookies)
    {
        Cookies result;
        std::string cookies_pattern("([\\S]+?)=([^;|^\\r|^\\n]+)");
        std::regex cookies_regex(cookies_pattern);
        std::smatch results;

        auto cookies_begin = std::sregex_iterator(cookies.begin(), cookies.end(), cookies_regex);
        auto cookies_end = std::sregex_iterator();

        for (auto i = cookies_begin; i != cookies_end; ++i)
        {
            std::smatch match = *i;
            if (match.size() == 3)
                result[match[1].str()] = match[2].str();
        }

        return result;
    }

    BOOL CWinHttp::Open(const std::string& Url, const std::string& method)
    {
        m_hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!m_hSession)
        {
            printf("Error %u in WinHttpOpen.\n", GetLastError());
            return FALSE;
        }

        if (WinHttpCheckPlatform())
        {
            URL_COMPONENTSW urlComp;
            ZeroMemory(&urlComp, sizeof(urlComp));
            urlComp.dwStructSize = sizeof(urlComp);
            urlComp.dwHostNameLength = 1;
            urlComp.dwUserNameLength = 1;
            urlComp.dwPasswordLength = 1;
            urlComp.dwUrlPathLength = 1;
            urlComp.dwExtraInfoLength = 1;

            std::wstring wstrUrl = static_cast<std::wstring>(textconv_helper::A2W_(Url.c_str()));

            if (!WinHttpCrackUrl(wstrUrl.c_str(), 0, 0, &urlComp))
            {
                printf("Error %u in WinHttpCrackUrl.\n", GetLastError());
                return FALSE;
            }

            m_strHost = std::wstring(urlComp.lpszHostName, urlComp.dwHostNameLength);
            m_strPath = std::wstring(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
            m_strExt = std::wstring(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
            m_nScheme = urlComp.nScheme;
            m_nPort = urlComp.nPort;
            m_strPath.append(m_strExt);
            m_request_method = textconv_helper::A2W_(method.c_str());
            return TRUE;
        }
        return FALSE;
    }

    BOOL CWinHttp::SetTimeout(int nTimeOut)
    {
        assert(m_hSession != NULL);
        return WinHttpSetTimeouts(m_hSession, nTimeOut, nTimeOut, nTimeOut, nTimeOut);
    }

    void CWinHttp::SetRequestHeader(const std::string& key, const std::string& value)
    {
        m_RequestHeaders[key] = value;
    }

    void CWinHttp::SetHeaders(HeaderFields headers)
    {
        m_RequestHeaders = headers;
    }

    void CWinHttp::SetUserAgent(const std::string& userAgent)
    {
        SetRequestHeader("User-Agent", userAgent);
    }

    void CWinHttp::SetCookie(const std::string& cookies)
    {
        SetRequestHeader("Cookie", cookies);
    }

    void CWinHttp::SetReferer(const std::string& referer)
    {
        SetRequestHeader("Referer", referer);
    }

    void CWinHttp::FollowRedirects(BOOL follow)
    {
        m_followRedirects = follow;
    }

    BOOL CWinHttp::Send(LPVOID lpPostData, DWORD dwsize)
    {
        assert(m_hSession != NULL);

        m_hConnect = WinHttpConnect(m_hSession, m_strHost.c_str(), m_nPort, 0);
        if (!m_hConnect)
        {
            printf("Error %u in WinHttpConnect.\n", GetLastError());
            return FALSE;
        }

        if (!m_proxy_info.empty())
        {
            WINHTTP_PROXY_INFO proxyInfo;
            proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
            proxyInfo.lpszProxy = const_cast<LPWSTR>(m_proxy_info.c_str());
            proxyInfo.lpszProxyBypass = NULL;

            if (!WinHttpSetOption(m_hSession, WINHTTP_OPTION_PROXY, (LPVOID)&proxyInfo, sizeof(proxyInfo)))
            {
                printf("Error %u in WinHttpSetOption. WINHTTP_OPTION_PROXY \n", GetLastError());
                return FALSE;
            }

            if (!m_proxy_user.empty() && !m_proxy_pass.empty())
            {
                if (!WinHttpSetOption(m_hConnect, WINHTTP_OPTION_PROXY_USERNAME, (LPVOID)m_proxy_user.c_str(), m_proxy_user.size() + 1))
                {
                    printf("Error %u in WinHttpSetOption. WINHTTP_OPTION_PROXY_USERNAME \n", GetLastError());
                    return FALSE;
                }
                if (!WinHttpSetOption(m_hConnect, WINHTTP_OPTION_PROXY_PASSWORD, (LPVOID)m_proxy_pass.c_str(), m_proxy_pass.size() + 1))
                {
                    printf("Error %u in WinHttpSetOption. WINHTTP_OPTION_PROXY_PASSWORD \n", GetLastError());
                    return FALSE;
                }
            }
        }

        m_hRequest = WinHttpOpenRequest(
            m_hConnect,
            m_request_method.c_str(),
            m_strPath.c_str(),
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            m_nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);

        if (!m_hRequest)
        {
            printf("Error %u in WinHttpOpenRequest.\n", GetLastError());
            return FALSE;
        }

        if (!m_basic_auth_user.empty() && !m_basic_auth_pass.empty())
        {
            if (!WinHttpSetCredentials(m_hRequest,
                                       WINHTTP_AUTH_TARGET_SERVER,
                                       WINHTTP_AUTH_SCHEME_BASIC,
                                       m_basic_auth_user.c_str(),
                                       m_basic_auth_pass.c_str(),
                                       NULL))
            {
                printf("Error %u in WinHttpSetCredentials \n", GetLastError());
                return FALSE;
            }
        }

        if (!m_followRedirects)
        {
            DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
            if (!WinHttpSetOption(m_hRequest, WINHTTP_OPTION_REDIRECT_POLICY, (LPVOID)&dwOption, sizeof(dwOption)))
            {
                printf("Error %u in WinHttpSetOption WINHTTP_OPTION_REDIRECT_POLICY .\n", GetLastError());
                return FALSE;
            }
        }

        std::string strHeaders;
        for (auto &k : m_RequestHeaders)
            strHeaders.append(k.first).append(": ").append(k.second).append("\n");
        m_RequestHeaders.clear();

        std::wstring wstrHeaders = static_cast<std::wstring>(textconv_helper::A2W_(strHeaders.c_str()));

        if (!WinHttpSendRequest(m_hRequest, wstrHeaders.c_str(), wstrHeaders.length(), lpPostData, dwsize, dwsize, 0))
        {
            printf("Error %u in WinHttpSendRequest.\n", GetLastError());
            return FALSE;
        }

        return TRUE;
    }

    std::vector<BYTE> CWinHttp::GetResponseBody()
    {
        DWORD dwReadSize = 0;
        DWORD dwDownloaded = 0;
        std::vector<BYTE> list;

        if (WinHttpReceiveResponse(m_hRequest, NULL))
        {
            do
            {
                dwReadSize = 0;
                if (!WinHttpQueryDataAvailable(m_hRequest, &dwReadSize))
                {
                    printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                    break;
                }

                if (!dwReadSize)
                    break;

                BYTE* lpReceivedData = new BYTE[dwReadSize];
                if (!lpReceivedData)
                {
                    printf("Out of memory\n");
                    break;
                }

                ZeroMemory(lpReceivedData, dwReadSize);

                if (!WinHttpReadData(m_hRequest, lpReceivedData, dwReadSize, &dwDownloaded))
                {
                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                }
                else
                {
                    for (size_t i = 0; i < dwReadSize; i++)
                        list.push_back(lpReceivedData[i]);
                }

                delete[] lpReceivedData;

                if (!dwDownloaded)
                    break;

            } while (dwReadSize > 0);
        }
        return list;
    }

    std::string CWinHttp::GetResponseHeaderValue(int dwInfoLevel, DWORD dwIndex)
    {
        std::wstring result;
        LPVOID lpOutBuffer = NULL;
        DWORD dwSize = 0;

    retry:

        if (!WinHttpQueryHeaders(m_hRequest, dwInfoLevel, WINHTTP_HEADER_NAME_BY_INDEX, lpOutBuffer, &dwSize, &dwIndex))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
                goto retry;
            }
        }

        if (lpOutBuffer)
        {
            result.assign(reinterpret_cast<LPWSTR>(lpOutBuffer));
            delete[] lpOutBuffer;
        }

        return std::string(textconv_helper::W2A_(result.c_str()));
    }

    std::string CWinHttp::GetResponseHeaders()
    {
        return GetResponseHeaderValue(WINHTTP_QUERY_RAW_HEADERS_CRLF);
    }

    std::string CWinHttp::GetResponseHeaderValue(const std::string& name)
    {
        std::wstring result;
        LPVOID lpOutBuffer = NULL;
        DWORD dwSize = 0;
        DWORD dwIndex = 0;

    retry:

        if (!WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_CUSTOM, textconv_helper::A2W_(name.c_str()), lpOutBuffer, &dwSize, &dwIndex))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
                goto retry;
            }
        }

        if (lpOutBuffer)
        {
            result.assign(reinterpret_cast<LPWSTR>(lpOutBuffer));
            delete[] lpOutBuffer;
        }

        return std::string(textconv_helper::W2A_(result.c_str()));
    }

    HeaderFields CWinHttp::GetHeaderFields()
    {
        std::string strHeaders = GetResponseHeaders();
        HeaderFields Headers;

        std::string pattern("(.+?): ([^;|^\\r|^\\n]+)");
        std::regex headers_regex(pattern);
        std::smatch results;

        auto _begin = std::sregex_iterator(strHeaders.begin(), strHeaders.end(), headers_regex);
        auto _end = std::sregex_iterator();

        for (auto i = _begin; i != _end; ++i)
        {
            std::smatch match = *i;
            if (match.size() == 3)
                Headers[match[1].str()] = match[2].str();
        }

        Headers["Set-Cookies"] = GetCookieStr();

        return Headers;
    }

    std::string CWinHttp::GetLocal()
    {
        return GetResponseHeaderValue(WINHTTP_QUERY_LOCATION);
    }

    DWORD CWinHttp::GetStatusCode()
    {
        DWORD dwStatusCode;
        DWORD dwSize = sizeof(DWORD);
        BOOL bRet = WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwStatusCode, &dwSize, NULL);
        return bRet ? dwStatusCode : 0;
    }

    std::string CWinHttp::GetCookieStr()
    {
        Cookies result = GetCookies();
        std::string cookies;

        for (auto& kv : result)
        {
            if (kv.second == "-" || kv.second == "''")
                continue;
            if (cookies.empty())
                cookies.append(kv.first).append("=").append(kv.second);
            else
                cookies.append("; ").append(kv.first).append("=").append(kv.second);
        }

        return cookies;
    }

    Cookies CWinHttp::GetCookies()
    {
        Cookies result;
        std::string strHeaders = GetResponseHeaders();
        std::string cookies_pattern("Set-Cookie: ([^\\r|^\\n]+)");

        std::regex cookies_regex(cookies_pattern);
        std::smatch results;

        auto cookies_begin = std::sregex_iterator(strHeaders.begin(), strHeaders.end(), cookies_regex);
        auto cookies_end = std::sregex_iterator();

        for (auto i = cookies_begin; i != cookies_end; ++i)
        {
            std::smatch match = *i;
            Cookies cookies = ParseCookies(match.str());
            for (auto& kv : cookies)
                result[kv.first] = kv.second;
        }

        return result;
    }

    void CWinHttp::SetBasicAuthenticator(const std::string& username, const std::string& password)
    {
        this->m_basic_auth_user = textconv_helper::A2W_(username.c_str());
        this->m_basic_auth_pass = textconv_helper::A2W_(password.c_str());
    }

    void CWinHttp::SetProxy(const std::string& proxy, const std::string& proxyName, const std::string& proxyPass)
    {
        this->m_proxy_info = textconv_helper::A2W_(proxy.c_str());
        this->m_proxy_user = textconv_helper::A2W_(proxyName.c_str());
        this->m_proxy_pass = textconv_helper::A2W_(proxyPass.c_str());
    }
}
