#pragma once
#include <QString>

/* Header ini untuk menampung kelas-kelas Singleton */

class DbVar
{
private:
    QStringList m_value;
    static DbVar *s_instance;

public:
    QStringList get_value(){ return m_value; }

    void set_value(int v, QString w)
    {
        m_value.insert(v, w);
    }

    static DbVar *instance()
    {
        if (!s_instance)
            s_instance = new DbVar;
        return s_instance;
    }

};

class SysVar
{
private:
    QStringList m_value;
    static SysVar *s_instance;

public:
    QStringList get_value(){ return m_value; }

    void set_value(int v, QString w)
    {
        m_value.insert(v, w);
    }

    static SysVar *instance()
    {
        if (!s_instance)
            s_instance = new SysVar;
        return s_instance;
    }

};
