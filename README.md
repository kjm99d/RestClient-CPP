# [REST client for C++](https://hpsocket.github.io/restclient-cpp/)
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](http://opensource.org/licenses/MIT)


## Intro
이 구성 요소는 Windows (winhttp/winnet) API를 래핑한 C++용 간단하고 편리한 REST 클라이언트입니다.    
프록시, HTTP BasicAuth 인증을 지원합니다.    

이 프로젝트는 아래 프로젝트에 대한 위험한 코드에 대한 패치를 진행한 후 공유되는 소스코드입니다.    
https://github.com/LFYG/restclient-cpp

## Refactoring
- 생성자 초기화 리스트 ( 단순 워닝 제거 )
- Incremental Jump ( goto 문으로 인한 위로 점프 시, Scope Unsafe 로 인한 Stack 흐름에 대한 영향이 있을 수 있음 )

## Env
C++17

## Help
restclient-cpp는 REST 엔드포인트와 상호작용할 수 있는 두 가지 방법을 제공합니다.    
간단한 방법이 있어서 설정이 필요하지 않습니다. 따라서 간단한 HTTP 호출 이상의 기능이 필요하다면,    
고급 사용법을 참고하시기 바랍니다. 또한 textconv_helper라는 인코딩 변환 클래스도 제공합니다.    
사용 방법:    
```cpp
//response utf8
std::wstring body = textconv_helper::A2W_(response.body.c_str(), CP_UTF8);
wprintf(L"body = %s \n", body.c_str());
```
기본값은 Winnet Api이고 WinHttp Api로 전환합니다.
```cpp
#define _USE_WIHTTP_INTERFACE
```
### Easy to Use

```cpp
#include "restclient-cpp/restclient.h"

RestClient::Response r = RestClient::get("http://url.com")
RestClient::Response r = RestClient::post("http://url.com/post", "text/json", "{\"foo\": \"bla\"}")
RestClient::Response r = RestClient::put("http://url.com/put", "text/json", "{\"foo\": \"bla\"}")
RestClient::Response r = RestClient::del("http://url.com/delete")
RestClient::Response r = RestClient::head("http://url.com")
```

The response type is RestClient :: Response    
Attribute：    

```cpp
RestClient::Response.code // HTTP response code
RestClient::Response.body // HTTP response body
RestClient::Response.headers // HTTP response headers,The type is map
RestClient::Response.cookies // HTTP response cookies,The type is string
RestClient::Response.cookie // HTTP response cookies,The type is map
RestClient::Response.get_header //It can be used to get the specified header value
RestClient::Response.get_cookie //It can be used to get the specified cookie value
```

### Advanced Usage

#### GET simple
```cpp
RestClient::Request request;

request.timeout     	        = 3000;
request.followRedirects     	= false;
request.headers["User-Agent"]	= "Mozilla/5.0";
request.headers["Cookies"]	  = "name=value;";

RestClient::Response response	= RestClient::get("http://www.baidu.com", &request);

printf("%s \n", response.body.c_str());
printf("%d \n", response.code);
printf("%s \n", response.cookies.c_str());
printf("%s %s \n", response.Cookie["BAIDUID"].c_str(), response.get_cookie("BAIDUID").c_str());
printf("%s %s \n", response.headers["Location"].c_str(), response.get_header("Location").c_str());
printf("%s %s \n", response.headers["Content-Type"].c_str(), response.get_header("Content-Type").c_str());
printf("%s %s \n", response.headers["Content-Length"].c_str(), response.get_header("Content-Length").c_str());
```

#### POST simple
```cpp
RestClient::Request request;

request.timeout	= 3000;
request.followRedirects	= false;
request.headers["User-Agent"]	= "Mozilla/5.0";
request.headers["Cookies"] = "name=value;";

RestClient::Response response = RestClient::post("http://www.baidu.com/post.php",
"text/json", "{\"foo\": \"bla\"}",&request);

printf("%s \n", response.body.c_str());
printf("%d \n", response.code);
printf("%s \n", response.cookies.c_str());
printf("%s \n", response.headers["Content-Type"].c_str());
printf("%s \n", response.headers["Content-Length"].c_str());
```

#### Proxy simple

```cpp
RestClient::Request request;

request.proxy.proxy = "http=115.29.2.139:80";
request.proxy.username = "";
request.proxy.password = "";

//request.basicAuth.username = "your username";
//request.basicAuth.password = "your password";

request.timeout	= 3000;
request.followRedirects	= false;
request.headers["User-Agent"]	= "Mozilla/5.0";
request.headers["Cookie"] = "name=value;";

RestClient::Response response	= RestClient::get("http://ipip.yy.com/get_ip_info.php", &request);

//response utf8
std::wstring body = textconv_helper::A2W_(response.body.c_str(), CP_UTF8);
wprintf(L"body = %s \n", body.c_str());

printf("code = %d \n", response.code);
printf("cookie = %s \n", response.cookies.c_str());
printf("%s %s \n", response.Cookie["BAIDUID"].c_str(), response.get_cookie("BAIDUID").c_str());
printf("%s %s \n", response.headers["Location"].c_str(), response.get_header("Location").c_str());
printf("%s %s \n", response.headers["Content-Type"].c_str(), response.get_header("Content-Type").c_str());
printf("%s %s \n", response.headers["Content-Length"].c_str(), response.get_header("Content-Length").c_str());

```
