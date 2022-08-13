#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <winuser.h>
#include <processthreadsapi.h>
#include <string.h>
#pragma comment(lib, "winmm.lib")


// run :
// a.exe "filepath"
DWORD thread_ID;  

DWORD WINAPI PlayMusic(LPVOID data){
    char * audiofile = data;
    
    char command[MAX_PATH];
    MCIERROR err;
   
    printf("Now playing %s\n",audiofile);
    sprintf(command,"open \"%s\" type mpegvideo alias audiofile", audiofile); // constructing the command string
    
    mciSendString(command, NULL, 0, NULL ); //opening the media file
   
    err = mciSendString("play audiofile", NULL,0,NULL);//playing the file by its alias name
    if (err)
    {
       puts("Error playing file");
        
        ExitProcess(-1);
    }
    
    MSG lpMsg; //Inter-Threads message handling
    while (GetMessageA(&lpMsg, 0,0,0)){ //waiting for incoming messages sent to the thread using its unique thread id
        switch (lpMsg.message) //traiting the messages
        {
     
        case WM_APP + 1:
            printf("Pausing...\n");
            mciSendString("pause audiofile", NULL,0,NULL);
            break;
        case WM_APP + 2:
            printf("Resuming...\n");
            mciSendString("resume audiofile", NULL,0,NULL);
            break;
        case WM_APP + 3:
            printf("closing...\n");
            mciSendString("close audiofile", NULL,0,NULL);
            break;
        default:
            break;
        }
        DispatchMessageA(&lpMsg);

    }

}

int main(int argcount, char* argvalues[]){
    
    if (argcount > 1 ){
             
        HANDLE threadHandle = CreateThread(NULL, 0,PlayMusic, argvalues[1],0, &thread_ID); //create the thread that will play the music
        Sleep(500); //sleeping half a second to prevent thread racing
        if (threadHandle){ //if the thread is created the function will retun its handle
            char c;
            int test = 1;
            while (test){// waiting for user input commands p for pause r for resume and c for closing
                printf("Enter command : ");
                scanf(" %c", &c);
                switch (c ){
                    case 'p' :
                        PostThreadMessageA(thread_ID, WM_APP + 1,0 , 0);// post inter-thread message to control the player
                        Sleep(500);
                    continue;
                case 'r' :
                        PostThreadMessageA(thread_ID, WM_APP + 2,0 , 0);
                        Sleep(500);
                    continue;
                case 'c' :
                    
                        PostThreadMessageA(thread_ID, WM_APP + 3,0 , 0);
                        Sleep(500);
                        test = 0;
                    continue;

                }
            }
            CloseHandle(threadHandle);
        }
        return EXIT_FAILURE;
    }else{
        printf("%s", "NO AUDIO FILE TO READ");
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}

