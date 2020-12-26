/*Team assignment 3  408415026游鎮豪 408415040 黃文龍*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct list
{
    int ID;
    int Type;
    int arrtime;
    int completed_time;
    struct list *next;
} LType;
typedef struct queue
{
    int free;
    int count;
    int time;
    int wait;
    int num;
    int max;
    struct list *front;
    struct list *rear;
    struct list *ing;
} QType;

int prepare(int *, LType **, int *);
void generate_data(int, char[]);
void read_data(char[], LType **);
QType *creatQ(int);
void classify(QType *, QType *, QType *, LType *, int *);
void enqueue(QType *, LType *);
void start(QType *, QType *, QType *, int, LType *, int *, int *, int *);
void process(QType *, int, int *);
LType *dequeue(QType *, int *, int);
void look(QType *, int);
void copy(LType *, LType *);
int EmptyQ(QType*);
int main()
{
    int counter = 0, Time, qnum[3], totwait = 0, i, arr[3];
    LType *List = NULL;
    QType *Q1, *Q2, *Q3;
    srand(time(NULL));
    for (i = 0; i < 3; i++)
        arr[i] = 0;
    if (prepare(&Time, &List, qnum) == 0)
    {
        printf("wrong input!!!!\n");
        return 0;
    }
    /*輸入不同類型data所需的處理時間*/
    printf("Q1 workink time:\n");
    Q1 = creatQ(qnum[0]);
    printf("Q2 workink time:\n");
    Q2 = creatQ(qnum[1]);
    printf("Q3 workink time:\n");
    Q3 = creatQ(qnum[2]);
    while (counter <= Time) /*計數*/
    {
        printf("---------------------------------------------------------\n");
        printf("When time is %d:\n", counter);
        start(Q1, Q2, Q3, counter, List, qnum, &totwait, arr);
        counter++;
        printf("---------------------------------------------------------\n");
    }
    printf("total wait:%d", totwait);
}
/*提供了輸入一些屬性跟印出文字的功能，並會呼叫generate_data和read_data。*/
int prepare(int *Time, LType **List, int *qnum)
{
    int data_num, i;
    char filename[21];
    printf("Please input filename:\n"); /*輸入檔案名稱*/
    scanf("%20s", filename);
    setbuf(stdin, NULL);
    printf("Please input number of data(>=0):\n"); /*輸入data數量*/
    scanf("%d", &data_num);
    if (data_num < 0)
        return 0;
    printf("Please input max time(>=0)\n"); /*輸入最大執行時間*/
    scanf("%d", Time);
    if (*Time < 0)
        return 0;
    generate_data(data_num, filename);
    read_data(filename, List);
    /*輸入需要多少流水線*/
    printf("How many line is needed for Q1(at least 1)?\n");
    scanf("%d", &qnum[0]);
    printf("How many line is needed for Q2(at least 1)?\n");
    scanf("%d", &qnum[1]);
    printf("How many line is needed for Q3(at least 1)?\n");
    scanf("%d", &qnum[2]);
    for (i = 0; i < 3; i++)
    {
        if (qnum[i] < 1)
        {
            return 0;
        }
    }
    return 1;
}
/*會產生data的ID、類型和到達時間到使用者決定名稱的txt檔。*/
void generate_data(int num, char name[21])
{
    int i = 0, ID;
    int *array;
    array = malloc(sizeof(int) * num);
    FILE *op = fopen(name, "w");
    while (i < num)
    {
        array[i] = 0;
        i++;
    }
    i = 0;
    while (i != num)
    {
        ID = rand() % num;
        if (array[ID] == 0)
        {
            array[ID] = 1;
            fprintf(op, "%d %d %d\n", ID, rand() % 3 + 1, rand() % 20);
            i++;
        }
    }
    fclose(op);
}
/*讀取txt檔的資料，並根據到達時間以小到大的排序存到list中。*/
void read_data(char name[21], LType **List)
{
    FILE *ip = fopen(name, "r");
    LType *temp = malloc(sizeof(LType)), *last, *current;
    temp->next = NULL;
    while ((fscanf(ip, "%d %d %d", &temp->ID, &temp->Type, &temp->arrtime) != EOF))
    {
        if (*List == NULL)
        {
            *List = temp;
            temp = malloc(sizeof(LType));
            temp->next = NULL;
        }
        else if (temp->arrtime <= (*List)->arrtime)
        {
            temp->next = *List;
            *List = temp;
            temp = malloc(sizeof(LType));
            temp->next = NULL;
        }
        else
        {
            current = *List;
            while (current != NULL && temp->arrtime >= current->arrtime)
            {
                last = current;
                current = current->next;
            }
            if (current == NULL)
            {
                last->next = temp;
                temp = malloc(sizeof(LType));
                temp->next = NULL;
            }
            else
            {
                temp->next = current;
                last->next = temp;
                temp = malloc(sizeof(LType));
                temp->next = NULL;
            }
        }
    }
    fclose(ip);
}
/*創造並初始化queue且將創造出的queue回傳 */
QType *creatQ(int num)
{
    int time, i;
    scanf("%d", &time);
    QType *Q = malloc(sizeof(QType) * num);
    for (i = 0; i < num; i++)
    {
        Q[i].count = 0;
        Q[i].wait = 0;
        Q[i].num = 0;
        Q[i].max = 0;
        Q[i].front = NULL;
        Q[i].rear = NULL;
        Q[i].ing = NULL;
        Q[i].free = 1;
        Q[i].time = time;
    }
    return Q;
}
/*將list中的data根據type分類，讓他們進到不同的queue中。*/
void classify(QType *Q1, QType *Q2, QType *Q3, LType *L, int *cho)
{

    switch (L->Type)
    {
    case 1:
        enqueue(Q1, L);
        *cho = 1;
        break;
    case 2:
        enqueue(Q2, L);
        *cho = 2;
        break;
    case 3:
        enqueue(Q3, L);
        *cho = 3;
        break;
    default:
        printf("error!!\n");
        return;
        break;
    }
}
/*enqueue 將data放進queue中排隊。這裡不讓Q直接=L，因為怕會動到原先的List L*/
void enqueue(QType *Q, LType *L)
{
    if (Q->count == 0)
    {
        Q->front = malloc(sizeof(LType));
        Q->front->next = NULL;
        Q->rear = Q->front;
        copy(Q->front, L);
        Q->front->completed_time = Q->time;
        Q->count++;
    }
    else
    {
        Q->rear->next = malloc(sizeof(LType));
        Q->rear = Q->rear->next;
        Q->rear->next = NULL;
        copy(Q->rear, L);
        Q->rear->completed_time = Q->time;
        Q->count++;
    }
}
/*會呼叫classify和process，開始將資料進行處理。*/
void start(QType *Q1, QType *Q2, QType *Q3, int counter, LType *list, int *num, int *wait, int arr[3])
{
    LType *temp;
    int *cho, i, j, k;
    temp = list;
    while (temp != NULL)
    {
        if (temp->arrtime == counter) /*看是否要enqueue*/
        {
            classify(&Q1[arr[0]], &Q2[arr[1]], &Q3[arr[2]], temp, cho);
            if (*cho == 1)
                arr[0]++;
            else if (*cho == 2)
                arr[1]++;
            else if (*cho == 3)
                arr[2]++;
            for (i = 0; i < 3; i++)
            {
                if (arr[i] >= num[i])
                    arr[i] = 0;
            }
        }
        temp = temp->next;
    }
    /*看是否要dequeue*/
    printf("Type 1:\n");
    for (i = 0; i < num[0]; i++)
    {
        printf("line %d:\n", i + 1);
        process(&Q1[i], counter, wait);
    }
    printf("Type 2:\n");
    for (j = 0; j < num[1]; j++)
    {
        printf("line %d:\n", j + 1);
        process(&Q2[j], counter, wait);
    }
    printf("Type 3:\n");
    for (k = 0; k < num[2]; k++)
    {
        printf("line %d:\n", k + 1);
        process(&Q3[k], counter, wait);
    }
}
/*判斷是否能dequeue或是得等待*/
void process(QType *Q, int counter, int *wait)
{
    if (Q->free == 1 && !EmptyQ(Q))
    {
        Q->ing = dequeue(Q, wait, counter);
    }

    if (Q->free == 0)
    {
        printf("%d is processing(I will be completed at time %d)\n", Q->ing->ID, counter + Q->ing->completed_time);
        (Q->ing->completed_time)--;
        if (Q->ing->completed_time == 0) /*如果被dequeue回傳回來的data 他的completed_time被減到0 表示他已被完成*/
        {
            Q->free = 1;
            free(Q->ing);
        }
    }
    else
    {
        printf("Nothing be processed!\n");
    }

    printf("wating: ");
    look(Q, counter); /*印出queue裡的data*/
    printf("\n");
}
/*dequeue 將data移出queue 會回傳一個被處理data 回傳目的是為了數他的completed_time*/
LType *dequeue(QType *Q, int *wait, int counter)
{

    LType *del = Q->front;
    LType *temp = malloc(sizeof(LType));
    copy(temp, Q->front);
    temp->completed_time = Q->front->completed_time;
    Q->front = Q->front->next;
    Q->wait = Q->wait + counter - temp->arrtime;
    *wait = *wait + counter - temp->arrtime;
    Q->num++;
    free(del);
    Q->count--;
    Q->free = 0;
    return temp;
}
/*用來查看queue裡面的data*/
void look(QType *Q, int counter)
{
    int num = 0;
    float avg;
    LType *temp = Q->front;
    if (EmptyQ(Q))
        printf("Nothing is waiting!\n");
    else
    {
        while (temp != NULL)
        {
            printf("%d (%d s) ", temp->ID, counter - temp->arrtime);
            num++;
            temp = temp->next;
        }
    }
    if (Q->max < num)
        Q->max = num;
    if(Q->num>0)
        avg=(float)Q->wait / Q->num;
    else
    {
        avg=0;
    }
    printf("\nthe queue has  processed %d items\n", Q->num);
    /*printf("wait time of the queue:%d(only when dequeue a item，the data renew)\n",Q->wait);*/
    printf("max number of item  in the queue:%d\n", Q->max);
    printf("average time of item wait:%f\n", avg);
}
int EmptyQ(QType* Q)
{
    if(Q->count==0)
        return 1;
    else
        return 0;
}
/*複製data*/
void copy(LType *Q, LType *L)
{   
    Q->ID = L->ID;
    Q->Type = L->Type;
    Q->arrtime = L->arrtime;
}