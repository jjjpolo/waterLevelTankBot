#pragma once

class Mutex 
{
  public:
    Mutex();
    void lock();
    void unlock() ;
  private:
    volatile bool m_locked;
};
