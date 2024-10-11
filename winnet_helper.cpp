#include "winnet_helper.h"

namespace winnet
{
    CWinNet::CWinNet() : m_hSession(NULL), m_hConnect(NULL), m_hRequest(NULL), m_followRedirects(FALSE), m_nPort(0), m_nScheme(INTERNET_SCHEME::INTERNET_SCHEME_DEFAULT)
    {
    }

    CWinNet::~CWinNet()
    {
        if (m_hRequest)
        {
            InternetCloseHandle(m_hRequest);
            m_hRequest = NULL;
        }
        if (m_hConnect)
        {
            InternetCloseHandle(m_hConnect);
            m_hConnect = NULL;
        }
        if (m_hSession)
        {
            InternetCloseHandle(m_hSession);
            m_hSession = NULL;
        }
    }

    Cookies CWinNet::ParseCookies(const std::string& cookies)
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

    BOOL CWinNet::Open(const std::string& Url, const std::string& method)
    {
        m_hSession = InternetOpenA("winnet http client v1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
        if (!m_hSession)
        {
            printf("Error %u in InternetOpenA.\n", GetLastError());
            return FALSE;
        }

        URL_COMPONENTSA urlComp;
        ZeroMemory(&urlComp, sizeof(urlComp));
        urlComp.dwStructSize = sizeof(urlComp);
        urlComp.dwHostNameLength = 1;
        urlComp.dwUserNameLength = 1;
        urlComp.dwPasswordLength = 1;
        urlComp.dwUrlPathLength = 1;
        urlComp.dwExtraInfoLength = 1;

        if (!InternetCrackUrlA(Url.c_str(), 0, 0, &urlComp))
        {
            printf("Error %u in InternetCrackUrlA.\n", GetLastError());
            return FALSE;
        }

        m_strHost = std::string(urlComp.lpszHostName, urlComp.dwHostNameLength);
        m_strPath = std::string(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
        m_strExt = std::string(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
        m_nScheme = urlComp.nScheme;
        m_nPort = urlComp.nPort;
        m_strPath.append(m_strExt);
        m_request_method = method;

        return TRUE;
    }

    BOOL CWinNet::SetTimeout(DWORD dwTimeOut)
    {
        assert(m_hSession != NULL);

        if (InternetSetOptionA(m_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut))
            && InternetSetOptionA(m_hSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut))
            && InternetSetOptionA(m_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut)))
        {
            return TRUE;
        }
        return FALSE;
    }

    void CWinNet::SetRequestHeader(const std::string& key, const std::string& value)
    {
        m_RequestHeaders[key] = value;
    }

    void CWinNet::SetHeaders(HeaderFields headers)
    {
        m_RequestHeaders = headers;
    }

    void CWinNet::SetUserAgent(const std::string& userAgent)
    {
        SetRequestHeader("User-Agent", userAgent);
    }

    void CWinNet::SetCookie(const std::string& cookies)
    {
        SetRequestHeader("Cookie", cookies);
    }

    void CWinNet::SetReferer(const std::string& referer)
    {
        SetRequestHeader("Referer", referer);
    }

    void CWinNet::FollowRedirects(BOOL follow)
    {
        m_followRedirects = follow;
    }

    BOOL CWinNet::Send(LPVOID lpPostData, DWORD dwsize)
    {
        assert(m_hSession != NULL);

        m_hConnect = InternetConnectA(m_hSession, m_strHost.c_str(), m_nPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
        if (!m_hConnect)
        {
            printf("Error %u in InternetConnectA.\n", GetLastError());
            return FALSE;
        }

        if (!m_basic_auth_user.empty() && !m_basic_auth_pass.empty())
        {
            if (!InternetSetOptionA(m_hConnect, INTERNET_OPTION_USERNAME, (LPVOID)m_basic_auth_user.c_str(), m_basic_auth_user.size() + 1))
            {
                printf("Error %u in InternetSetOptionA. INTERNET_OPTION_USERNAME \n", GetLastError());
                return FALSE;
            }
            if (!InternetSetOptionA(m_hConnect, INTERNET_OPTION_PASSWORD, (LPVOID)m_basic_auth_pass.c_str(), m_basic_auth_pass.size() + 1))
            {
                printf("Error %u in InternetSetOptionA. INTERNET_OPTION_PASSWORD \n", GetLastError());
                return FALSE;
            }
        }

        if (!m_proxy_info.empty())
        {
            INTERNET_PROXY_INFO proxyInfo;
            proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
            proxyInfo.lpszProxy = reinterpret_cast<LPCTSTR>(m_proxy_info.c_str());
            proxyInfo.lpszProxyBypass = NULL;

            if (!InternetSetOptionA(m_hSession, INTERNET_OPTION_PROXY, (LPVOID)&proxyInfo, sizeof(proxyInfo)))
            {
                printf("Error %u in InternetSetOptionA. INTERNET_OPTION_PROXY \n", GetLastError());
                return FALSE;
            }

            if (!m_proxy_user.empty() && !m_proxy_pass.empty())
            {
                if (!InternetSetOptionA(m_hConnect, INTERNET_OPTION_PROXY_USERNAME, (LPVOID)m_proxy_user.c_str(), m_proxy_user.size() + 1))
                {
                    printf("Error %u in InternetSetOptionA. INTERNET_OPTION_PROXY_USERNAME \n", GetLastError());
                    return FALSE;
                }
                if (!InternetSetOptionA(m_hConnect, INTERNET_OPTION_PROXY_PASSWORD, (LPVOID)m_proxy_pass.c_str(), m_proxy_pass.size() + 1))
                {
                    printf("Error %u in InternetSetOptionA. INTERNET_OPTION_PROXY_PASSWORD \n", GetLastError());
                    return FALSE;
                }
            }
        }

        DWORD dwFlags = INTERNET_FLAG_HYPERLINK
            | INTERNET_FLAG_KEEP_CONNECTION
            | INTERNET_FLAG_NO_UI
            | INTERNET_FLAG_RESYNCHRONIZE
            | INTERNET_FLAG_NO_CACHE_WRITE
            | INTERNET_FLAG_PRAGMA_NOCACHE
            | INTERNET_FLAG_RELOAD
            | INTERNET_COOKIE_THIRD_PARTY
            | INTERNET_FLAG_NO_COOKIES
            | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
            | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
            | INTERNET_FLAG_DONT_CACHE;

        if (m_nScheme == INTERNET_SCHEME_HTTPS)
            dwFlags = dwFlags | INTERNET_FLAG_SECURE;
        else
            dwFlags = dwFlags | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;

        if (m_followRedirects == FALSE)
            dwFlags = dwFlags | INTERNET_FLAG_NO_AUTO_REDIRECT;

        m_hRequest = HttpOpenRequestA(m_hConnect, m_request_method.c_str(), m_strPath.c_str(), "HTTP/1.1", NULL, NULL, dwFlags, NULL);
        if (!m_hRequest)
        {
            printf("Error %u in HttpOpenRequestA.\n", GetLastError());
            return FALSE;
        }

        DWORD dwGzip = 1;
        InternetSetOptionW(m_hRequest, INTERNET_OPTION_HTTP_DECODING, &dwGzip, sizeof(dwGzip));

        std::string strHeaders;
        for (auto& k : m_RequestHeaders)
            strHeaders.append(k.first).append(": ").append(k.second).append("\n");

        if (!HttpAddRequestHeadersA(m_hRequest, strHeaders.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE))
        {
            printf("Error %u in HttpAddRequestHeadersA.\n", GetLastError());
            return FALSE;
        }

        m_RequestHeaders.clear();

        if (!HttpSendRequestA(m_hRequest, NULL, 0, lpPostData, dwsize))
        {
            printf("Error %u in HttpSendRequestA.\n", GetLastError());
            return FALSE;
        }

        return TRUE;
    }

    std::vector<BYTE> CWinNet::GetResponseBody()
    {
        assert(m_hRequest != NULL);

        DWORD dwReadSize = 0;
        DWORD dwDownloaded = 0;
        std::vector<BYTE> list;

        do
        {
            dwReadSize = 0;
            if (!InternetQueryDataAvailable(m_hRequest, &dwReadSize, 0, 0))
            {
                printf("Error %u in InternetQueryDataAvailable.\n", GetLastError());
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

            if (!InternetReadFile(m_hRequest, lpReceivedData, dwReadSize, &dwDownloaded)) {
                printf("Error %u in InternetReadFile.\n", GetLastError());
            }
            else {
                for (size_t i = 0; i < dwReadSize; i++)
                    list.push_back(lpReceivedData[i]);
            }

            delete[] lpReceivedData;

            if (!dwDownloaded)
                break;

        } while (dwReadSize > 0);

        return list;
    }

    std::string CWinNet::GetResponseHeaders()
    {
        return GetResponseHeaderValue(HTTP_QUERY_RAW_HEADERS_CRLF);
    }

    std::string CWinNet::GetResponseHeaderValue(const std::string& name)
    {
        assert(m_hRequest != NULL);

        std::string result;
        DWORD dwSize = name.length() + 1;
        LPVOID lpOutBuffer = new char[dwSize];

        StringCchPrintfA((LPSTR)lpOutBuffer, dwSize, name.c_str());

    retry:
        if (!HttpQueryInfoA(m_hRequest, HTTP_QUERY_CUSTOM, (LPVOID)lpOutBuffer, &dwSize, NULL))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                delete[] lpOutBuffer;
                lpOutBuffer = NULL;

                lpOutBuffer = new char[dwSize];

                StringCchPrintfA((LPSTR)lpOutBuffer, dwSize, name.c_str());

                goto retry;
            }
            else
            {
                delete[] lpOutBuffer;
                lpOutBuffer = NULL;
            }
        }

        if (lpOutBuffer)
            result.assign(reinterpret_cast<LPCSTR>(lpOutBuffer));

        return result;
    }

    std::string CWinNet::GetResponseHeaderValue(int dwInfoLevel, DWORD dwIndex)
    {
        assert(m_hRequest != NULL);

        std::string result;
        LPVOID lpOutBuffer = NULL;
        DWORD dwSize = 0;

        while (FALSE == HttpQueryInfoA(m_hRequest, dwInfoLevel, reinterpret_cast<LPVOID>(lpOutBuffer), &dwSize, &dwIndex))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                lpOutBuffer = new char[dwSize];
            }
        }

        if (lpOutBuffer)
        {
            result.assign(reinterpret_cast<LPCSTR>(lpOutBuffer));
        }

        if (lpOutBuffer)
        {
            delete[] lpOutBuffer;
            lpOutBuffer = NULL;
        }

        return result;
    }

    HeaderFields CWinNet::GetHeaderFields()
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

    std::string CWinNet::GetLocal()
    {
        return GetResponseHeaderValue(HTTP_QUERY_LOCATION);
    }

    DWORD CWinNet::GetStatusCode()
    {
        assert(m_hRequest != NULL);

        DWORD dwStatusCode = 0;
        DWORD dwSize = sizeof(DWORD);
        HttpQueryInfoA(m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwSize, NULL);
        return dwStatusCode;
    }

    std::string CWinNet::GetCookieStr()
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

    Cookies CWinNet::GetCookies()
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

    void CWinNet::SetBasicAuthenticator(const std::string& username, const std::string& password)
    {
        this->m_basic_auth_user = username;
        this->m_basic_auth_pass = password;
    }

    void CWinNet::SetProxy(const std::string& proxy, const std::string& proxyName, const std::string& proxyPass)
    {
        this->m_proxy_info = proxy;
        this->m_proxy_user = proxyName;
        this->m_proxy_pass = proxyPass;
    }
}
