#include "regex.h"

std::vector<std::string> Regex::match(std::string const& pattern, std::string const& str){
	std::vector<std::string> result;
	GMatchInfo* mi = NULL;
	GRegex* gr = g_regex_new(pattern.c_str(), GRegexCompileFlags(0), GRegexMatchFlags(0), NULL);
	bool success = g_regex_match(gr, str.c_str(), GRegexMatchFlags(0), &mi);
	if (!success)
		throw REGEX_PATTERN_NOT_MATCHING;

	gint match_count = g_match_info_get_match_count(mi);
	for (gint i = 0; i < match_count; i++){
		gchar* matched_string = g_match_info_fetch(mi, i);
		result.push_back(std::string(matched_string));
		delete matched_string;
	}

	g_match_info_free(mi);
	g_regex_unref(gr);
	return result;
}
std::string Regex::replace(std::string const& pattern, std::string const& str, std::map<int, std::string> const& newValues){
	std::string result = str;
	GMatchInfo* mi = NULL;
	GRegex* gr = g_regex_new(pattern.c_str(), GRegexCompileFlags(0), GRegexMatchFlags(0), NULL);
	bool success = g_regex_match(gr, str.c_str(), GRegexMatchFlags(0), &mi);
	if (!success)
		throw REGEX_PATTERN_NOT_MATCHING;

	gint match_count = g_match_info_get_match_count(mi);
	gint offset = 0;
	for (std::map<int, std::string>::const_iterator iter = newValues.begin(); iter != newValues.end(); iter++){
		gint start_pos, end_pos;
		g_match_info_fetch_pos(mi, iter->first, &start_pos, &end_pos);
		if (start_pos != -1 && end_pos != -1) { //ignore unmatched (optional) values
			result.replace(start_pos+offset, end_pos-start_pos, iter->second);
			offset += iter->second.length() - (end_pos-start_pos);
		}
	}

	g_match_info_free(mi);
	g_regex_unref(gr);
	return result;
}
