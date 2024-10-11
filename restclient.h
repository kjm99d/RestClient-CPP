#ifndef RESTCLIENT_HPP
#define RESTCLIENT_HPP

#include <string>
#include <map>

#if defined _USE_WIHTTP_INTERFACE
#include "winhttp_helper.h"
using namespace winhttp;
#define HttpClient CWinHttp
#else
#include "winnet_helper.h"
using namespace winnet;
#define HttpClient CWinNet
#endif

namespace RestClient
{
	typedef struct _Response {
		int				code = 0;
		std::string		body;
		HeaderFields	headers;
		std::string		cookies;
		Cookies			Cookie;
		std::string get_header(const std::string& name);
		std::string get_cookie(const std::string& name);
	} Response;

	typedef struct _Request {
		int timeout;
		bool followRedirects = true;
		HeaderFields headers;
		struct {
			std::string proxy;
			std::string username;
			std::string password;
		} proxy;
		struct {
			std::string username;
			std::string password;
		} basicAuth;
		std::string set_userAgent(const std::string& value);
		std::string set_cookie(const std::string& value);
		std::string set_referer(const std::string& value);
	} Request;

	extern Request default_request;

	Response get(const std::string& url, Request* request = nullptr);
	Response post(const std::string& url, const std::string& content_type, const std::string& data, Request* request = nullptr);
	Response put(const std::string& url, const std::string& content_type, const std::string& data, Request* request = nullptr);
	Response del(const std::string& url, Request* request = nullptr);
	Response head(const std::string& url, Request* request = nullptr);
}

#endif // RESTCLIENT_HPP
