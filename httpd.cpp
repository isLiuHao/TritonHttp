/******************************************************************************************
Project: UCSD CSE291C Course Project: Web Server for TritonHTTP

httpd.cpp:
Concurrency:
implementation of server start up, creating thread pool.
*******************************************************************************************/
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>
#include <string>
#include "httpd.h"
#include "diewithmessage.hpp"
#include "handleTCPClient.hpp"

using namespace std;

void start_httpd(unsigned short port, string doc_root)
{
	int servSocket;                 //设置服务器套接字
	const string root = doc_root;   //设置文档为根目录
	sockaddr_in echoServAddr;       //本地IP地址
	sockaddr *echoServAddr_t;
	unsigned short echoServPort = port;     //设置服务器端口
	pthread_t pid[POOL_SIZE];               //初始化线程池
	ThreadArgs *args = new ThreadArgs;      // servSocket+doc_root

	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;

    //socket()初始化套接字 AF_INET: TCP/IP–IPv4   SOCK_STREAM:TCP流  IPPROTO_TCP：TCP协议
	if((servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		DiewithMessage("Called socket(): Socket create failed"); //socket creation failed
	}

	// 构造本地地址结构
    memset(&echoServAddr, 0, sizeof(echoServAddr));   // echoServAddr内存初始化
    echoServAddr.sin_family = AF_INET;                // 地址族设为 TCP/IP–IPv4
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 设置IP INADDR_ANY：0.0.0.0本机
    echoServAddr.sin_port = htons(echoServPort);      // 设置端口号

	echoServAddr_t = (sockaddr *)&echoServAddr;
	// bind()绑定 协议,ip地址,端口号 到 servSocket
	if(bind(servSocket, echoServAddr_t, sizeof(echoServAddr)) < 0){
		DiewithMessage("Called bind(): socket binding failed"); //bind socket failed
	}
    // listen()监听
	if(listen(servSocket, CONNECTION_SIZE) < 0){
		DiewithMessage("Called listen(): listen failed"); //listen on socket failed
	}

	// 初始化线程池
	args->servSocket = servSocket;
	args->doc_root = doc_root;
	for(unsigned i = 0; i < POOL_SIZE; i++){
	    //创建线程 从HandleTCPClient函数运行
		if(pthread_create(&pid[i], NULL, &HandleTCPClient, args) < 0){
			DiewithMessage("Called pthread_create(): threads creation failed");
		}
		cerr << "Thread" << i <<":"<< pid[i] << "is spawned \n";
	}

	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;

	for(unsigned i = 0; i < POOL_SIZE; i++){
		//阻塞主进程，直到线程加入
		if(pthread_join(pid[i], NULL) < 0){
			DiewithMessage("Called pthread_join(): threads join failed");
		}
	}

}
