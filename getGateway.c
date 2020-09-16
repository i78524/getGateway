/******************************************************************************/
/* main進入點                                                                 */
/* Author:YiWen Chen                                                          */
/* Last update: 2020/07/29                                                    */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <math.h>



/*******************************************************************************
int sendUART(char* sendstring)
Iface         為 欲查詢的網卡 例如："eth0"
outputGateWay 為 查詢的到的結果
*******************************************************************************/
void gatewayFromroute(char* Iface, char* outputGateWay) {

  // 組成指令
  char cmdStr[24];
  memset(cmdStr, 0, sizeof(cmdStr));
  sprintf(cmdStr, "route -n");
  
  // 讀檔
  char buf[128];
  FILE *fp;
  int count = 0;
  if(!(fp = popen(cmdStr, "r"))) {
      printf("[gatewayFromroute] popen is ERROR, cmdStr is %s\n", cmdStr);
  } else {
    while((fgets(buf, sizeof(buf), fp))) {
      count++;
      //printf("[gatewayFromroute] count=%d, buf=%s", count, buf);
      // 1. 不是0.0.0.0
      if( (strncmp(&(buf[16]), "0.0.0.0", 7) != 0) ) {
        //printf("[gatewayFromroute] pass 01 \n");
        
        // 2. 是我要的網卡
        if(strlen(buf) > 66) {
          //printf("[gatewayFromroute] pass 02 \n");       
          //printf("[gatewayFromroute] strlen(Iface) =%d \n", strlen(Iface) );
          //printf("[gatewayFromroute] Iface =%s \n", Iface );
          //printf("[gatewayFromroute] &(buf[72]) =%s \n", &(buf[72]) ) ;                                
          if( (strncmp(&(buf[72]), Iface, strlen(Iface)) == 0) ) {
            // 3. ok
            //printf("[gatewayFromroute] pass 03 \n");         
            // 範例 
            //0.0.0.0         12.12.1.1       0.0.0.0         UG    1      0        0 eth2.2
            int endNo;
            int i =0;
            for(i = 16; i <50; i++) {
              if(buf[i] == ' ') {
                  endNo = i;
                  break;
              }
            }
            strncpy(outputGateWay, &(buf[16]), (endNo-16));
            printf("[gatewayFromroute] outputGateWay = %s \n", outputGateWay);                     
            break;
          }        
        }

      }
      
      
    }
    pclose(fp);    
  }
}



int main(int argc, char **argv){

  char Iface[64];
  char gateway[64];        
  memset(Iface, 0, sizeof(Iface));
  strcpy(Iface, "eth0");
  memset(gateway, 0, sizeof(gateway));

  gatewayFromroute(Iface, gateway);

  printf("[getDNS] input Iface    : %s\n", Iface);  
  printf("[getDNS] output gateway : %s\n", gateway);
}
