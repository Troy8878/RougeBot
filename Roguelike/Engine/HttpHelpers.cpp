/*********************************
 * HttpHelpers.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

// ----------------------------------------------------------------------------

HttpRequestBody::HttpRequestBody()
{
  impl = std::make_shared<HttpRequestBodyImpl>();
}

// ----------------------------------------------------------------------------

HttpRequestBodyImpl::HttpRequestBodyImpl()
{
  bodyType = BODY_EMPTY;
}

// ----------------------------------------------------------------------------

HttpRequestBodyImpl::~HttpRequestBodyImpl()
{
}

// ----------------------------------------------------------------------------

HttpHeaderCollection HttpHeaderCollection::ParseHeaders(const WCHAR *str)
{
  HttpHeaderCollection collection;
  std::wistringstream input{str};

  std::wstring line;
  std::getline(input, line); // Skip over the status code

  while(std::getline(input, line))
  {
    line = chomp(line);
    if (line.empty())
      break;

    size_t key_end = line.find_first_of(L':');
    auto key = narrow(line.substr(0, key_end));
    auto val = line.substr(key_end + 1);

    auto set = collection[key];
    auto items = split(val, L';');
    for (auto& item : items)
    {
      set.AddValue(chomp(narrow(item)));
    }
  }

  return collection;
}

// ----------------------------------------------------------------------------

HttpHeaderCollection::HttpHeaderCollection()
{
  impl = std::make_shared<HttpHeaderCollectionImpl>();
}

// ----------------------------------------------------------------------------

HttpHeaderSet HttpHeaderCollection::operator[](const std::string& key)
{
  return HttpHeaderSet(key, &impl->headers[key]);
}

// ----------------------------------------------------------------------------

const HttpHeaderSet HttpHeaderCollection::operator[](const std::string& key) const
{
  return HttpHeaderSet(key, &impl->headers[key]);
}

// ----------------------------------------------------------------------------

std::wstring HttpHeaderCollection::BuildList() const
{
  std::wostringstream buf;

  for (auto& pair : impl->headers)
  {
    if (pair.first == "Accept")
      continue;
    if (pair.second.empty())
      continue;

    buf << widen(pair.first) << L": ";

    bool first = true;
    for (auto& val : pair.second)
    {
      if (first)
        first = false;
      else
        buf << L';';

      buf << widen(val.Value);
    }

    buf << "\r\n";
  }

  return buf.str();
}

// ----------------------------------------------------------------------------

void HttpHeaderSet::AddValue(const std::string& val)
{
  HttpHeaderEntry entry(key, val);

  if (std::find(begin(), end(), entry) != end())
    return;

  items->push_back(entry);
}

// ----------------------------------------------------------------------------

void HttpHeaderSet::RemoveValue(const std::string& val)
{
  HttpHeaderEntry entry(key, val);
  auto it = std::find(begin(), end(), entry);
  if (it != end())
    items->erase(it);
}

// ----------------------------------------------------------------------------

void HttpHeaderSet::Clear()
{
  items->clear();
}

// ----------------------------------------------------------------------------
