class Cell
{
    public:
        Cell() {};
        ~Cell() {};

        virtual bool Update(int diff = 0);
        void Insert(Session* m_pSes);
        void Remove(Session* m_pSes);
    private:
        std::list<Session*> c_sessions;
    
        Mutex _lock_session;
        Mutex _lock_list;
}
