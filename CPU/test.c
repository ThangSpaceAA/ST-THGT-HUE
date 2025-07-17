#include "Parameter_CPU.h"
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>

// int main()
// {
//    char str[80] = "Hoc C - co ban va nang cao - tai VietJack\n";
//    const char s[2] = "-";
//    char *token;

//    /* lay token dau tien */
//    token = strtok(str, s);

//    /* duyet qua cac token con lai */
// //    while( token != NULL )
// //    {
//       printf( " %s\n", token );

//       token = strtok(NULL, "\n");
//       printf( " %s\n", token );
// //    }

//    return(0);
// }

// bool get_State_Cmd_Terminal(char *cmd, char *sample)
// {
//    // Lay output gan bien
//    FILE *fd;

//    int line_num = 1;
//    int find_result = 0;
//    char temp[512];

//    fd = popen(cmd, "r");
//    if (!fd)
//       return 1;
//    char buffer[1024];

//    while(fgets(temp, 512, fd) != NULL) {
// 		if((strstr(temp, sample)) != NULL) {
// 			printf("A match found on line: %d\n", line_num);
// 			printf("\n%s\n", temp);
// 			find_result++;
// 		}
// 		line_num++;
// 	}

// 	if(find_result == 0) {
// 		printf("\nSorry, couldn't find a match.\n");
// 	}

// 	//Close the file if still open.
// 	if(fd) {
// 		fclose(fd);
// 	}
//    if(line_num > 0)
//    {
//       return true;
//    }
//    return false;
// }

// int main()
// {
//    printf("%d\r\n", get_State_Cmd_Terminal("timedatectl", "active"));
//    return 0;
// }

typedef struct
{
   uint8_t hh_start;
   uint8_t mm_start;
   uint8_t hh_end;
   uint8_t mm_end;
} time_active_test_t;

time_active_test_t time_active_test;
int aa = 234;

// int main()
// {
//    int cmd;
//    while (1)
//    {
//       printf("Nhap cmd: \r\n");
//       scanf("%d", &cmd);
//       if (cmd)
//       {
//          system("sudo timedatectl set-ntp true");
//       }
//       else
//       {
//          system("sudo timedatectl set-ntp false");
//       }
//    }
// }