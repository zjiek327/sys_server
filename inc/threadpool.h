#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__
 
#include <pthread.h>

#define TPOOL_DEFAULT_NUM 64
#define TPOOL_MAX_NUM 128
 
/* Ҫִ�е��������� */
typedef struct tpool_work {
    void*               (*routine)(void*);       /* ������ */
    void                *arg;                    /* �����������Ĳ��� */
    struct tpool_work   *next;                    
}tpool_work_t;
 
typedef struct tpool {
    int             shutdown;                    /* �̳߳��Ƿ����� */
    int             max_thr_num;                /* ����߳��� */
    pthread_t       *thr_id;                    /* �߳�ID���� */
    tpool_work_t    *queue_head;                /* �߳����� */
    pthread_mutex_t queue_lock;                    
    pthread_cond_t  queue_ready;    
}tpool_t;
 
/*
 * @brief     �����̳߳� 
 * @param     max_thr_num ����߳���
 * @return     0: �ɹ� ����: ʧ��  
 */
int
tpool_create(int max_thr_num);
 
/*
 * @brief     �����̳߳� 
 */
void
tpool_destroy();
 
/*
 * @brief     ���̳߳����������
 * @param    routine ������ָ��
 * @param     arg ����������
 * @return     0: �ɹ� ����:ʧ�� 
 */
int
tpool_add_work(void*(*routine)(void*), void *arg);
 
#endif
