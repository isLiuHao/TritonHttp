/******************************************************************************************
Project: UCSD CSE291C Course Project: Web Server for TritonHTTP

handTCPClient:
This module handles incoming connection, and message parsing and framing.
Handle error in data format level.

Data in valid format will be passed to responser for content verification and response
*******************************************************************************************/
#include "handleTCPClient.hpp"
using namespace std;

/****************************************
  Client Handler Threads for ThreadPool
**************************************/
void *HandleTCPClient(void *args){
    ThreadArgs *arg = static_cast<ThreadArgs *>(args);
    int servSock = arg->servSocket;
    string doc_root = arg->doc_root;
    unsigned int clntLen;         //客户端地址的长度
    int clntSock;                 //客户端套接字
    sockaddr_in echoClntAddr;     //客户端IP地址

    while(true){
        clntLen = sizeof(echoClntAddr);
        cerr << "waiting for accept....." << '\n';
        //accept()函数 接受连接
  		  if ((clntSock = accept(servSock, (sockaddr *) &echoClntAddr, &clntLen)) < 0){
  		      DiewithMessage_t("accept() failed");
        }

        //设置套接字接收超时
        struct timeval timeOutVal;
        timeOutVal.tv_sec = 5;
        timeOutVal.tv_usec = 0;
        if(setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOutVal, sizeof(timeOutVal)) < 0){
            cerr << strerror(errno) << '\n';
            DiewithMessage("Called setsockopt(): socket option set failed"); /*socket creation failed*/
        }
        //inet_ntoa()将二进制地址转化为172.18.56.32形式
        string addr(inet_ntoa(echoClntAddr.sin_addr));
        cerr << "Handling client " + addr << '\n';

        //开始请求处理过程
        HandleReq(clntSock, doc_root);
    }
}

void HandleReq(int clntSock, string doc_root){
    // 对传入的字节进行分帧和解析，解析为请求数据结构
    // 处理超时错误
    char buffer[BUFSIZE];
    memset(buffer, 0, sizeof(buffer));
    Framer framer;
    Parser parser;
    Responder responder(clntSock, doc_root);
    ssize_t numBytesRcvd;   //64位无符号整型
    string msg;

    //recv函数接收消息存入buffer数组
    numBytesRcvd = recv(clntSock, buffer, BUFSIZE, 0);
    if(numBytesRcvd < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)){
        /* Insert handle timeout error response */
        cerr << "time out error" << '\n';
        responder.sendResponse(CLIENT_ERROR); //400
        close(clntSock);
        return;
    }


    while(numBytesRcvd > 0){
        string input(buffer, numBytesRcvd);
        framer.append(input);
        while(framer.hasMessage()){
            msg = framer.topMessage();
            framer.popMessage();
            cerr << msg << '\n';
            if(!parser.parse(msg)){
              /* Insert handle invalid header error response */
              cerr << "header issue" << '\n';
              cerr << msg << '\n';
              responder.sendResponse(CLIENT_ERROR);
              close(clntSock);
              return;
            }

            /* Produce response based on header provided*/
            while(parser.isInstr()){
              cerr << "parse" << '\n';
              int status = responder.verifyandAppendReq(parser.getReqHeader());
              responder.sendResponse(status);
            }
            //
            // if(parser.isTerminated()){
            //   cout << "terminated" << '\n';
            //   break;
            // }

        }

        if(parser.isTerminated()){
            cout << "terminated" << '\n';
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        numBytesRcvd = recv(clntSock, buffer, BUFSIZE, 0);
        if(numBytesRcvd < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)){
            /* Insert Handle timeout error response*/
            cerr << "time out" << '\n';
            responder.sendResponse(CLIENT_ERROR);
            close(clntSock);
            return;
        }
        cerr << numBytesRcvd << '\n';
    }

    /* Connection: close detected, close socket and return */
    cerr << "reach out of loop" << '\n';
    close(clntSock);
    cerr << "socket close" << '\n';
    return;
}
