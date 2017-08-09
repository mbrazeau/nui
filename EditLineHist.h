#pragma once

#include "myostream.h"
#ifdef _WINDOWS
#include "editline/readline.h"
#else
#include "readline/readline.h"
#include "readline/history.h"
#endif

static char **my_completion_func(const char *a, int b, int c);

class CEditLineHist
{
public:
    CEditLineHist(const char *pgmName, myofstream *fCommandLog)
        : m_pfCommandLog(fCommandLog)

    {
    }
    virtual ~CEditLineHist() {}
    void GetUserInput(string strPrompt, string *strInput, bool filename = false)
    {
        char *pLine;
        if (filename == false)
        {
            rl_attempted_completion_function = my_completion_func;
        }
        else
        {
            rl_attempted_completion_function = NULL;
        }

        strPrompt.append(" ");
        pLine = readline(strPrompt.c_str());

        *strInput = pLine;
        trim(*strInput);
        if (pLine)
        {
            add_history(pLine);
            free(pLine);

            if (*m_pfCommandLog)
            {
                (*m_pfCommandLog) << *strInput << endl;
            }
        }
    }

    static string &ltrim(string &s)
    {
        s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
        return s;
    }

    static string &rtrim(string &s)
    {
        s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
        return s;
    }

    static string &trim(string &s)
    {
        return ltrim(rtrim(s));
    }

protected:
    myofstream *m_pfCommandLog;
};

#ifdef _WINDOWS

static char **my_completion_func(const char *a, int b, int c)
{
    char** ret = (char**)malloc(sizeof(char*));
    ret[0] = 0;
    return ret;
}

#else

static char **my_completion_func(const char *a, int b, int c)
{
    rl_attempted_completion_over = 1;
    return NULL;
}

#endif
