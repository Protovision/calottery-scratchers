#ifndef CURL_GUMBO_UTILITY_H
#define CURL_GUMBO_UTILITY_H

#include <json-c/arraylist.h>
#include <curl/curl.h>
#include <gumbo.h>
#include <stdbool.h>

enum curl_http_get_error_code{
	CURL_HTTP_GET_ERROR_CODE_SUCCESS = 0,
	CURL_HTTP_GET_ERROR_CODE_LENGTH_LIMIT,
	CURL_HTTP_GET_ERROR_CODE_ALLOCATION_FAILURE};

enum gumbo_traverse_tree_error_code{
	GUMBO_TRAVERSE_TREE_ERROR_CODE_SUCCESS = 0,
	GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_NEW,
	GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_ADD,
	GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_GET_IDX,
	GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_DEL_IDX};

struct error_domain{
	char const *(*message)(int code);
	char const *name;};

struct error{
	struct error_domain const *domain;
	int code;};

bool curl_http_get(
	CURL *in_out_curl,
	char const *in_url,
	size_t *out_header_length,
	unsigned char **out_header_data,
	size_t *out_body_length,
	unsigned char **out_body_data,
	struct error *out_error);

bool gumbo_traverse_tree(
	GumboNode const *in_root,
	void *in_out_cookie,
	bool (*in_visit)(
		void *in_out_cookie,
		GumboNode const *in_node,
		struct error *out_error),
	struct error *out_error);

extern struct error_domain const curl_error_domain;
extern struct error_domain const curl_http_get_error_domain;
extern struct error_domain const gumbo_traverse_tree_error_domain;

#endif

