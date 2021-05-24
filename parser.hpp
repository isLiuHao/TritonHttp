/******************************************************************************************
Project: UCSD CSE291C Course Project: Web Server for TritonHTTP

Parser.hpp: header file
Verify the header format and parse the request into valid request data structure
*******************************************************************************************/


#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <queue>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include "httpd.h"
using namespace std;

typedef struct HttpInstruction_t {
	// DEFINE YOUR DATA STRUCTURE HERE
	/* Noted: the only correct method is GET, therefore no instruction is indicated here*/
	string url;
	float http_ver;
	string host;
	string connection;
} HttpInstruction;

class Parser {
	private:
		queue<HttpInstruction> reqQ;
		HttpInstruction _req;
		bool hasInstr;
		bool _isTerminated;
		vector<string> parseHelper(string insstr, char del);
	public:

		//构造函数，初始化私有字段
		Parser();

		/*
			将字符串解析为有效的header
			返回值：如果字符串包含有效header，则返回true；否则，返回false。否则为假
		*/
		bool parse(string insstr);

		//返回当前解析的http headers
		HttpInstruction getReqHeader(){
			if(!reqQ.empty()){
				HttpInstruction res = reqQ.front();
				reqQ.pop();
				return res;
			}
			return _req;
		}

		/*
			Return if connection needs to be terminated by client
		*/
		bool isTerminated(){
			return _isTerminated;
		}

		bool isInstr(){
			return !reqQ.empty();
		}
};

#endif // CALCPARSER_HPP
