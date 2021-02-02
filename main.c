#include "utility.h"
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>

#define CALOTTERY_SCRATCHERS_URL "https://www.calottery.com/api/Sitecore/ScratchersFilteredList/GetScratchers?modelId=0dc0c687-836a-43d3-aa8b-a0491dbf4001&sortBy=newest&page=1&size=256&show=&gametype=&price=&nameOrNumber="

enum main_error_code{
	MAIN_ERROR_CODE_SUCCESS = 0,
	MAIN_ERROR_CODE_CURL_EASY_INIT,
	MAIN_ERROR_CODE_JSON_TOKENER_PARSE,
	MAIN_ERROR_CODE_LH_KCHAR_TABLE_NEW,
	MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_GET,
	MAIN_ERROR_CODE_ARRAY_GET_IDX,
	MAIN_ERROR_CODE_SNPRINTF,
	MAIN_ERROR_CODE_GUMBO_PARSE,
	MAIN_ERROR_CODE_JSON_OBJECT_NEW_DOUBLE,
	MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_ADD_EX,
	MAIN_ERROR_CODE_LH_TABLE_INSERT};

char const *main_error_message(
	int code){
	switch((enum main_error_code)code){
	case MAIN_ERROR_CODE_CURL_EASY_INIT:
		return "curl_easy_init";
	case MAIN_ERROR_CODE_JSON_TOKENER_PARSE:
		return "json_tokener_parse";
	case MAIN_ERROR_CODE_LH_KCHAR_TABLE_NEW:
		return "lh_kchar_table_new";
	case MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_GET:
		return "json_object_object_get";
	case MAIN_ERROR_CODE_ARRAY_GET_IDX:
		return "array_get_idx";
	case MAIN_ERROR_CODE_SNPRINTF:
		return "snprintf";
	case MAIN_ERROR_CODE_GUMBO_PARSE:
		return "gumbo_parse";
	case MAIN_ERROR_CODE_JSON_OBJECT_NEW_DOUBLE:
		return "json_object_new_double";
	case MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_ADD_EX:
		return "json_object_object_add_ex";
	case MAIN_ERROR_CODE_LH_TABLE_INSERT:
		return "lh_table_insert";}
	return "unknown error";}

struct error_domain const main_error_domain = {
	&main_error_message,
	"main"};

bool update_scratcher_profit(
	void *in_out_profit,
	GumboNode const *in_node,
	struct error *out_error){
	double *profit;
	GumboAttribute const *attribute;
	GumboAttribute const **attribute_list_data;
	GumboAttribute const **attribute_list_data_end;
	GumboNode const *child;
	GumboNode const **child_list_data;
	GumboNode const **child_list_data_end;
	char const *text;
	double prize;
	double odds;
	size_t length;
	size_t i;
	size_t n;
	char character;
	static char odds_string[64];
	if(in_node->type != GUMBO_NODE_ELEMENT){
		return true;}
	if(in_node->v.element.tag != GUMBO_TAG_TR){
		return true;}
	if(!(length = in_node->v.element.attributes.length)){
		return true;}
	profit = (double *)in_out_profit;
	attribute_list_data = (GumboAttribute const **)in_node->
	v.element.attributes.data;
	attribute_list_data_end = attribute_list_data + length;
	while(attribute_list_data != attribute_list_data_end){
		if(strcmp((attribute = *attribute_list_data)->name, "class")){
			++attribute_list_data;
			continue;}
		if(strcmp(attribute->value, "odds-available-prizes__table__body")){
			++attribute_list_data;
			continue;}
		prize = 0.0;
		odds = 0.0;
		child_list_data = (GumboNode const **)in_node->v.element.children.data;
		child_list_data_end = child_list_data + in_node->v.element.children.length;
		while(child_list_data != child_list_data_end){
			if((child = *child_list_data)->type != GUMBO_NODE_ELEMENT){
				++child_list_data;
				continue;}
			if(child->v.element.tag != GUMBO_TAG_TD){
				++child_list_data;
				continue;}
			text = ((GumboNode const *)*child->v.element.children.data)->v.text.text;
			character = *text;
			if(character == '$'){
				prize = atof(text + 1);}
			else if((character >= '0') & (character <= '9')){
				i = 0;
				while((character = *text)){
					if(character != ','){
						odds_string[i++] = character;}
					++text;}
				odds_string[i] = 0;
				odds = atof(odds_string);}
			if((prize != 0.0) & (odds != 0.0)){
				*profit += prize / odds;
				break;}
			++child_list_data;}
		break;}
	return true;}

void free_best_scratchers_entry(struct lh_entry *unused){
}

int compare_scratchers_by_percent_profit(
	void const *left,
	void const *right){
	struct json_object *left_scratcher;
	struct json_object *right_scratcher;
	double left_value;
	double right_value;
	left_scratcher = *(struct json_object **)left;
	right_scratcher = *(struct json_object **)right;
	left_value = json_object_get_double(json_object_object_get(left_scratcher,
	"percentProfit"));
	right_value = json_object_get_double(json_object_object_get(right_scratcher,
	"percentProfit"));
	if(left_value < right_value){
		return 1;}
	else if(left_value == right_value){
		return 0;}
	return -1;}

int main(){
	static char url[256];
	CURL *curl;
	unsigned char *header_data;
	unsigned char *body_data;
	struct json_object *json;
	struct json_object *scratcher_list;
	struct json_object *scratcher;
	struct json_object *scratcher_page;
	struct json_object *scratcher_price;
	struct json_object *scratcher_profit;
	struct json_object *scratcher_percent_profit;
	GumboOutput *html;
	char const *price_string;
	struct error error;
	double profit;
	double price;
	size_t header_length;
	size_t body_length;
	size_t i;
	size_t n;
	int status;
	header_data = NULL;
	body_data = NULL;
	html = NULL;
	error.domain = NULL;
	error.code = 0;
	if(!(curl = curl_easy_init())){
		error.code = MAIN_ERROR_CODE_CURL_EASY_INIT;
		goto main_failure;}
	if(!curl_http_get(curl, CALOTTERY_SCRATCHERS_URL,
	&header_length, &header_data, &body_length, &body_data, &error)){
		goto main_failure;}
	if(!(json = json_tokener_parse((char const *)body_data))){
		error.code = MAIN_ERROR_CODE_JSON_TOKENER_PARSE;
		goto main_failure;}
	free(header_data);
	header_data = NULL;
	free(body_data);
	body_data = NULL;
	if(!(scratcher_list = json_object_object_get(json, 
	"SerializedScratcherCardList"))){
		error.code = MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_GET;
		goto main_failure;}
	i = 0;
	n = json_object_array_length(scratcher_list);
	while(i < n){
		if(!(scratcher = json_object_array_get_idx(scratcher_list, i))){
			error.code = MAIN_ERROR_CODE_ARRAY_GET_IDX;
			goto main_failure;}
		if(!(scratcher_page = json_object_object_get(scratcher, 
		"GameProductPage"))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_GET;
			goto main_failure;}
		if(snprintf(url, sizeof(url), "%s%s", "https://www.calottery.com", 
		json_object_get_string(scratcher_page)) <= 0){
			error.code = MAIN_ERROR_CODE_SNPRINTF;
			goto main_failure;}
		fprintf(stdout, "%s\n", url);
		if(!curl_http_get(curl, url, &header_length, &header_data, &body_length, 
		&body_data, &error)){
			goto main_failure;}
		if(!(html = gumbo_parse((char const *)body_data))){
			error.code = MAIN_ERROR_CODE_GUMBO_PARSE;
			goto main_failure;}
		profit = 0.0;
		if(!gumbo_traverse_tree(html->root, &profit, &update_scratcher_profit, 
		&error)){
			goto main_failure;}
		gumbo_destroy_output(&kGumboDefaultOptions, html);
		html = NULL;
		free(header_data);
		header_data = NULL;
		free(body_data);
		body_data = NULL;
		if(!(scratcher_price = json_object_object_get(scratcher, "GamePrice"))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_GET;
			goto main_failure;}
		profit -= (price = atof(json_object_get_string(scratcher_price) + 1));
		if(!(scratcher_profit = json_object_new_double(profit))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_NEW_DOUBLE;
			goto main_failure;}
		if(json_object_object_add_ex(scratcher, "profit", scratcher_profit,
		(JSON_C_OBJECT_ADD_KEY_IS_NEW | JSON_C_OBJECT_KEY_IS_CONSTANT))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_ADD_EX;
			goto main_failure;}
		if(!(scratcher_percent_profit = json_object_new_double(profit / price * 
		100.0))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_NEW_DOUBLE;
			goto main_failure;}
		if(json_object_object_add_ex(scratcher, "percentProfit", 
		scratcher_percent_profit, (JSON_C_OBJECT_ADD_KEY_IS_NEW | 
		JSON_C_OBJECT_KEY_IS_CONSTANT))){
			error.code = MAIN_ERROR_CODE_JSON_OBJECT_OBJECT_ADD_EX;
			goto main_failure;}
		++i;}
	json_object_array_sort(scratcher_list, &compare_scratchers_by_percent_profit);
	i = 0;
	n = json_object_array_length(scratcher_list);
	fputs("\npercent_profit,profit,price,name,number\n", stdout);
	while(i < n){
		if(!(scratcher = json_object_array_get_idx(scratcher_list, i))){
			error.code = MAIN_ERROR_CODE_ARRAY_GET_IDX;
			goto main_failure;}
		fprintf(stdout, "%lf,%s,%s,\"%s\",%s\n", json_object_get_double(
		json_object_object_get(scratcher, "percentProfit")), 
		json_object_get_string(json_object_object_get(scratcher, "profit")),(
		json_object_get_string(json_object_object_get(scratcher, "GamePrice")) + 1),
		json_object_get_string(json_object_object_get(scratcher, "GameName")),
		json_object_get_string(json_object_object_get(scratcher, "GameNumber")));
		++i;}
main_failure:
	if(error.code){
		if(!error.domain){
			error.domain = &main_error_domain;}
		fprintf(stderr, "%s: %s\n", error.domain->name, error.domain->message(
		error.code));
		status = EXIT_FAILURE;}
	else{
		status = EXIT_SUCCESS;}
	if(html){
		gumbo_destroy_output(&kGumboDefaultOptions, html);}
	if(json){
		json_object_put(json);}
	if(header_data){
		free(header_data);}
	if(body_data){
		free(body_data);}
	curl_easy_cleanup(curl);
	return status;}

