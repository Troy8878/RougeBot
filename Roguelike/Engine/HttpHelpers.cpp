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
}

// ----------------------------------------------------------------------------

HttpRequestBodyImpl::~HttpRequestBodyImpl()
{
}

// ----------------------------------------------------------------------------

HttpHeaderCollection::HttpHeaderCollection()
{
  impl = std::make_shared<HttpHeaderCollectionImpl>();
}

// ----------------------------------------------------------------------------

HttpHeaderCollectionImpl::HttpHeaderCollectionImpl()
{
}

// ----------------------------------------------------------------------------

HttpHeaderCollectionImpl::~HttpHeaderCollectionImpl()
{
}

// ----------------------------------------------------------------------------
