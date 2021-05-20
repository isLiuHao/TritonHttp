/******************************************************************************************
Project: UCSD CSE291C Course Project: Web Server for TritonHTTP

diewithmessage.hpp:
System util:
end server if critical error is detected
*******************************************************************************************/
#ifndef DIEWITHMESSAGE_HPP
#define DIEWITHMESSAGE_HPP

#include <string>
#include <iostream>
#include <pthread.h>
void DiewithMessage(std::string msg);
void DiewithMessage_t(std::string msg);
#endif // HTTPD_H
