// Copyright (c) 2022-2024 The MetabaseNet developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MTBASE_UTIL_H
#define MTBASE_UTIL_H

#include <boost/asio/ip/address.hpp>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/common.hpp>
#include <regex>

#include "type.h"

namespace mtbase
{

extern bool STD_DEBUG;

void SetThreadName(const char* name);

std::string GetThreadName();

void PrintTrace();

inline int64 GetTime()
{
    using namespace boost::posix_time;
    static ptime epoch(boost::gregorian::date(1970, 1, 1));
    return int64((second_clock::universal_time() - epoch).total_seconds());
}

inline bool IsDoubleEqual(double a, double b)
{
    return std::abs(a - b) < std::abs(std::min(a, b)) * std::numeric_limits<double>::epsilon();
}

inline bool IsDoubleNonPositiveNumber(double a)
{
    return (a < std::numeric_limits<double>::epsilon());
}

inline bool IsNumber(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

inline int64 GetLocalTimeSeconds()
{
    using namespace boost::posix_time;
    static ptime epoch(boost::gregorian::date(1970, 1, 1));
    return int64((second_clock::local_time() - epoch).total_seconds());
}

inline int64 GetTimeMillis()
{
    using namespace boost::posix_time;
    static ptime epoch(boost::gregorian::date(1970, 1, 1));
    return int64((microsec_clock::universal_time() - epoch).total_milliseconds());
}

inline std::string GetLocalTime()
{
    using namespace boost::posix_time;
    time_facet* facet = new time_facet("%Y-%m-%d %H:%M:%s");
    std::stringstream ss;
    ss.imbue(std::locale(std::locale("C"), facet));
    ss << microsec_clock::local_time();
    return ss.str();
}

inline std::string GetUniversalTime()
{
    using namespace boost::posix_time;
    time_facet* facet = new time_facet("%Y-%m-%d %H:%M:%S");
    std::stringstream ss;
    ss.imbue(std::locale(std::locale("C"), facet));
    ss << second_clock::universal_time();
    return ss.str();
}

inline std::string GetTimeString(int64 nTime)
{
    using namespace boost::posix_time;
    time_facet* facet = new time_facet("%Y-%m-%d %H:%M:%S");
    std::stringstream ss;
    ss.imbue(std::locale(std::locale("C"), facet));
    ss << from_time_t(nTime);
    return ss.str();
}

class CTicks
{
public:
    CTicks()
      : t(boost::posix_time::microsec_clock::universal_time()) {}
    int64 operator-(const CTicks& ticks) const
    {
        return ((t - ticks.t).ticks());
    }
    int64 Elapse() const
    {
        return ((boost::posix_time::microsec_clock::universal_time() - t).ticks());
    }

public:
    boost::posix_time::ptime t;
};

enum severity_level
{
    debug,
    info,
    warn,
    error
};

namespace src = boost::log::sources;

typedef src::severity_channel_logger_mt<severity_level, std::string> sclmt_type;
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(lg, sclmt_type)

void StdTrace(const char* pszName, const char* pszFormat, ...);
void StdDebug(const char* pszName, const char* pszFormat, ...);
void StdLog(const char* pszName, const char* pszFormat, ...);
void StdWarn(const char* pszName, const char* pszFormat, ...);
void StdError(const char* pszName, const char* pszFormat, ...);

bool InitLog(const boost::filesystem::path& pathData, bool debug, bool daemon, int nLogFileSizeIn, int nLogHistorySizeIn);

inline std::string PulsFileLine(const char* file, int line, const char* info)
{
    std::stringstream ss;
    ss << file << "(" << line << ") " << info;
    return ss.str();
}

#define STD_DEBUG(Mod, Info) mtbase::StdDebug(Mod, mtbase::PulsFileLine(__FILE__, __LINE__, Info).c_str())

#define STD_LOG(Mod, Info) mtbase::StdLog(Mod, mtbase::PulsFileLine(__FILE__, __LINE__, Info).c_str())

#define STD_WARN(Mod, Info) mtbase::StdWarn(Mod, mtbase::PulsFileLine(__FILE__, __LINE__, Info).c_str())

#define STD_Eerror(Mod, Info) mtbase::StdError(Mod, mtbase::PulsFileLine(__FILE__, __LINE__, Info).c_str())

inline bool IsRoutable(const boost::asio::ip::address& address)
{
    if (address.is_loopback() || address.is_unspecified())
    {
        return false;
    }
    if (address.is_v4())
    {
        unsigned long u = address.to_v4().to_ulong();

        // RFC1918 https://tools.ietf.org/html/rfc1918
        // IP address space for private internets
        // 0x0A000000 => 10.0.0.0 prefix
        // 0xC0A80000 => 192.168.0.0 prefix
        // 0x0xAC100000 - 0xAC200000 => 172.16.0.0 - 172.31.0.0 prefix
        if ((u & 0xFF000000) == 0x0A000000 || (u & 0xFFFF0000) == 0xC0A80000
            || (u >= 0xAC100000 && u < 0xAC200000))
        {
            return false;
        }

        // RFC3927 https://tools.ietf.org/html/rfc3927
        // IPv4 Link-Local addresses
        // 0xA9FE0000 => 169.254.0.0 prefix
        if ((u & 0xFFFF0000) == 0xA9FE0000)
        {
            return false;
        }
    }
    else
    {
        boost::asio::ip::address_v6::bytes_type b = address.to_v6().to_bytes();

        // RFC4862 https://tools.ietf.org/html/rfc4862
        // IPv6 Link-local addresses
        // FE80::/64
        if (b[0] == 0xFE && b[1] == 0x80 && b[2] == 0 && b[3] == 0
            && b[4] == 0 && b[5] == 0 && b[6] == 0 && b[7] == 0)
        {
            return false;
        }

        // RFC4193 https://tools.ietf.org/html/rfc4193
        // Local IPv6 Unicast Addresses
        // FC00::/7 prefix
        if ((b[0] & 0xFE) == 0xFC)
        {
            return false;
        }

        // RFC4843 https://tools.ietf.org/html/rfc4843
        // An IPv6 Prefix for Overlay Routable Cryptographic Hash Identifiers
        // 2001:10::/28 prefix
        if (b[0] == 0x20 && b[1] == 0x01 && b[2] == 0x00 && (b[3] & 0xF0) == 0x10)
        {
            return false;
        }
    }
    return true;
}

inline std::string ToHexString(const unsigned char* p, std::size_t size)
{
    if (p == nullptr || size == 0)
    {
        return std::string("");
    }
    const char hexc[17] = "0123456789abcdef";
    char hex[128];
    std::string strHex;
    strHex.reserve(size * 2 + 2);
    strHex.append("0x", 2);

    for (size_t i = 0; i < size; i += 64)
    {
        size_t k;
        for (k = 0; k < 64 && k + i < size; k++)
        {
            int c = *p++;
            hex[k * 2] = hexc[c >> 4];
            hex[k * 2 + 1] = hexc[c & 15];
        }
        strHex.append(hex, k * 2);
    }
    return strHex;
}

inline std::string ToHexString(const std::vector<unsigned char>& vch)
{
    return ToHexString(&vch[0], vch.size());
}

inline std::string ToHexString(const uint32 n)
{
    char obuf[64] = { 0 };
    sprintf(obuf, "0x%x", n);
    return std::string(obuf);
}

inline std::string ToHexString(const uint64 n)
{
    char obuf[64] = { 0 };
    sprintf(obuf, "0x%lx", n);
    return std::string(obuf);
}

inline uint64 ParseNumericHexString(const std::string& str)
{
    return (uint64)std::stoll(str, nullptr, 16);
}

template <typename T>
inline std::string UIntToHexString(const T& t)
{
    const char hexc[17] = "0123456789abcdef";
    char hex[sizeof(T) * 2 + 2 + 1];
    strcpy(hex, "0x");
    for (std::size_t i = 0; i < sizeof(T); i++)
    {
        int byte = (t >> ((sizeof(T) - i - 1)) * 8) & 0xFF;
        hex[2 + i * 2] = hexc[byte >> 4];
        hex[2 + i * 2 + 1] = hexc[byte & 15];
    }
    hex[2 + sizeof(T) * 2] = 0;
    return std::string(hex);
}

inline int CharToHex(char c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    return -1;
}

inline std::vector<unsigned char> ParseHexString(const char* psz)
{
    std::vector<unsigned char> vch;
    vch.reserve(128);
    if (psz[0] == '0' && tolower(psz[1]) == 'x')
        psz += 2;
    while (*psz)
    {
        int h = CharToHex(*psz++);
        int l = CharToHex(*psz++);
        if (h < 0 || l < 0)
            break;
        vch.push_back((unsigned char)((h << 4) | l));
    }
    return vch;
}

inline std::size_t ParseHexString(const char* psz, unsigned char* p, std::size_t n)
{
    unsigned char* end = p + n;
    if (n >= 2 && psz[0] == '0' && tolower(psz[1]) == 'x')
        psz += 2;
    while (*psz && p != end)
    {
        int h = CharToHex(*psz++);
        int l = CharToHex(*psz++);
        if (h < 0 || l < 0)
            break;
        *p++ = (unsigned char)((h << 4) | l);
    }
    return (n - (end - p));
}

inline std::vector<unsigned char> ParseHexString(const std::string& str)
{
    return ParseHexString(str.c_str());
}

inline std::size_t ParseHexString(const std::string& str, unsigned char* p, std::size_t n)
{
    return ParseHexString(str.c_str(), p, n);
}

inline bool isNumeric(std::string const& str)
{
    return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
}

inline bool isHexNumeric(std::string const& str)
{
    if (str.empty())
    {
        return false;
    }
    std::regex r(R"([^0-9a-fA-Fx-xX-X]+)");
    std::smatch sm;
    if (std::regex_search(str, sm, r))
    {
        return false;
    }
    return true;
}

inline std::string ReverseHexString(const std::string& strIn)
{
    if (strIn.size() % 2 != 0 || !(strIn.size() >= 2 && strIn[0] == '0' && tolower(strIn[1]) == 'x'))
    {
        return "";
    }
    std::string strOut = strIn;
    for (auto itBegin = strOut.begin() + 2, itEnd = strOut.end() - 2; itBegin < itEnd; itBegin += 2, itEnd -= 2)
    {
        std::swap_ranges(itBegin, itBegin + 2, itEnd);
    }
    return strOut;
}

inline std::string ReverseHexNumericString(const std::string& strIn)
{
    if (!isHexNumeric(strIn))
    {
        return "";
    }
    if (!(strIn.size() >= 2 && strIn[0] == '0' && tolower(strIn[1]) == 'x'))
    {
        return "";
    }

    //0x9 -> 0x09
    //0x1234 -> 0x3412
    //0x123 -> 0x2301

    std::string strOut;
    strOut.resize(strIn.size() + (strIn.size() % 2));
    std::size_t b = 2;
    std::size_t e = strIn.size() - 1;
    strOut[0] = '0';
    strOut[1] = 'x';
    while (e >= 2)
    {
        strOut[b + 1] = strIn[e];
        if (e - 1 < 2)
        {
            strOut[b] = '0';
            break;
        }
        strOut[b] = strIn[e - 1];
        e -= 2;
        b += 2;
    }
    return strOut;
}

#ifdef __GNUG__
#include <cxxabi.h>
inline const char* TypeName(const std::type_info& info)
{
    int status = 0;
    return abi::__cxa_demangle(info.name(), 0, 0, &status);
}
#else
inline const char* TypeName(const std::type_info& info)
{
    return info.name();
}
#endif

inline uint32 BSwap16(uint16 n)
{
    return (((n & 0x00FF) << 8) | ((n & 0xFF00) >> 8));
}

inline uint32 BSwap32(uint32 n)
{
    return ((((n)&0x000000FF) << 24) | (((n)&0x0000FF00) << 8) | (((n)&0x00FF0000) >> 8) | (((n)&0xFF000000) >> 24));
}

inline uint64 BSwap64(uint64 n)
{
    n = ((n & 0xff00ff00ff00ff00ULL) >> 8) | ((n & 0x00ff00ff00ff00ffULL) << 8);
    n = ((n & 0xffff0000ffff0000ULL) >> 16) | ((n & 0x0000ffff0000ffffULL) << 16);
    return (n >> 32) | (n << 32);
}

bool SplitNumber(const std::string& strNumber, std::string& strInteger, std::string& strDecimal);

bool BtCompress(const bytes& btSrc, bytes& btDst);
bool BtUncompress(const bytes& btSrc, bytes& btDst);

} // namespace mtbase

#endif //MTBASE_UTIL_H
