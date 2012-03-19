#ifndef _SSCHEDULING_ENGINE_H
#define _SSCHEDULING_ENGINE_H

#include "scheduling-engine.h"
#include "../../shared/singleton.h"

class SSchedulingEngine: public SchedulingEngine
{
    friend class Singleton<SSchedulingEngine>;

    private:
        SSchedulingEngine() : SchedulingEngine() {};
        virtual ~SSchedulingEngine() {};
};

#define s_sched_engine Singleton<SSchedulingEngine>::GetInstance()

#endif
