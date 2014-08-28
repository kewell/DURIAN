#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

//#define SOCK_STREAM 2
#define MYPORT 3490 /* the port users will be connecting to */
#define BACKLOG 10 /* how many pending connections queue will hold */

#define BUFFER_LEN 40 * 1024 //40 * 1024
int g_BUFFER_CNT  = 5 * 512; // 5 * 1024
int g_TIME_INTERVAL = 60;
int g_ucSendCountAsMB = 1;

//#defien WRITE_FILE

void start_send (int socket_send);
double gettime();
void PRINT_INFO(char *string);


struct timezone1
{
    int tz_minuteswest;     /* Minutes west of GMT.  */
    int tz_dsttime;     /* Nonzero if DST is ever in effect.  */
};

static struct timeval _tstart;
static struct timeval _tend;
struct timezone _tz;

int sockfd, new_fd; /* listen on sock_fd, new connection on new_fd */
unsigned char Tx_BUF[BUFFER_LEN];
sem_t m_hSem;

double last_time, current_time;
double total_time;
long total_size;
#ifdef WRITE_FILE
FILE *m_fpFileHandle;
#endif
int m_ErrorHappan;
int send_num;

void tstart (void)
{
    gettimeofday (&_tstart, &_tz);
}

void tend (void)
{
    gettimeofday (&_tend, &_tz);
}

double tval ()
{
    double t1, t2;

    t1 = (double) _tstart.tv_sec + (double) _tstart.tv_usec / (1000 * 1000);
    t2 = (double) _tend.tv_sec + (double) _tend.tv_usec / (1000 * 1000);
    printf("T_Start=%10.10f T_end=%10.10f   ", t1, t2);
    return (t2 - t1);
}

void *thread_send (void *lParam)
{
    //int fd = (int)(*lParam);	
    start_send(new_fd);
}

void start_send_task(int new_fd)
{
    pthread_t thread_id;
    int result;

    result = pthread_create(&thread_id, NULL, thread_send, (void*)&new_fd);

    if (result != 0 || thread_id == 0)
    {
        printf ("%s EROOR *********pthread_create failed*********\n",__FUNCTION__);
        //创建线程失败
        if (EAGAIN == result)
            printf ("%s EROOR *********pthread_create failed with EAGAIN (thread num limit arrived)*********\n",__FUNCTION__);
        else if (EINVAL == result)
            printf ("%s ERROR *********pthread_create failed with EINVAL (create thread with invalid parameters)***********.\n", __FUNCTION__);
        else if (ENOMEM == result)
            printf ("%s ERROR *********pthread_create failed with ENOMEM (no memeory to create thread)*********.\n",__FUNCTION__);
        else
            printf ("%s ERROR *********pthread_create failed with unkonen error code %d *************\n",__FUNCTION__, result);
    }    

    pthread_detach(thread_id);
}

void start_send (int socket_send)
{
    int nLoopCnt = 0;
    static int iThread = 0;
    char putout_info[150];
    int nSend;

    iThread += 1;

    tstart ();
    while (nLoopCnt < g_BUFFER_CNT)
    {
        nSend = write (socket_send, Tx_BUF, BUFFER_LEN);
        if (nSend == -1)
        {
            //    	  printf("ssslll %d\n",errno);
            if (errno == EPIPE)
            {
                PRINT_INFO("Socked Closed\n"); 	
            }
            else if (errno == EINTR)
                continue;
            else
            {
                sprintf (putout_info,"Send error = %d\n", errno); 
                PRINT_INFO(putout_info);
            }
            m_ErrorHappan = 1;
            sem_post (&m_hSem);
            return;
        }
        nLoopCnt ++;
    }
    tend ();

    total_time = tval();
    printf("Totoal time=%10.10f\n", total_time);
    total_size = BUFFER_LEN / 1024 * g_BUFFER_CNT / 1024;

    printf("(%03d) Total send %dMB to remote client in %3.3f seconds , Speed = %.2f Mbps \n", \
            send_num,total_size, total_time, ((double)total_size)*8 / (total_time));

    total_time = 0;
    total_size = 0;
    send_num ++;
#if 0
    current_time = gettime();
    if ((current_time - last_time) >= g_TIME_INTERVAL && 0)  // Change by WENJING 2014-04-22 16:45:26 
    {
        sprintf(putout_info,"(%03d) Total send %dMB to remote client in %3.3f seconds , Speed = %.2f Mbps \n", \
                send_num,total_size, total_time, ((double)total_size)*8 / (total_time));
        PRINT_INFO( putout_info );
        last_time = gettime();
        total_time = 0;
        total_size = 0;
        send_num ++;
    }
    else if (0)
    {
    	printf ("(%d)send %dMB to remote client in %3.3f seconds = %.2f Mbps \n", \
    		      iThread, g_BUFFER_CNT * BUFFER_LEN  /(1024*1024), tval(), g_BUFFER_CNT * BUFFER_LEN /(1024* 1024) / tval());
    }
#endif
    sem_post (&m_hSem);
}

int main(int argc, char* argv[])
{
    double begin_time;
    char putout_info[100];

    sem_init (&m_hSem, 0, 0);
    int run_time = 1;

    if (argc > 1)
    {
        run_time = atoi(argv[1]);
        if (run_time < 1) run_time = 1;
    }

    if (argc > 2)
    {
        g_ucSendCountAsMB = atoi(argv[2]);
    }
    printf("Once print should be %dMB\n", g_ucSendCountAsMB*100);
    g_BUFFER_CNT = g_ucSendCountAsMB * 5 * 512;

    run_time *= 3600;           //将小时换成秒
    struct sockaddr_in my_addr; /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    int sin_size;

    total_time = 0;
    total_size = 0;
    m_ErrorHappan = 0;
    send_num = 1;

#ifdef WRITE_FILE
    m_fpFileHandle = fopen("/server_info", "w+");

    if (NULL == m_fpFileHandle)
    {
        printf("Open file mac_info failed\n");
        return 0;	
    }    
#endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0); /* do some error checking! */
    if (-1 == sockfd)
    {
        printf("socket create failed.\n");
        return 1;
    }                                   
    int iSet = 1;                               
    setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &iSet, sizeof (iSet));
    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_port = htons(MYPORT); /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    bzero(&(my_addr.sin_zero), 8); /* zero the rest of the struct */

    /* don't forget your error checking for these calls: */
    if (-1 == bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)))
    {
        printf("socket bind failed.\n");
        return 1;
    }

    if (-1 == listen(sockfd, BACKLOG))
    {
        printf("socket bind failed.\n");
        return 1;
    }

    sin_size = sizeof(struct sockaddr_in);

    printf("wait client connect...\n");
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size);
    if (-1 == new_fd)
    {
        printf("socket bind failed.\n");
        return 1;
    }
    sprintf (putout_info,"server accept remote connet with new_fd = %d\n", new_fd);
    PRINT_INFO(putout_info);
    int iCount = 0;
    memset (Tx_BUF, 's', BUFFER_LEN);

    begin_time = gettime();
    current_time = begin_time;
    last_time = begin_time;

    while(((current_time - begin_time) < (double)run_time) && (!m_ErrorHappan))
    {
        start_send_task(new_fd);
        //start_send (new_fd);
        while (sem_wait(&m_hSem));
        sleep(1);
        current_time = gettime();
    }
    PRINT_INFO ("network test over\n");


    close(new_fd);
    close(sockfd);
#ifdef WRITE_FILE    
    fclose(m_fpFileHandle);
#endif    
    sem_destroy(&m_hSem);
}

double gettime()
{
    static struct timeval _curtime;
    struct timezone _ctz;
    double ctime;  

    gettimeofday (&_curtime, &_ctz);
    ctime = (double) _curtime.tv_sec + (double) _curtime.tv_usec / (1000 * 1000);

    return ctime;
}

void PRINT_INFO(char *string)
{
    printf(string);

#ifdef WRITE_FILE
    fwrite(string, strlen(string), 1, m_fpFileHandle);
    //	  system("sync");	
    fflush(m_fpFileHandle);
#endif

}
