#include "utility.h"
#include <stdlib.h>
#include <string.h>

struct curl_http_get_write_cookie{
	unsigned char *buffer_data;
	struct error *error;
	size_t buffer_length;
	size_t buffer_capacity;};

size_t curl_http_get_write(
	char *in_data,
	size_t in_width,
	size_t in_length,
	void *in_out_cookie){
	struct curl_http_get_write_cookie *cookie;
	struct error *error;
	unsigned char *buffer_data;
	unsigned char *new_buffer_data;
	size_t length;
	size_t buffer_length;
	size_t new_buffer_length;
	size_t buffer_capacity;
	size_t new_buffer_capacity_1;
	size_t new_buffer_capacity_2;
	size_t new_buffer_capacity;
	int code;
	if(!(length = in_width * in_length)){
		return in_length;}
	cookie = (struct curl_http_get_write_cookie *)in_out_cookie;
	if(length > SIZE_MAX - (buffer_length = cookie->buffer_length)){
		code = CURL_HTTP_GET_ERROR_CODE_LENGTH_LIMIT;
		goto curl_http_get_write_failure;}
	if((new_buffer_length = buffer_length + length) <= (buffer_capacity =
	cookie->buffer_capacity)){
		memcpy(cookie->buffer_data + buffer_length, in_data, length);
		cookie->buffer_length = new_buffer_length;
		return length;}
	if(buffer_capacity < (SIZE_MAX >> 1)){
		new_buffer_capacity_1 = new_buffer_length;
		new_buffer_capacity_2 = (buffer_capacity << 1);
		if(new_buffer_capacity_1 > new_buffer_capacity_2){
			new_buffer_capacity = new_buffer_capacity_1;}
		else{
			new_buffer_capacity = new_buffer_capacity_2;}}
	else{
		new_buffer_capacity = SIZE_MAX;}
	if(!(new_buffer_data = (unsigned char *)malloc(new_buffer_capacity))){
		code = CURL_HTTP_GET_ERROR_CODE_ALLOCATION_FAILURE;
		goto curl_http_get_write_failure;}
	if((buffer_data = cookie->buffer_data)){
		memcpy(new_buffer_data, buffer_data, buffer_length);
		free(buffer_data);}
	memcpy(new_buffer_data + buffer_length, in_data, length);
	cookie->buffer_data = new_buffer_data;
	cookie->buffer_length = new_buffer_length;
	cookie->buffer_capacity = new_buffer_capacity;
	return in_length;
curl_http_get_write_failure:
	(error = cookie->error)->domain = &curl_http_get_error_domain;
	error->code = code;
	return 0;}

bool curl_http_get(
	CURL *in_out_curl,
	char const *in_url,
	size_t *out_header_length,
	unsigned char **out_header_data,
	size_t *out_body_length,
	unsigned char **out_body_data,
	struct error *out_error){
	struct curl_http_get_write_cookie header_cookie;
	struct curl_http_get_write_cookie body_cookie;
	struct error header_error;
	struct error body_error;
	CURLcode code;
	body_cookie.buffer_data = header_cookie.buffer_data = NULL;
	body_error.code = header_error.code = 0;
	header_cookie.error = &header_error;
	body_cookie.error = &body_error;
	body_cookie.buffer_capacity = body_cookie.buffer_length = 
	header_cookie.buffer_capacity = header_cookie.buffer_length = 0;
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_HTTPGET, 1)) != CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_URL, in_url)) != CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_HEADERFUNCTION,
	&curl_http_get_write)) != CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_HEADERDATA,
	&header_cookie)) != CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_WRITEFUNCTION,
	&curl_http_get_write)) != CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_setopt(in_out_curl, CURLOPT_WRITEDATA, &body_cookie)) != 
	CURLE_OK){
		goto curl_http_get_failure;}
	if((code = curl_easy_perform(in_out_curl)) != CURLE_OK){
		if(header_error.code){
			*out_error = header_error;}
		else{
			*out_error = body_error;}
		return false;}
	*out_header_length = header_cookie.buffer_length;
	*out_header_data = header_cookie.buffer_data;
	*out_body_length = body_cookie.buffer_length;
	*out_body_data = body_cookie.buffer_data;
	return true;
curl_http_get_failure:
	out_error->domain = &curl_error_domain;
	out_error->code = code;
	return false;}

char const *curl_http_get_error_message(
	int code){
	switch(code){
	case CURL_HTTP_GET_ERROR_CODE_LENGTH_LIMIT:
		return "length limit";
	case CURL_HTTP_GET_ERROR_CODE_ALLOCATION_FAILURE:
		return "allocation failure";}
	return "unknown error";}

struct error_domain const curl_http_get_error_domain = {
	&curl_http_get_error_message,
	"curl_http_get"};

char const *curl_error_message(
	int code){
	return curl_easy_strerror(code);}

struct error_domain const curl_error_domain = {
	&curl_error_message,
	"curl"};

void gumbo_free_node(void *node){
}

bool gumbo_traverse_tree(
	GumboNode const *in_root,
	void *in_out_cookie,
	bool (*in_visit)(
		void *in_out_cookie,
		GumboNode const *in_node,
		struct error *out_error),
	struct error *out_error){
	struct array_list *node_list;
	GumboNode const *node;
	GumboNode const **child_nodes;
	GumboNode const **end_child_nodes;
	GumboVector const *vector;
	size_t length;
	size_t index;
	GumboNodeType type;
	int code;
	if(!(node_list = array_list_new(&gumbo_free_node))){
		code = GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_NEW;
		goto gumbo_traverse_tree_failure;}
	if(array_list_add(node_list, (void *)in_root)){
		code = GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_ADD;
		goto gumbo_traverse_tree_failure;}
	while((length = array_list_length(node_list))){
		if(!(node = (GumboNode const *)array_list_get_idx(node_list, (
		index = length - 1)))){
			code = GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_GET_IDX;
			goto gumbo_traverse_tree_failure;}
		if(array_list_del_idx(node_list, index, 1)){
			code = GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_DEL_IDX;
			goto gumbo_traverse_tree_failure;}
		if(!in_visit(in_out_cookie, node, out_error)){
			return false;}
		type = node->type;
		if((type == GUMBO_NODE_DOCUMENT) | (type == GUMBO_NODE_ELEMENT)){
			child_nodes = (GumboNode const **)((vector = 
			(GumboVector const *)&node->v)->data);
			end_child_nodes = child_nodes + vector->length;
			while(child_nodes != end_child_nodes){
				if(array_list_add(node_list, (void *)*child_nodes)){
					code = GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_ADD;
					goto gumbo_traverse_tree_failure;}
				++child_nodes;}}}
	array_list_free(node_list);
	return true;
gumbo_traverse_tree_failure:
	out_error->domain = &gumbo_traverse_tree_error_domain;
	out_error->code = code;
	return false;}

char const *gumbo_traverse_tree_error_code(
	int code){
	switch(code){
	case GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_NEW:
		return "array_list_new";
	case GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_ADD:
		return "array_list_add";
	case GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_GET_IDX:
		return "array_list_get_idx";
	case GUMBO_TRAVERSE_TREE_ERROR_CODE_ARRAY_LIST_DEL_IDX:
		return "array_list_del_idx";}
	return "unknown error";}

struct error_domain const gumbo_traverse_tree_error_domain = {
	&gumbo_traverse_tree_error_code,
	"gumbo_traverse_tree"};

