#include "Mutex.h"

Mutex::Mutex()
{
    m_locked = false;
}

void Mutex::lock()
{
    while (m_locked)
        ;            // Wait until the mutex is available
    m_locked = true; // Set the mutex to locked
}

void Mutex::unlock()
{
    m_locked = false; // Set the mutex to unlocked
}