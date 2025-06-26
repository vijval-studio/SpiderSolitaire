#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct stack
{
    int value,open;
    struct stack *next, *prev;
}stk;

int n_add = 0;
int move_count = 0;
float time_taken;
int deck[104];

void main();

void gen()
{
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<13;j++)
        {
            deck[i*13+j]=j+1;
        }
    }
}

void shuffle()
{
    srand(time(NULL));
    for(int i=0;i<150;i++)
    {
        int a=rand()%104;
        int b=rand()%104;
        int t=deck[a];
        deck[a]=deck[b];
        deck[b]=t;
    }
}

void insertEnd(stk **p,int x,int o)
{
    stk *t=(stk*)malloc(sizeof(stk));
    t->value=x;
    t->next=NULL;
    t->prev=NULL;
    t->open=o;
    if(*p==NULL) *p=t;
    else
    {
        stk *q=*p;
        while(q->next!=NULL)
            q=q->next;
        q->next=t;
        t->prev=q;
    }
}

void init(stk **stack)
{
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<10;j++)
        {
            if(i==4&&j>=4)
                insertEnd(&stack[j],deck[i*10+j],1);
            else
                insertEnd(&stack[j],deck[i*10+j],0);

        }
    }
    for(int i=0;i<4;i++)
    {
        insertEnd(&stack[i],deck[50+i],1);
    }
}

int max(stk **stack)
{
    int m=0;
    stk *t=NULL;
    for(int i=0;i<10;i++)
    {
        t=stack[i];
        int j=0;
        for(j=0;t!=NULL;j++)
        {
            t=t->next;
        }
        if(j>m)
            m=j;
    }
    return m;
}

void display(stk **stack)
{
    int m=max(stack);
    for(int i=-1;i<=m;i++)
    {
        for(int j=0;j<10;j++)
        {
            if(i==-1)
                printf("%d\t",j);
            else
            {
                stk *st=stack[j];
                for(int k=0;k<i&&st!=NULL;k++)
                {
                    st=st->next;
                }
                if(st!=NULL)
                {
                    if(st->open==1)
                    {
                        if(st->value==1)
                            printf("A\t");
                        else if(st->value==11)
                            printf("J\t");
                        else if(st->value==12)
                            printf("Q\t");
                        else if(st->value==13)
                            printf("K\t");
                        else
                            printf("%d\t",st->value);
                    }
                    else
                        printf("X\t");
                }
                else
                {
                    printf(" \t");
                }
            }
        }
        if(i==-1)
            printf("\n");
        printf("\n");
    }
}

int count=0;

void check(stk **table)
{
    for (int i = 0; i <= 9; i++)
    {
        stk* a = table[i];
        stk* b;
        if(a==NULL)
            continue;
        while(a->next!=NULL)
        {
            a=a->next;
        }
        if(a->value==1)
        {
            int j=0;
            for(j=1;j<13&&a!=NULL;j++)
            {
                if(a->value==j)
                    a=a->prev;
                else
                    break;
            }
            if(a!=NULL&&j==13&&a->open==1&&a->value==13)
            {
                if(a->prev!=NULL)
                {
                    a->prev->open=1;
                    a->prev->next=NULL;
                }
                else
                {
                    a=a->next;
                    table[i]->next=NULL;
                    table[i]=NULL;
                }
                for(;a->next!=NULL;)
                {
                    b=a;
                    a=a->next;
                    b->prev=NULL;
                    b->next=NULL;
                    a->prev=NULL;
                    free(b);
                }
                free(a);
                count = count + 1;
            }
        }
    }
    if (count == 8)
    {
        display(table);
        printf("The game is finished\n");
        printf("You won in %d moves and %d minutes %d seconds\n\n", move_count, (int)(time_taken)/60, ((int)(time_taken))%60);
        printf("Type q to quit or any other character to play again\n");
        fflush(stdout);
        char opt;
        scanf("%c",&opt);
        if(opt == 'q')
            exit(0);
        main();
    }
}

void add(stk **table)
{
    int i;
    if(n_add == 5)
        return;
    for(i = 0; i<10; i++)
    {
        insertEnd(&table[i], deck[54+n_add*10+i], 1);
    }
    n_add++;
}

void move(stk **table)
{
    int ip, from, to;
    char cip;
    cip=getchar();
    if(cip=='\n')
        cip=getchar();
    if(cip == '0')
    {
        add(table);
        return;
    }
    else if(cip=='1')
    {
        cip=getchar();
        if(cip=='0')
            ip=10;
        else if(cip==' '||cip=='\t'||cip=='\n')
            ip=1;
        else if(cip=='1')
            ip=11;
        else if(cip=='2')
            ip=12;
        else if(cip=='3')
            ip=13;
        else
            ip=-1;
    }
    else if(cip == 'a' || cip == 'A')
        ip = 1;
    else if(cip == 'j' || cip == 'J')
        ip = 11;
    else if(cip == 'q' || cip == 'Q')
        ip = 12;
    else if(cip == 'k' || cip == 'K')
        ip = 13;
    else
        ip = (int)(cip-48);
    scanf("%d%d", &from, &to);
    cip=getchar();
    if (ip >=1 && ip <= 13 && from >= 0 && from < 10 && to >= 0 && to < 10 && from != to)
    {
        stk *pres_a = table[from];
        if(pres_a == NULL)
            return;
        while(pres_a->next != NULL)
            pres_a = pres_a->next;
        while(pres_a->prev != NULL && pres_a->value == pres_a->prev->value - 1 && pres_a->open == 1 && pres_a->value != ip)
            pres_a = pres_a->prev;
        if (pres_a->value == ip)
        {
            stk *pres_b = table[to];
            if(pres_b == NULL)
            {
                move_count++;
                if(pres_a->prev != NULL)
                {
                    pres_a->prev->open = 1;
                    pres_a->prev->next = NULL;
                }
                else
                {
                    table[from] = NULL;
                }
                pres_a->prev = NULL;
                table[to] = pres_a;
            }
            else
            {
                while(pres_b->next != NULL)
                    pres_b = pres_b->next;
                if(pres_b->value == ip+1)
                {
                    move_count++;
                    if(pres_a->prev != NULL)
                    {
                        pres_a->prev->open = 1;
                        pres_a->prev->next = NULL;
                    }
                    else
                    {
                        table[from] = NULL;
                    }
                    pres_a->prev = pres_b;
                    pres_b->next = pres_a;
                }
            }
        }
    }
}

void main()
{
    time_t start_time, end_time;
    gen();
    shuffle();
    stk *table[10];
    for(int i=0;i<10;i++)
        table[i]=NULL;
    init(table);
    start_time = time(NULL);
    while(1)
    {
        display(table);
        fflush(stdout);
        end_time = time(NULL);
        time_taken = time_taken = difftime(end_time, start_time);
        move(table);
        check(table);
        printf("\nMove Count: %d\n", move_count);
        printf("Time taken so far: %d minutes %d seconds\n\n", (int)(time_taken)/60, ((int)(time_taken))%60);
        fflush(stdout);
    }
}