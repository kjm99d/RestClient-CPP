#include "textconv_helper.h"
#include <windows.h>
#include <vector>
#include <comdef.h>

namespace textconv_helper
{
    // CA2W_ class implementation
    CA2W_::CA2W_(LPCSTR pStr, UINT codePage) : m_pStr(pStr)
    {
        if (pStr)
        {
            int length = MultiByteToWideChar(codePage, 0, pStr, -1, NULL, 0) + 1;
            m_vWideArray.assign(length, L'\0');
            MultiByteToWideChar(codePage, 0, pStr, -1, &m_vWideArray[0], length);
        }
    }

    CA2W_::~CA2W_() {}

    CA2W_::operator LPCWSTR() { return m_pStr ? &m_vWideArray[0] : NULL; }

    CA2W_::operator LPOLESTR() { return m_pStr ? (LPOLESTR)&m_vWideArray[0] : (LPOLESTR)NULL; }

    // CW2A_ class implementation
    CW2A_::CW2A_(LPCWSTR pWStr, UINT codePage) : m_pWStr(pWStr)
    {
        int length = WideCharToMultiByte(codePage, 0, pWStr, -1, NULL, 0, NULL, NULL) + 1;
        m_vAnsiArray.assign(length, '\0');
        WideCharToMultiByte(codePage, 0, pWStr, -1, &m_vAnsiArray[0], length, NULL, NULL);
    }

    CW2A_::~CW2A_() { m_pWStr = 0; }

    CW2A_::operator LPCSTR() { return m_pWStr ? &m_vAnsiArray[0] : NULL; }

    // CW2W_ class implementation
    CW2W_::CW2W_(LPCWSTR pWStr) : m_pWStr(pWStr) {}

    CW2W_::operator LPCWSTR() { return const_cast<LPWSTR>(m_pWStr); }

    CW2W_::operator LPOLESTR() { return const_cast<LPOLESTR>(m_pWStr); }

    // CA2A_ class implementation
    CA2A_::CA2A_(LPCSTR pStr) : m_pStr(pStr) {}

    CA2A_::operator LPCSTR() { return (LPSTR)m_pStr; }

    // CW2BSTR_ class implementation
    CW2BSTR_::CW2BSTR_(LPCWSTR pWStr)
    {
        m_bstrString = ::SysAllocString(pWStr);
    }

    CW2BSTR_::~CW2BSTR_()
    {
        ::SysFreeString(m_bstrString);
    }

    CW2BSTR_::operator BSTR() { return m_bstrString; }

    // CA2BSTR_ class implementation
    CA2BSTR_::CA2BSTR_(LPCSTR pStr)
    {
        m_bstrString = ::SysAllocString(textconv_helper::CA2W_(pStr));
    }

    CA2BSTR_::~CA2BSTR_()
    {
        ::SysFreeString(m_bstrString);
    }

    CA2BSTR_::operator BSTR() { return m_bstrString; }
}
