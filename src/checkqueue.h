//ticoin Copyright (c) 2012 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHECKQUEUE_H
#define CHECKQUEUE_H

#include <algorithm>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

template<typename T> class CCheckQueueControl;

/** Queue for verifications that have to be performed.
  * The verifications are represented by a type T, which must provide an
  * operator(), returning a bool.
  *
  * One thread (the master) is assumed to push batches of verifications
  * onto the queue, where they are processed by N-1 worker threads. When
  * the master is done adding work, it temporarily joins the worker pool
  * as an N'th worker, until all jobs are done.
  */
template<typename T> class CCheckQueue {
private:
    //ticoin Mutex to protect the inner state
    boost::mutex mutex;

    //ticoin Worker threads block on this when out of work
    boost::condition_variable condWorker;

    //ticoin Master thread blocks on this when out of work
    boost::condition_variable condMaster;

    //ticoin The queue of elements to be processed.
    //ticoin As the order of booleans doesn't matter, it is used as a LIFO (stack)
    std::vector<T> queue;

    //ticoin The number of workers (including the master) that are idle.
    int nIdle;

    //ticoin The total number of workers (including the master).
    int nTotal;

    //ticoin The temporary evaluation result.
    bool fAllOk;

    //ticoin Number of verifications that haven't completed yet.
    //ticoin This includes elements that are not anymore in queue, but still in
    //ticoin worker's own batches.
    unsigned int nTodo;

    //ticoin Whether we're shutting down.
    bool fQuit;

    //ticoin The maximum number of elements to be processed in one batch
    unsigned int nBatchSize;

    //ticoin Internal function that does bulk of the verification work.
    bool Loop(bool fMaster = false) {
        boost::condition_variable &cond = fMaster ? condMaster : condWorker;
        std::vector<T> vChecks;
        vChecks.reserve(nBatchSize);
        unsigned int nNow = 0;
        bool fOk = true;
        do {
            {
                boost::unique_lock<boost::mutex> lock(mutex);
                //ticoin first do the clean-up of the previous loop run (allowing us to do it in the same critsect)
                if (nNow) {
                    fAllOk &= fOk;
                    nTodo -= nNow;
                    if (nTodo == 0 && !fMaster)
                        //ticoin We processed the last element; inform the master he can exit and return the result
                        condMaster.notify_one();
                } else {
                    //ticoin first iteration
                    nTotal++;
                }
                //ticoin logically, the do loop starts here
                while (queue.empty()) {
                    if ((fMaster || fQuit) && nTodo == 0) {
                        nTotal--;
                        bool fRet = fAllOk;
                        //ticoin reset the status for new work later
                        if (fMaster)
                            fAllOk = true;
                        //ticoin return the current status
                        return fRet;
                    }
                    nIdle++;
                    cond.wait(lock); //ticoin wait
                    nIdle--;
                }
                //ticoin Decide how many work units to process now.
                //ticoin * Do not try to do everything at once, but aim for increasingly smaller batches so
                //ticoin   all workers finish approximately simultaneously.
                //ticoin * Try to account for idle jobs which will instantly start helping.
                //ticoin * Don't do batches smaller than 1 (duh), or larger than nBatchSize.
                nNow = std::max(1U, std::min(nBatchSize, (unsigned int)queue.size() / (nTotal + nIdle + 1)));
                vChecks.resize(nNow);
                for (unsigned int i = 0; i < nNow; i++) {
                     //ticoin We want the lock on the mutex to be as short as possible, so swap jobs from the global
                     //ticoin queue to the local batch vector instead of copying.
                     vChecks[i].swap(queue.back());
                     queue.pop_back();
                }
                //ticoin Check whether we need to do work at all
                fOk = fAllOk;
            }
            //ticoin execute work
            BOOST_FOREACH(T &check, vChecks)
                if (fOk)
                    fOk = check();
            vChecks.clear();
        } while(true);
    }

public:
    //ticoin Create a new check queue
    CCheckQueue(unsigned int nBatchSizeIn) :
        nIdle(0), nTotal(0), fAllOk(true), nTodo(0), fQuit(false), nBatchSize(nBatchSizeIn) {}

    //ticoin Worker thread
    void Thread() {
        Loop();
    }

    //ticoin Wait until execution finishes, and return whether all evaluations where succesful.
    bool Wait() {
        return Loop(true);
    }

    //ticoin Add a batch of checks to the queue
    void Add(std::vector<T> &vChecks) {
        boost::unique_lock<boost::mutex> lock(mutex);
        BOOST_FOREACH(T &check, vChecks) {
            queue.push_back(T());
            check.swap(queue.back());
        }
        nTodo += vChecks.size();
        if (vChecks.size() == 1)
            condWorker.notify_one();
        else if (vChecks.size() > 1)
            condWorker.notify_all();
    }

    ~CCheckQueue() {
    }

    friend class CCheckQueueControl<T>;
};

/** RAII-style controller object for a CCheckQueue that guarantees the passed
 *  queue is finished before continuing.
 */
template<typename T> class CCheckQueueControl {
private:
    CCheckQueue<T> *pqueue;
    bool fDone;

public:
    CCheckQueueControl(CCheckQueue<T> *pqueueIn) : pqueue(pqueueIn), fDone(false) {
        //ticoin passed queue is supposed to be unused, or NULL
        if (pqueue != NULL) {
            assert(pqueue->nTotal == pqueue->nIdle);
            assert(pqueue->nTodo == 0);
            assert(pqueue->fAllOk == true);
        }
    }

    bool Wait() {
        if (pqueue == NULL)
            return true;
        bool fRet = pqueue->Wait();
        fDone = true;
        return fRet;
    }

    void Add(std::vector<T> &vChecks) {
        if (pqueue != NULL)
            pqueue->Add(vChecks);
    }

    ~CCheckQueueControl() {
        if (!fDone)
            Wait();
    }
};

#endif
