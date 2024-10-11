#include "RestClient.h"

namespace RestClient
{
	Request default_request = { 3000, true, {std::make_pair("User-Agent", "restclient for cpp v1.0")} };

	std::string Response::get_header(const std::string& name) {
		return this->headers[name];
	}

	std::string Response::get_cookie(const std::string& name) {
		return this->Cookie[name];
	}

	std::string Request::set_userAgent(const std::string& value) {
		return this->headers["User-Agent"] = value;
	}

	std::string Request::set_cookie(const std::string& value) {
		return this->headers["Cookie"] = value;
	}

	std::string Request::set_referer(const std::string& value) {
		return this->headers["Referer"] = value;
	}

	Response get(const std::string& url, Request* request) {
		Response ret;
		HttpClient http;

		if (http.Open(url, HTTP_METHOD_GET)) {
			if (request == nullptr)
				request = &default_request;

			if (request->timeout > 0)
				http.SetTimeout(request->timeout);

			http.FollowRedirects(request->followRedirects);

			for (auto& kv : request->headers)
				http.SetRequestHeader(kv.first.c_str(), kv.second.c_str());

			if (!request->proxy.proxy.empty())
				http.SetProxy(request->proxy.proxy, request->proxy.username, request->proxy.password);

			if (!request->basicAuth.username.empty() && !request->basicAuth.password.empty())
				http.SetBasicAuthenticator(request->basicAuth.username, request->basicAuth.password);

			if (http.Send()) {
				std::string result;
				std::vector<BYTE> response = http.GetResponseBody();
				result.insert(result.begin(), response.begin(), response.end());
				ret.body = result;
				ret.code = http.GetStatusCode();
				ret.cookies = http.GetCookieStr();
				ret.Cookie = http.GetCookies();
				ret.headers = http.GetHeaderFields();
			}
		}
		return ret;
	}

	Response post(const std::string& url, const std::string& content_type, const std::string& data, Request* request) {
		Response ret;
		HttpClient http;

		if (http.Open(url, HTTP_METHOD_POST)) {
			if (request == nullptr)
				request = &default_request;

			http.FollowRedirects(request->followRedirects);
			http.SetRequestHeader("Content-Type", content_type);

			if (request->timeout > 0)
				http.SetTimeout(request->timeout);

			for (auto& kv : request->headers)
				http.SetRequestHeader(kv.first.c_str(), kv.second.c_str());

			if (!request->proxy.proxy.empty())
				http.SetProxy(request->proxy.proxy, request->proxy.username, request->proxy.password);

			if (!request->basicAuth.username.empty() && !request->basicAuth.password.empty())
				http.SetBasicAuthenticator(request->basicAuth.username, request->basicAuth.password);

			if (http.Send((LPVOID)data.c_str(), data.length())) {
				std::string result;
				std::vector<BYTE> response = http.GetResponseBody();
				result.insert(result.begin(), response.begin(), response.end());
				ret.body = result;
				ret.code = http.GetStatusCode();
				ret.cookies = http.GetCookieStr();
				ret.Cookie = http.GetCookies();
				ret.headers = http.GetHeaderFields();
			}
		}
		return ret;
	}

	Response put(const std::string& url, const std::string& content_type, const std::string& data, Request* request) {
		Response ret;
		HttpClient http;

		if (http.Open(url, HTTP_METHOD_PUT)) {
			if (request == nullptr)
				request = &default_request;

			http.FollowRedirects(request->followRedirects);
			http.SetRequestHeader("Content-Type", content_type);

			if (request->timeout > 0)
				http.SetTimeout(request->timeout);

			for (auto& kv : request->headers)
				http.SetRequestHeader(kv.first.c_str(), kv.second.c_str());

			if (!request->proxy.proxy.empty())
				http.SetProxy(request->proxy.proxy, request->proxy.username, request->proxy.password);

			if (!request->basicAuth.username.empty() && !request->basicAuth.password.empty())
				http.SetBasicAuthenticator(request->basicAuth.username, request->basicAuth.password);

			if (http.Send((LPVOID)data.c_str(), data.length())) {
				std::string result;
				std::vector<BYTE> response = http.GetResponseBody();
				result.insert(result.begin(), response.begin(), response.end());
				ret.body = result;
				ret.code = http.GetStatusCode();
				ret.cookies = http.GetCookieStr();
				ret.Cookie = http.GetCookies();
				ret.headers = http.GetHeaderFields();
			}
		}
		return ret;
	}

	Response del(const std::string& url, Request* request) {
		Response ret;
		HttpClient http;

		if (http.Open(url, HTTP_METHOD_DELETE)) {
			if (request == nullptr)
				request = &default_request;

			if (request->timeout > 0)
				http.SetTimeout(request->timeout);

			http.FollowRedirects(request->followRedirects);

			for (auto& kv : request->headers)
				http.SetRequestHeader(kv.first.c_str(), kv.second.c_str());

			if (!request->proxy.proxy.empty())
				http.SetProxy(request->proxy.proxy, request->proxy.username, request->proxy.password);

			if (!request->basicAuth.username.empty() && !request->basicAuth.password.empty())
				http.SetBasicAuthenticator(request->basicAuth.username, request->basicAuth.password);

			if (http.Send()) {
				std::string result;
				std::vector<BYTE> response = http.GetResponseBody();
				result.insert(result.begin(), response.begin(), response.end());
				ret.body = result;
				ret.code = http.GetStatusCode();
				ret.cookies = http.GetCookieStr();
				ret.Cookie = http.GetCookies();
				ret.headers = http.GetHeaderFields();
			}
		}
		return ret;
	}

	Response head(const std::string& url, Request* request) {
		Response ret;
		HttpClient http;

		if (http.Open(url, HTTP_METHOD_HEAD)) {
			if (request == nullptr)
				request = &default_request;

			if (request->timeout > 0)
				http.SetTimeout(request->timeout);

			http.FollowRedirects(request->followRedirects);

			for (auto& kv : request->headers)
				http.SetRequestHeader(kv.first.c_str(), kv.second.c_str());

			if (!request->proxy.proxy.empty())
				http.SetProxy(request->proxy.proxy, request->proxy.username, request->proxy.password);

			if (!request->basicAuth.username.empty() && !request->basicAuth.password.empty())
				http.SetBasicAuthenticator(request->basicAuth.username, request->basicAuth.password);

			if (http.Send()) {
				std::string result;
				std::vector<BYTE> response = http.GetResponseBody();
				result.insert(result.begin(), response.begin(), response.end());
				ret.body = result;
				ret.code = http.GetStatusCode();
				ret.cookies = http.GetCookieStr();
				ret.Cookie = http.GetCookies();
				ret.headers = http.GetHeaderFields();
			}
		}
		return ret;
	}
}
