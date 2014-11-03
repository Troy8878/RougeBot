/*********************************
 * HttpUri.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

// ----------------------------------------------------------------------------

HttpUri::HttpUri()
{
}

// ----------------------------------------------------------------------------

HttpUri::HttpUri(const std::string& uri)
  : HttpUri(Parse(uri))
{
}

// ----------------------------------------------------------------------------

std::string HttpUri::Encode(const std::string& str)
{
  static std::locale loc;
  std::ostringstream buf;
  buf.fill(0);
  buf << std::hex;

  for (auto c : str)
  {
    if (std::isalnum(c, loc))
      buf << c;
    else
      buf << '%' << std::setw(2) << int(c);
  }

  return buf.str();
}

// ----------------------------------------------------------------------------

std::string HttpUri::Decode(const std::string& str)
{
  std::ostringstream buf;

  const size_t size = str.size();
  for (size_t i = 0; i < size; ++i)
  {
    const auto& c = str[i];

    if (c == '%')
    {
      if (i + 2 >= size)
        break;

      auto val = std::stoul(str.substr(i + 1, 2));
      buf << char(val);

      i += 2;
    }
    else
    {
      buf << c;
    }
  }

  return buf.str();
}

// ----------------------------------------------------------------------------

HttpUri HttpUri::Parse(std::string uri)
{
  try
  {
    HttpUri result;
    size_t pos = 0;

    if ((pos = uri.find_first_of("://")) != uri.npos && uri.size() >= pos + 3)
    {
      result.Scheme = uri.substr(0, pos);
      uri = uri.substr(pos + 3);
    }
    else if ((pos = uri.find_first_of("//")) == 0)
    {
      uri = uri.substr(2);
    }

    size_t path_start = uri.find_first_of('/');
    size_t at_sign = uri.find_first_of('@');
    if (at_sign < path_start)
    {
      std::string userinfo = uri.substr(0, at_sign);
      pos = userinfo.find_first_of(':');
      if (pos != userinfo.npos)
      {
        result.Password = userinfo.substr(pos + 1);
        userinfo = userinfo.substr(0, pos);
      }

      result.Username = userinfo;

      uri = uri.substr(at_sign + 1);
      path_start = uri.find_first_of('/');
    }

    size_t port_start = uri.find_first_of(':');
    if (port_start < path_start)
    {
      result.Host = uri.substr(0, port_start);

      size_t port_len = path_start - (port_start + 1);
      if (port_len != uri.npos)
        result.Port = std::stoi(uri.substr(port_start + 1, port_len));
    }
    else
    {
      result.Host = uri.substr(0, path_start);
    }

    if (path_start == uri.npos)
      return result;

    uri = uri.substr(path_start);

    size_t query_start = uri.find_first_of('?');
    if (query_start != uri.npos)
    {
      result.Path = uri.substr(0, query_start);
      uri = uri.substr(query_start + 1);
    }
    else
    {
      result.Path = uri;
      return result;
    }

    size_t fragment_start = uri.find_first_of('#');
    if (fragment_start != uri.npos)
    {
      result.Query = uri.substr(0, fragment_start);
      uri = uri.substr(fragment_start + 1);
    }
    else
    {
      result.Query = uri;
      return result;
    }

    // The rest of the URI will be the fragment
    result.Fragment = uri;
    return result;
  }
  catch(...)
  {
    throw basic_exception("Malformed URI");
  }
}

// ----------------------------------------------------------------------------

std::string HttpUri::Build() const
{
  if (!HasHost)
    throw basic_exception("A URI must have a host");

  std::stringstream buf;

  if (HasScheme)
    buf << Scheme << "://";

  if (HasUsername)
  {
    buf << Username;
    if (HasPassword)
      buf << ':' << Password;
    buf << '@';
  }

  buf << Host;
  buf << BuildPath();

  return buf.str();
}

// ----------------------------------------------------------------------------

std::string HttpUri::BuildPath() const
{
  std::stringstream buf;

  if (HasPath)
    buf << Path;

  if (HasQuery)
    buf << '?' << Query;

  if (HasFragment)
    buf << '#' << Fragment;

  return buf.str();
}

// ----------------------------------------------------------------------------

