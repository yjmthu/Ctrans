#include <yjson.h>

#include "picosha2.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "yencode.h"

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

#ifdef _WIN32
#include <utf8_console.h>
#endif

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#ifdef _WIN32
#include <wininet.h>
#endif

std::string uuid1()
{
    using boost::uuids::detail::sha1;
    using std::cout;
    using std::hex;
    using std::ostringstream;
    sha1 sha;

    char szMsg[] = "a short message";        //用于摘要的消息
    sha.process_byte(0x10);                  //处理一个字节
    sha.process_bytes(szMsg, strlen(szMsg)); //处理多个字节
    sha.process_block(szMsg, szMsg + strlen(szMsg));
    unsigned int digest[5];                  //摘要的返回值
    sha.get_digest(digest);
    ostringstream ostr;
    for (int i = 0; i< 5; ++i)
        ostr << hex << digest[i];            //16进制输出
    return ostr.str();
}

std::string getTimeStamp() {
    std::chrono::seconds ms = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(ms.count());
}

std::string truncate(const std::string& q) {
    if (q.empty())
        return std::string();
    return  q.size() <= 20 ? q : (q.substr(0, 10) + std::to_string(q.size()) + q.substr(q.size()-10, 10));
}

#ifdef _WIN32

typedef std::conditional<std::is_same<TCHAR, char>::value, std::string, std::wstring>::type MString;
std::wstring GbkToUnicode(const std::string& strGbk) //返回值是wstring
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
    std::wstring strTemp(len, 0);
	MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, &strTemp.front(), len);
    if (!strTemp.back()) strTemp.pop_back();
	return strTemp;
}

std::string UnicodeToGbk (const std::wstring& strUnicode)//参数是wstring
{
	int len = WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
    std::string strTemp(len, 0);
	WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, &strTemp.front(), len, NULL, NULL);
    if (!strTemp.back()) strTemp.pop_back();
	return strTemp;
}

std::wstring Utf8ToUnicode(const std::string& strGbk) //返回值是wstring
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strGbk.c_str(), -1, NULL, 0);
    std::wstring strTemp(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, strGbk.c_str(), -1, &strTemp.front(), len);
    if (!strTemp.back()) strTemp.pop_back();
	return strTemp;
}

std::string UnicodeToUtf8 (const std::wstring& strUnicode)//参数是wstring
{
	int len = WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
    std::string strTemp(len, 0);
	WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, &strTemp.front(), len, NULL, NULL);
    if (!strTemp.back()) strTemp.pop_back();
	return strTemp;
}
#endif

class Translation {
private:
    const char APP_KEY   [17]    { "0b5f90d14623b917" };
    const char APP_SECRET[33]    { "8X1HcIvXXETCRf2smIbey8AGJ2xGRyK3" };
    YJson m_scJson;
    const std::string m_sCuttingLine;
public:
    Translation(): m_scJson(YJson::Object), m_sCuttingLine("\n " + std::string(10, '-') + " \n\n") {
        m_scJson.append("auto", "from");
        m_scJson.append("auto", "to");
        m_scJson.append(APP_KEY, "appKey");
        m_scJson.append("v3", "signType");
        m_scJson.append("1646156135", "curtime");
        m_scJson.append(YJson::Null, "q");
        m_scJson.append(YJson::Null, "salt");
        m_scJson.append(YJson::Null, "sign");
    }
    ~Translation() {}
//     static bool getTransCodeJson(const std::string& url, std::string& outcome)
//     {
// #if 0
//         HINTERNET hSession = InternetOpen(TEXT("Microsoft Edge"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
//         if (hSession)
//         {
//             HINTERNET hURL = InternetOpenUrlA(hSession, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
//             if (hURL)
//             {
//                 char buffer[1025];
//                 DWORD dwRecv = 0;
//                 buffer[1024] = 0;
//                 do {
//                     memset(buffer, 0, 1024);
//                     InternetReadFile(hURL, buffer, 1024, &dwRecv);
//                     if (dwRecv) {
//                         buffer[dwRecv] = 0;
//                         outcome.append(buffer);
//                     } else {
//                         break;
//                     }
//                 } while (true);
//                 InternetCloseHandle(hURL);
//             }

//             InternetCloseHandle(hSession);
//             hSession = NULL;
//         }
// #endif
//         return !outcome.empty();
//     }
//     static bool getTransCodeXml(const std::string& url, std::stringstream& strs)
//     {
//         HINTERNET hSession = InternetOpen(TEXT("Microsoft Edge"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
//         if (hSession)
//         {
//             HINTERNET hURL = InternetOpenUrlA(hSession, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
//             if (hURL)
//             {
//                 char buffer[1025];
//                 DWORD dwRecv = 0;
//                 buffer[1024] = 0;
//                 do {
//                     memset(buffer, 0, 1024);
//                     InternetReadFile(hURL, buffer, 1024, &dwRecv);
//                     if (dwRecv) {
//                         buffer[dwRecv] = 0;
//                         strs << buffer;
//                     } else {
//                         break;
//                     }
//                 } while (true);
//                 InternetCloseHandle(hURL);
//             }

//             InternetCloseHandle(hSession);
//             hSession = NULL;
//         }
//         return strs.bad();
//     }

    inline void printCuttingLine() {
        std::cout << m_sCuttingLine;
    }

    std::string getResultYoudao(const std::string& text) {
        std::string curtime = getTimeStamp();
        m_scJson["curtime"].setText(curtime);
        std::string salt = uuid1();  //"d818bc30-99df-11ec-9e18-1cbfc0a98096";
        std::string signStr = APP_KEY + truncate(text) + salt + curtime + APP_SECRET;
        std::array<unsigned char, picosha2::k_digest_size> hash;
        picosha2::hash256(signStr.begin(), signStr.end(), hash.begin(), hash.end());
        std::string sign = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
        m_scJson["q"].setText(text);
        m_scJson["salt"].setText(salt);
        m_scJson["sign"].setText(sign);

        httplib::Client cli("http://openapi.youdao.com");
        std::string && data = m_scJson.urlEncode("/api/?");
        return cli.Get(data.c_str())->body;
    }

    std::string getResultIciba(const std::string& encode_text) {
        httplib::Client cli("https://dict-co.iciba.com");
        std::string&& data = "/api/dictionary.php?key=D191EBD014295E913574E1EAF8E06666&w="+encode_text;
        return cli.Get(data.c_str())->body;
    }

    std::string getResultDictionary(const std::string& encode_text) {
#if 0X0
        constexpr char api[] = "https://www.dictionaryapi.com/api/v1/references/collegiate/xml/";
        constexpr char key[] = "?key=82c5d495-ccf0-4e72-9051-5089e85c2975";
#endif
        // constexpr char api[] = "https://api.dictionaryapi.dev/api/v2/entries/en/";
        httplib::Client cli("https://api.dictionaryapi.dev");
        std::string&& data = "/api/v2/entries/en/" + encode_text;
        return cli.Get(data.c_str())->body;
    }

    void printResultYoudao(const std::string& text, const std::string& jsonStr) {
        YJson js(jsonStr);
        if (js.getType() != YJson::Object) {
            std::cout << jsonStr;
            return;
        }
        if (strcmp(js["errorCode"].getValueString(), "0")) {
            std::cout << text << "  ~  \033[33mfanyi.youdao.com\033[0m\n\n" << std::string("errorCode: ") << js["errorCode"].getValueString() << std::endl;
            return;
        }
        // std::string res(text + "  " + js["translation"].getChild()->getValueString() + "\n");
        // std::cout << jsonStr << std::endl;
        YJson *ptr = js.find("basic");
        if (ptr && !ptr->empty()) {
            const YJson* us = ptr->find("us-phonetic");
            const YJson* uk = ptr->find("uk-phonetic");
            const YJson* zh = ptr->find("phonetic");
            if (us && uk) {
                std::cout << text << "  英[ " << uk->getValueString() << " ]  美[ " << us->getValueString() << " ]  ~  \033[33mfanyi.youdao.com\033[0m\n";
            } else if (zh) {
                std::cout << text << "  中[ " << zh->getValueString() << " ]  ~  \033[33mfanyi.youdao.com\033[0m\n";
            } else {
                std::cout << text << "  ~  \033[33mfanyi.youdao.com\033[0m\n";
            }
            YJson* wfs = js["basic"].find("wfs"), *wf;
            if (wfs && !wfs->empty()) {
                wf = wfs->getChild();
                while (wf)
                {
                    YJson* temp = wf->getChild();
                    if (!temp || strcmp(temp->getKeyString(), "wf")) break;
                    std::cout << "\n+ " << temp->find("name")->getValueString() << "：" << temp->find("value")->getValueString();
                    wf = wf->getNext();
                }
                std::cout << std::endl;                
            }
            ptr = ptr->find("explains")->getChild();
            while (ptr) {
                std::cout << std::endl << "- " << ptr->getValueString();
                ptr = ptr->getNext();
            }
            std::cout << std::endl;
        } else if ((ptr = js.find("translation"))) {
            if ((ptr = ptr->getChild())) {
                std::cout << text << "  ~  \033[33mfanyi.youdao.com\033[0m\n\n- ";
                do {
                    std::cout << ptr->getValueString() << "; ";
                } while ((ptr = ptr->getNext()));
                std::cout << std::endl;
            }
        }
        int i = 1;
        ptr = js.find("web");
        if (ptr && (ptr = ptr->getChild())) {
            while (ptr && !ptr->empty()) {
                auto ptr1 = ptr->find("value")->getChild();
                std::cout << std::endl << i++ << ". " << ptr->find("key")->getValueString() << std::endl << "    ";
                while (ptr1)
                {
                    std::cout << ptr1->getValueString() << "; ";
                    ptr1 = ptr1->getNext();
                }
                ptr = ptr->getNext();
            }
            std::cout << std::endl;
        }
    }

    void printResultIciba(const std::string& text, std::string& str) {
        std::stringstream strs(str);
        using boost::property_tree::ptree;
        ptree pt;
        read_xml(strs, pt);
        int j = 1;
        const auto& dict = pt.get_child("dict");
        for (auto iter1 = dict.begin(); iter1 !=  dict.end(); ++iter1) {
            if (iter1->first == "key") {
                std::cout << iter1->second.data() << "  ";
                if (++iter1 != dict.end()) {
                    if (iter1->first == "ps") {
                        std::cout << "英[ " << iter1++->second.data() << " ]  ";
                        std::cout << "美[ " << (++iter1)->second.data() << " ]  ~  \033[33miciba.com\033[0m\n\n";
                        continue;
                    } else {
                        std::cout <<  "~  \033[33miciba.com\033[0m\n\n";
                    }
                }
                --iter1;
            } else if (iter1->first == "pos") {
                const auto& str = iter1->second.data();
                if (str.empty()) {
                    continue;
                }
                std::cout << "- " << str << ' ';
            } else if (iter1->first == "acceptation") {
                const auto& data = iter1->second.data();
                if (data.size() >= 3 && !strncmp(&data.front(), "；", 3)) {
                    continue;
                }
                std::cout << data;
                if (++iter1 != dict.end()) {
                    if (iter1->first != "pos") {
                        std::cout << std::endl;
                    }
                }
                --iter1;
            } else if (iter1->first == "sent") {
                BOOST_FOREACH(const ptree::value_type &v2, iter1->second) {
                    if (v2.first == "orig") {
                        std::string str(v2.second.data().begin() + 2, v2.second.data().end());
                        std::cout << j++ << ". " << str;
                    } else if (v2.first == "trans") {
                        std::string str(v2.second.data().begin() + 2, v2.second.data().end());
                        std::cout << "    " << str;
                    }
                }
            }
        }
    }

    void printResultDictionary(const std::string& text, const std::string& str) {
        YJson js(str), *ptr;
        if (js.getType() != YJson::Array) {
            if (js.getType() == YJson::Object) {
                std::cout << text << "  ~  \033[33mdictionaryapi.com\033[0m\n\nNo Definitions Found.\n";
            } else {
                std::cout << str;
            }
            return;
        }
        ptr = js.getChild();
        if (!ptr || ptr->getType() != YJson::Object || ptr->empty()) {
            std::cout << "Error occurred！" << std::endl;
            return;
        } else {
            YJson *speek = ptr->find("phonetic"), *speeks = ptr->find("phonetics");
            if (speek) {
                std::cout << ptr->find("word")->getValueString() << "  " << ptr->find("phonetic")->getValueString() << "  ~  \033[33mdictionaryapi.com\033[0m\n\n";
            } else if (speeks) {
                auto ptr1 = speeks->getChild();
                if (!ptr1) {
                    std::cout << ptr->find("word")->getValueString() << "  ~  \033[33mdictionaryapi.com\033[0m\n\n";
                } else {
                    while (ptr1) {
                        YJson *ptr2 = ptr1->find("text");
                        if (ptr2) {
                            std::cout << ptr->find("word")->getValueString() << "  " << ptr2->getValueString() << "  ~  \033[33mdictionaryapi.com\033[0m\n\n";
                            break;
                        }
                        ptr1 = ptr1->getNext();
                    }
                }
            } else {
                std::cout << ptr->find("word")->getValueString() << "  ~  dictionaryapi.com\n\n";
            }
            if ((ptr = ptr->find("meanings"))) {
                YJson* ptr1 = ptr->getChild();
                while (ptr1) {
                    int i = 1;
                    std::cout << "- [ " << ptr1->find("partOfSpeech")->getValueString() << " ]\n";
                    YJson* ptr2 = ptr1->find("definitions")->getChild();
                    while (ptr2) {
                        YJson *ptr3 = ptr2->find("definition"), *ptr4 = ptr2->find("example");
                        if (ptr4) {
                            std::cout << "  " << i++ << ". " << ptr3->getValueString() << "\n  > " << ptr4->getValueString() << std::endl;
                        } else {
                            std::cout << "  " << i++ << ". " << ptr3->getValueString() << std::endl;
                        }
                        ptr2 = ptr2->getNext();
                    }
                    ptr1 = ptr1->getNext();
                }
            }
        }
    }
};

bool writeLogFile(const std::string && str) {
    // std::cout << u8"当前日志为文字：" << str << std::endl;
    std::ofstream file("buginfo.log", std::ios::out | std::ios::binary);
    if (file.is_open()) {
        file << str;
        file.close();
        return true;
    } else {
        return false;
    }
}

#ifdef _WIN32

class ArgList final {
private:
    static int count;
    LPWSTR *szArglist;
    int nArgs = 1;
    std::vector<std::wstring_view> value;
public:
    explicit ArgList(): szArglist(CommandLineToArgvW(GetCommandLineW(), &nArgs)) {
        ++count;
        if( NULL == szArglist )
        {
            wprintf(L"CommandLineToArgvW failed\n");
        }
        value.reserve(nArgs);
        for (int i = 1; i < nArgs; ++i) {
            value.emplace_back(szArglist[i]);
        }
    }
    ~ArgList() {
        if (count == 1) {
            LocalFree(szArglist);
            --count;
        } else if (count) {
            --count;
        }
    }
    std::vector<std::wstring_view>& getValue() { return value; }
};

int ArgList::count = 0;
#endif

int main(int argc, char*argv[]) {
#if 1
#ifdef _WIN32
    initialize_utf8_console();
    {
        DWORD flags;
        if (!InternetGetConnectedState(&flags, 0)) {
            std::cout << "网络连接异常！\n";
            return 0;
        } 
    }
#endif
    try {
        Translation translater;
#ifdef _WIN32
        LPWSTR szCmdLine = GetCommandLineW();
        std::wstring_view str = szCmdLine;
        std::string text;
        auto iter = std::find(str.begin(), str.end(), ' ');
        while (iter != str.end() && *iter == ' ') ++iter;
        if (iter != str.end()) {
            text = UnicodeToUtf8(iter - str.begin() + szCmdLine);
            std::cout << "\033[32m\u276f\033[0m " << text << std::endl;
        } else {
            std::cout << "\033[32m\u276f\033[0m ";
            std::getline(std::cin, text);
            if (text.back() == '\r') text.pop_back();
        }
#else
        std::string text;
        for (int i=1; i<argc; ++i) {
            text.append(argv[i]);
            text.push_back(' ');
        }
        if (!text.empty()) {
            if (text.back() == ' ') text.pop_back();
            std::cout << "\033[32m\u276f\033[0m " << text << std::endl;
        } else {
            std::cout << "\033[32m\u276f\033[0m ";
            std::getline(std::cin, text);
            if (text.back() == '\r') text.pop_back();
        }
#endif
        // writeLogFile(std::move(text));
        while (!text.empty()) {
            const std::string&& ecode_text = YEncode::urlEncode(text);
            // 爱词霸
            std::stringstream isstr;
            std::string&& strIciba = translater.getResultIciba(ecode_text);
            translater.printResultIciba(text, strIciba);

            translater.printCuttingLine();

            // 有道翻译
            const std::string&& result = translater.getResultYoudao(text);
            translater.printResultYoudao(text, result);

#if 0
            // 字典翻译
            if (isascii(text.front())) {
                translater.printCuttingLine();
                const std::string&& str = translater.getResultDictionary(ecode_text);
                translater.printResultDictionary(text, str);
            }
#endif

            std:: cout << "\n\033[32m\u276f\033[0m ";
            std::getline(std::cin, text);
            if (text.back() == '\r') text.pop_back();
        }
    } catch (bool exit) {
        
    }
#else
    SetConsoleOutputCP(CP_UTF8);
    // SetConsoleCP(936);
    // UINT oCodepage = GetConsoleOutputCP(), iCodepage = GetConsoleCP();
    // printf(u8"当前输出代码页 %d；输入代码页：%d。\n", oCodepage, iCodepage);
#endif
    return 0;
}
