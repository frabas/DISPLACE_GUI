#ifndef THREAD_VESSELS_H
#define THREAD_VESSELS_H

void thread_vessel_init (int numthreads);
void thread_vessel_prepare();
void thread_vessel_insert_job(int idx);
void thread_vessel_wait_completed();
void thread_vessel_deinit();

#endif // THREAD_VESSELS_H
