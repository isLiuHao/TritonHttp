/******************************************************************************************
Project: UCSD CSE291C Course Project: Web Server for TritonHTTP

framer.hpp:
framing incoming bytes stream into http packets. Headers
*******************************************************************************************/
#ifndef FRAMER_HPP
#define FRAMER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <iostream>
#include <assert.h>
#include <string>

class Framer {
public:
    //向缓冲区添加数据
	void append(std::string chars);

	// 缓冲区是否有数据
	bool hasMessage() const;

	// 获取缓冲区中的第一条指令
	std::string topMessage() const;

	// 从缓冲区弹出第一条指令
	void popMessage();

	// 将字符串打印到ostream中（用于调试）
	void printToStream(std::ostream& stream) const;

	// 计算有效的消息
	int countMessage() const;

	~Framer(){
		_msgBuffer.clear();
	}

protected:
	// 在此处输入任何字段/状态
	std::vector<char> _msgBuffer;
	const char LF = '\n';
	const char CR = '\r';
};

#endif // CALCFRAMER_HPP
