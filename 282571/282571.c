
// 사번과 이름을 적으시오.
// 사번: 282571
// 이름: 조경미

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define err_abort(code,text) do {       					\
	fprintf(stderr, "%s at \"%s\":%d: %s\n",  			\
			text, __FILE__, __LINE__, strerror(code));  \
	abort();    																		\
}while(0)

#define errno_abort(text) do {       								\
	fprintf(stderr, "%s at \"%s\":%d: %s\n",  				\
			text, __FILE__, __LINE__, strerror(errno));		\
	abort();    																			\
} while(0)

//알람을 second기준으로 정렬해야함.
typedef struct alarm_tag {
	struct alarm_tag    *next;
	int                 seconds;
	time_t              time;   /* seconds from EPOCH */
	char                message[64]; 
} alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_cond = PTHREAD_COND_INITIALIZER;
alarm_t *alarm_list = NULL;


// 전역 변수는 최대 2개까지 추가 가능합니다.
int done=0;
// 아래의 함수를 구현하세요.
void display_list() {
	alarm_t *temp = alarm_list; //head를 가르키도록
	alarm_t *prev;
	if(alarm_list->next== NULL)
	{
		printf("[no alarm]\n");
		return;
	}

	for(temp=alarm_list->next ; temp->next != NULL ; temp=temp->next)
	{
		printf("[(%d) %c]",temp->seconds, temp->message[0]);
	}

}

// 아래의 함수를 구현하세요.
void alarm_insert (alarm_t *alarm){
	alarm_t *temp = alarm_list; //head를 가르키도록
	alarm_t *prev = temp;;
	alarm->next=NULL;
	printf("alarm second : %d\n",alarm->seconds);
	if(alarm_list->next == NULL)
	{
		alarm_list->next = alarm;
        alarm->next = NULL;
		//return;
	}
		
	else{
		for(temp=alarm_list ; temp->next != NULL ; temp=temp->next)
		{
            
			if(temp->seconds > alarm->seconds)//현재 삽입하고자 하는 알람의 시간이 더 작은경우
			{
				prev->next = alarm; //새로삽입된 알람
				alarm->next = temp;
			}
			else{
				temp->next = alarm;
			}
			prev = temp;
		}
	}
	return;
	
}

void *alarm_thread (void *arg)
{
	alarm_t *temp = alarm_list;
	struct timeval now;
    struct timespec ts;

	int status = pthread_mutex_lock (&alarm_mutex);
	if (status != 0)
		err_abort (status, "Lock mutex");

	while (1) 
	{
		//리스트를 순회하며 종료시간을 체크한다.
		for(temp=alarm_list->next ; temp != NULL ; temp=temp->next)
		{
			if(done)
			{
				gettimeofday(&now, NULL);
				ts.tv_sec = now.tv_sec + temp->seconds;
    			ts.tv_nsec = now.tv_usec * 1000;
				
				pthread_cond_timedwait(&alarm_cond, &alarm_mutex, &ts);
				temp->seconds--;
				done=0;
				pthread_mutex_unlock(&alarm_mutex);
			}
		}
	}
}

int main() {
	pthread_t thread;
	alarm_t a_input;
	int time_input[4];
	char name_input[100];
	int s=0;
	char a;
	int i=0;

	int status = pthread_create(&thread, NULL, alarm_thread, NULL);
	if (status != 0)
		err_abort (status, "Create alarm thread");
	alarm_list = (malloc(sizeof(alarm_t)*5));

	while (1) {
		printf("Alarm> ");

		char line[128];
		if (fgets(line, sizeof (line), stdin) == NULL)
			exit (0);

		if (strlen(line) <= 1)
			continue;

		if (strncmp(line, "list", 4) == 0) {
			display_list();
			continue;
		}

		// 이곳에서 알람 생성 및 리스트 추가 그리고 시그널 전송 코드를
		// 구현합니다.
		else{	
            pthread_mutex_lock(&alarm_mutex);
			while(line[i]!=' '){
				time_input[i] = (char)line[i]-'0';
				i++;
			}
			
			if(i==2) { //두자리
				s+=time_input[0]*10;
				s+=time_input[1];			
			}
			else if(i==1){
				s+=time_input[0];
			}
			else{
				printf("PLZ enter 0~99");
			}


			i++;
			while(line[i]!='\n'){
				//name_input[i] = line[i];
				name_input[i] = line[i];				
				a_input.message[i] = line[i];
				//printf("%c\n",a_input.message[i]);
				i++;
			}
			i=0;
			
			//strcpy(name_input,a_input.message);
			
			a_input.seconds = s;
			s=0;
            //구조체에 message와 seconds를 넣고삽입
			alarm_insert(&a_input);
			done=1;
            //시그널 전송
			pthread_cond_signal(&alarm_cond);

			pthread_mutex_unlock(&alarm_mutex);
            pthread_join(thread, NULL);
		}
	}
}


