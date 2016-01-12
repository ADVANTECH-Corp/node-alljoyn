/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <alljoyn/AboutData.h>
#include <alljoyn/AboutListener.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/AboutProxy.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Session.h>
#include <alljoyn/SessionListener.h>
#include <alljoyn/AllJoynStd.h>

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include "Rule.h"
#include <map>
#include <qcc/Mutex.h>

// marco
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

#define SOCKSERV_PORT		5000
#define EVENT_SOCKET_PORT		5050
#define SOCKSERV_MAX_CONN	1
#define SOCK_MSGBUF_SIZE	2000
//

#define DELETE_IF(x) { if (x) { delete x; x = NULL; } }
/*
 * Note the removal of almost all Error handling to make the sample code more
 * straight forward to read.  This is only used here for demonstration actual
 * programs should check the return values of all method calls.
 */
using namespace ajn;

static volatile sig_atomic_t s_interrupt = false;

// The interface name should be the only thing required to find and form a
// connection between the service and the client using the about feature.
static void SigIntHandler(int sig) {
    s_interrupt = true;
    exit(EXIT_FAILURE);
}

BusAttachment* g_bus;
qcc::Mutex mEventLock;
std::multimap<qcc::String, Rule*> List_Rules; 
extern std::multimap<qcc::String, RuleInfo> List_Triger;
/*
 * This will handle connection for each client
 * */
#define EVENT_JASSON_SIZE     6
const char* EVENT_JASSON_NAMES[EVENT_JASSON_SIZE] = {"busname", "path", "interface", "member" , "deviceName", "port"};

typedef struct EventJassonData {
    char busName[32];
    char path[64];
    char interface[32];
    char member[32];
    char deviceName[32];
    char nodeId[32];
    short port;
    bool eventTriger;
} MyEventJassonData; 

template <typename T>
void erase_ObjectSTL(T &MySTL)
{
		if (MySTL.size() > 0)
		{
			for (typename T::iterator iter = MySTL.begin(); iter != MySTL.end();)
			{
			    // removes all values
			    typename T::iterator erase_iter = iter++; 
					MySTL.erase(erase_iter);
			}
		}
}

template <typename T>
typename T::iterator FindLink(qcc::String target, T &MySTL)
{
 	//Find all values by key using count & find 
  typename T::size_type entries = MySTL.count(target);
  typename T::iterator ret_iter = MySTL.find(target);
  
  /* Debug */
  /*
  for(T::size_type cnt = 0; cnt != entries; ++cnt, ++ret_iter) 
  	std::cout << "-----"<<ret_iter->first << ", "<< ret_iter->second << std::endl;
	ret_iter = MySTL.find(target);	
	*/
	return ret_iter;
}


void Del_Rule(qcc::String nodeId)
{
    std::multimap<qcc::String, Rule*>::iterator it_List_Rules = FindLink<std::multimap<qcc::String, Rule*> >(nodeId, List_Rules);
    std::multimap<qcc::String, Rule*>::size_type count_target = List_Rules.count(nodeId); 

    mEventLock.Lock(MUTEX_CONTEXT);   	
    
    for(std::multimap<qcc::String, Rule*>::size_type cnt = 0; cnt != count_target; ++cnt, ++it_List_Rules) 
  	{
      it_List_Rules->second->disable();
      List_Rules.erase(it_List_Rules++);       
    }    
    mEventLock.Unlock(MUTEX_CONTEXT);         
}

void Add_Rule(MyEventJassonData *myEventData)
{
    //g_bus->EnableConcurrentCallbacks();
    RuleInfo* event = new RuleInfo( myEventData->busName, myEventData->path, \
  												myEventData->interface, myEventData->member,\
  												NULL, myEventData->deviceName, NULL, myEventData->port, myEventData->nodeId);
    RuleInfo* action = NULL;
    Rule* rule = new Rule(g_bus, event, action);

    rule->enable();
    mEventLock.Lock(MUTEX_CONTEXT);
    List_Rules.insert(std::pair<qcc::String, Rule*>(myEventData->nodeId, rule));
    mEventLock.Unlock(MUTEX_CONTEXT);
}

int FindData(char* target)
{
    int index = -1;
    for (int i = 0; i< EVENT_JASSON_SIZE; i++)
    {
      if (strcmp(target, EVENT_JASSON_NAMES[i]) == 0)
      {
        index = i;
        break;
      }
    }
    return index;
}

bool ParserJasson(MyEventJassonData *eventData, char* jassonString)
{
    bool status = false;
    const char* delim = "{},;";
		char *pch_jasson;
    pch_jasson = strtok(jassonString, delim);
    char name[128];
    char *split_char, *locatSubName, *locatSubValue;
    int locatSubNameLength, locatSubValueLength, index;

		while (pch_jasson != NULL){
         memset(name,0, 128);
         //Get Name & Value
         split_char = strstr(pch_jasson, "\":\"");
         locatSubName = (pch_jasson + 1);
         locatSubNameLength = split_char - locatSubName;
         strncpy(name, locatSubName, locatSubNameLength);
         index = FindData(name);
         if (index != -1)
         {
            char *ptr_string = 0;
            locatSubValue = (split_char + 3);
            locatSubValueLength =  strlen(locatSubValue) - 1;
            //Set eventData
            switch(index){
                case 0:
                case 2:
                case 3:
                case 4:
                    if (index == 0){
                       ptr_string = eventData->busName;
                    }else if(index == 2){
                      ptr_string = eventData->interface;
                    }else if(index == 3){
                      ptr_string = eventData->member;
                    }else if(index == 4){
                      ptr_string = eventData->deviceName;
                    }
                                       
                    if (locatSubValueLength < 32 && ptr_string != NULL){
                      strncpy(ptr_string, locatSubValue, locatSubValueLength);
                    }else{
                      printf("[EventDaemon] Error Data Length(%d).", index);
                    }
                    break;
                case 1:
                    if (locatSubValueLength < 64){
                      strncpy(eventData->path, locatSubValue, locatSubValueLength);
                    }else{
                      printf("[EventDaemon] Error Data Length(%d).", index);
                    }
                    break;
                case 5:
                    char value[32];
                    strncpy(value, locatSubValue, locatSubValueLength);
                    eventData->port = atoi(value);
                    //Last Step
                    status = true;
                    break;                                                        
            }             
         }     
			pch_jasson = strtok (NULL, delim);
		}
    return status;
}
  
QStatus alljoyn_connect(void)
{
    QStatus status;

    g_bus = new BusAttachment("AlljoynEvent", true);

    status = g_bus->Start();
    if (ER_OK == status) {
        printf("BusAttachment started.\n");
    } else {
        printf("FAILED to start BusAttachment (%s)\n", QCC_StatusText(status));
        exit(1);
    }

    status = g_bus->Connect();
    if (ER_OK == status) {
        printf("BusAttachment connect succeeded.\n");
    } else {
        printf("FAILED to connect to router node (%s)\n", QCC_StatusText(status));
        exit(1);
    }

    return status;            
}
bool init_socket(int *socket_desc, struct sockaddr_in* server, int serv_port)
{
  bool status = true;
  int socket_on = 1;
 	//Create socket
  *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (*socket_desc == -1)
	{
		  perror("Could not create socket");
      status = false;
      goto EXIT;
	}
           
  //Prepare the sockaddr_in structure
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = htonl(INADDR_ANY);
	server->sin_port = htons(serv_port);

  if((setsockopt(*socket_desc,SOL_SOCKET,SO_REUSEADDR,&socket_on,sizeof(socket_on)))<0)  
  {  
      perror("setsockopt failed");   
  } 	
  //Bind
	 if( bind(*socket_desc, (struct sockaddr *)server , sizeof(struct sockaddr)) < 0)
	 {
      status = false;
      goto EXIT;
	 }
    //Listen
   listen(*socket_desc, SOCKSERV_MAX_CONN);
EXIT:
  return status;      
}

void *triger_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[SOCK_MSGBUF_SIZE];
  	char rest_oper[10], rest_id[10];
    
  	memset(rest_oper, '0', sizeof(rest_oper));
    memset(rest_id, '0', sizeof(rest_id));
    memset(client_message, 0, SOCK_MSGBUF_SIZE);
                
    //Receive a message from client
    if((read_size = recv(sock , client_message , SOCK_MSGBUF_SIZE , 0)) > 0)
    {
      	sscanf(client_message, "%s %s ", rest_oper, rest_id);
      	printf("Got client REST oper:%s\n", rest_oper);
        printf("Got client REST id:%s\n", rest_id);
                      
        mEventLock.Lock(MUTEX_CONTEXT);
        std::multimap<qcc::String, RuleInfo>::iterator it_List_Triger = FindLink<std::multimap<qcc::String, RuleInfo> >(rest_id, List_Triger);
        std::multimap<qcc::String, RuleInfo>::size_type count_target = List_Triger.count(rest_id);        
        
        printf("==> event-triger:%d\n", count_target);
        if (count_target >0)
        {
            //Read first event-triger , send & remove it 
            memset(client_message, 0, SOCK_MSGBUF_SIZE);
            
            sprintf(client_message,"{\"id\":\"%s\",\"uniqueName\":\"%s\",\"member\":\"%s\"}", \
                   it_List_Triger->first.c_str(), it_List_Triger->second.mUniqueName.c_str(), it_List_Triger->second.mMember.c_str());
            
            write(sock , client_message , strlen(client_message));
            
            std::cout << "[MAP1] " << it_List_Triger->first.c_str() << " , " << it_List_Triger->second.mUniqueName.c_str() \
             <<" , "<< it_List_Triger->second.mMember.c_str()<< std::endl;
            // removes fisrt event-triger
    				List_Triger.erase(it_List_Triger++);
        }        
        mEventLock.Unlock(MUTEX_CONTEXT); 
    }
    
    fflush(stdout);         
    //Free the socket pointer
    close(sock);
    free(socket_desc);
    printf("Handler closed (Event Trigher)\n");  
    return 0;
}
void *event_handler(void* args)
{   
    bool status_socket = true;
  	int socket_desc , new_socket, c , *new_sock;
    struct sockaddr_in server , client;
 
    // init socket
    memset(&server, 0, sizeof(sockaddr_in));   
    status_socket = init_socket(&socket_desc, &server, EVENT_SOCKET_PORT);
    if(!status_socket)
    {
    	perror("socket bind failed");
      goto EXIT;
    } 
    
    //Accept and incoming connection
    printf("Waiting for incoming connections...(Event Trigher)\n");
    c = sizeof(struct sockaddr_in);    
    while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
        printf("=>Connection accepted\n");
                
        // if there are event trigers 
        //if(List_Triger.size() != 0)
        {
            pthread_t triger_event_thread;
            new_sock = (int*)malloc(1);
            *new_sock = new_socket;
            
            // create thread to receive & send for each socket  
            if( pthread_create( &triger_event_thread , NULL ,  triger_handler , (void*) new_sock) < 0)
            {
                perror("could not create thread: triger_handler");
                goto EXIT;
            }
            pthread_detach(triger_event_thread); 
            printf("Event Trigher Handler assigned\n");        
        }
    }
EXIT:
    return 0;    
} 

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[SOCK_MSGBUF_SIZE];
  	char rest_oper[10], rest_id[10], rest_data[2048];

    MyEventJassonData myEventData;
    bool status = false;
  	
    memset(rest_oper, '0', sizeof(rest_oper));
  	memset(rest_data, '0', sizeof(rest_data));
    memset(rest_id, '0', sizeof(rest_id));
         
    //Receive a message from client
    //while((read_size = recv(sock , client_message , SOCK_MSGBUF_SIZE , 0)) > 0 )
    if((read_size = recv(sock , client_message , SOCK_MSGBUF_SIZE , 0)) > 0 )
    {
    	sscanf(client_message, "%s %s %[^\n] ", rest_oper, rest_id, rest_data);
    	printf("Got client REST oper:%s\n", rest_oper);
      printf("Got client REST id:%s\n", rest_id);
    	printf("Got client REST data:%s\n", rest_data);
      
      strcpy(myEventData.nodeId, rest_id);
    	if (strncmp(rest_oper, "REG", strlen("REG")) == 0) {
          status = ParserJasson(&myEventData, rest_data);
          if (status){
              Add_Rule(&myEventData);
              printf("Add Rule.\n");
          }else{
              perror("Parser Jasson fail");
          }     		    	
      }else if (strncmp(rest_oper, "DEL", strlen("DEL")) == 0) {
          Del_Rule(myEventData.nodeId);  //rest_data = event member
          printf("Delete Rule.\n");
      }
      
      //Send the message back to client
      memset(client_message, 0, SOCK_MSGBUF_SIZE);
      sprintf(client_message, "%d\n", status);
      write(sock , client_message , strlen(client_message));       
    }
  
    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed\n");
    }
         
    //Free the socket pointer
    close(sock);
    free(socket_desc);    
    return 0;
}

int main(int argc, char *argv[] )
{
  /* Install SIGINT handler so Ctrl + C deallocates memory properly */
    signal(SIGINT, SigIntHandler);

// marco
    bool status_socket = true;
  	int ret = 0;
  	int socket_desc , new_socket , c , *new_sock;
  	struct sockaddr_in server , client;
    QStatus status;
    pthread_t eventHandler_thread;
  
    status = alljoyn_connect();
    if (ER_OK != status)
    {
        perror("Alljoyn connect failed");
        goto SaveExit;
    }
    
    if( pthread_create( &eventHandler_thread , NULL ,  event_handler , NULL)) 
    {
        perror("could not create eventHandler thread");
        ret = 1;
        goto SaveExit;
    }     
//												
	//Create socket
    memset(&server, 0, sizeof(sockaddr_in));   
    status_socket = init_socket(&socket_desc, &server, SOCKSERV_PORT);
    if(!status_socket)
    {
    	perror("socket bind failed");
      goto SaveExit;
    } 
    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
    {                             
        printf("Connection accepted\n");       
        pthread_t sniffer_thread;
        new_sock = (int*)malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
		        ret = 1;
            goto SaveExit;
        }
        pthread_detach(sniffer_thread);
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        printf("Handler assigned\n");
    }

    if (s_interrupt == true)
    {
        printf("Close socket\n");
	      ret = 1;
        goto SaveExit;
    }   

SaveExit:
	return ret;    
}
