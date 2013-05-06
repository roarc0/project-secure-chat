#ifndef METHOD_REQUEST_H
#define METHOD_REQUEST_H

class MethodRequest
{
    public:
        MethodRequest(){};
        virtual ~MethodRequest() {};

        virtual int Call()=0;
};

#endif
