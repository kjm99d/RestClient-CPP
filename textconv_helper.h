/*
* Author: LowBoyTeam (https://github.com/LowBoyTeam)
* License: Code Project Open License
* Disclaimer: The software is provided "as-is". No claim of suitability, guarantee, or any warranty whatsoever is provided.
* Copyright (c) 2016-2017.
*/

#ifndef _TEXTCONV_HELPER_HPP_INCLUDED_
#define _TEXTCONV_HELPER_HPP_INCLUDED_

#include <windows.h>
#include <vector>

////////////////////////////////////////////////////////
// Classes and functions (typedefs) for text conversions
//
//  This section defines the following text conversions:
//  A2BSTR        ANSI  to BSTR
//  A2OLE        ANSI  to OLE
//  A2T            ANSI  to TCHAR
//  A2W            ANSI  to WCHAR
//  OLE2A        OLE   to ANSI
//  OLE2T        OLE   to TCHAR
//  OLE2W        OLE   to WCHAR
//  T2A            TCHAR to ANSI
//  T2BSTR        TCHAR to BSTR
//  T2OLE       TCHAR to OLE
//  T2W            TCHAR to WCHAR
//  W2A            WCHAR to ANSI
//  W2BSTR        WCHAR to BSTR
//  W2OLE        WCHAR to OLE
//  W2T            WCHAR to TCHAR

namespace textconv_helper
{
    // Forward declarations of our classes.
    class CA2W_;
    class CW2A_;
    class CW2W_;
    class CA2A_;
    class CW2BSTR_;
    class CA2BSTR_;

    // typedefs for the well known text conversions
    typedef CA2W_ A2W_;
    typedef CW2A_ W2A_;
    typedef CW2BSTR_ W2BSTR_;
    typedef CA2BSTR_ A2BSTR_;
    typedef CW2A_ BSTR2A_;
    typedef CW2W_ BSTR2W_;

#ifdef _UNICODE
    typedef CA2W_ A2T_;
    typedef CW2A_ T2A_;
    typedef CW2W_ T2W_;
    typedef CW2W_ W2T_;
    typedef CW2BSTR_ T2BSTR_;
    typedef BSTR2W_ BSTR2T_;
#else
    typedef CA2A_ A2T_;
    typedef CA2A_ T2A_;
    typedef CA2W_ T2W_;
    typedef CW2A_ W2T_;
    typedef CA2BSTR_ T2BSTR_;
    typedef BSTR2A_ BSTR2T_;
#endif

    typedef A2W_  A2OLE_;
    typedef T2W_  T2OLE_;
    typedef CW2W_ W2OLE_;
    typedef W2A_  OLE2A_;
    typedef W2T_  OLE2T_;
    typedef CW2W_ OLE2W_;

    // Class declarations
    class CA2W_
    {
    public:
        CA2W_(LPCSTR pStr, UINT codePage = CP_ACP);
        ~CA2W_();
        operator LPCWSTR();
        operator LPOLESTR();

    private:
        CA2W_(const CA2W_&);
        CA2W_& operator= (const CA2W_&);
        std::vector<wchar_t> m_vWideArray;
        LPCSTR m_pStr;
    };

    class CW2A_
    {
    public:
        CW2A_(LPCWSTR pWStr, UINT codePage = CP_ACP);
        ~CW2A_();
        operator LPCSTR();

    private:
        CW2A_(const CW2A_&);
        CW2A_& operator= (const CW2A_&);
        std::vector<char> m_vAnsiArray;
        LPCWSTR m_pWStr;
    };

    class CW2W_
    {
    public:
        CW2W_(LPCWSTR pWStr);
        operator LPCWSTR();
        operator LPOLESTR();

    private:
        CW2W_(const CW2W_&);
        CW2W_& operator= (const CW2W_&);
        LPCWSTR m_pWStr;
    };

    class CA2A_
    {
    public:
        CA2A_(LPCSTR pStr);
        operator LPCSTR();

    private:
        CA2A_(const CA2A_&);
        CA2A_& operator= (const CA2A_&);
        LPCSTR m_pStr;
    };

    class CW2BSTR_
    {
    public:
        CW2BSTR_(LPCWSTR pWStr);
        ~CW2BSTR_();
        operator BSTR();

    private:
        CW2BSTR_(const CW2BSTR_&);
        CW2BSTR_& operator= (const CW2BSTR_&);
        BSTR m_bstrString;
    };

    class CA2BSTR_
    {
    public:
        CA2BSTR_(LPCSTR pStr);
        ~CA2BSTR_();
        operator BSTR();

    private:
        CA2BSTR_(const CA2BSTR_&);
        CA2BSTR_& operator= (const CA2BSTR_&);
        BSTR m_bstrString;
    };
}

#endif // _TEXTCONV_HELPER_HPP_INCLUDED_
