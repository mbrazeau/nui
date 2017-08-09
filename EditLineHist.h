#pragma once

#include "myostream.h"
#ifdef _WINDOWS
#include "editline/readline.h"
#else
#include "histedit.h"
#include "readline/readline.h"
#endif

static char **my_completion_func(const char *a, int b, int c);

class CEditLineHistBase
{
public:
    CEditLineHistBase(const char *pgmName, myofstream *fCommandLog)
        : m_pfCommandLog(fCommandLog)

    {
    }
    virtual ~CEditLineHistBase() {}
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
        addHistory(pLine);
        if (*m_pfCommandLog)
        {
            (*m_pfCommandLog) << *strInput << endl;
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
    virtual void addHistory(char* pLine) = 0;
    myofstream *m_pfCommandLog;
};

#ifdef _WINDOWS

static char **my_completion_func(const char *a, int b, int c)
{
    char** ret = (char**)malloc(sizeof(char*));
    ret[0] = 0;
    return ret;
}

class CEditLineHist : public CEditLineHistBase
{
public:
    CEditLineHist(const char *pgmName, myofstream *fCommandLog)
        : CEditLineHistBase(pgmName, fCommandLog)
    {
    }
protected:
    void addHistory(char* pLine)
    {
        if (pLine)
        {
            add_history(pLine);
            free(pLine);
        }
    }
};

#else

static char **my_completion_func(const char *a, int b, int c)
{
    rl_attempted_completion_over = 1;
    return NULL;
}

class CEditLineHist : public CEditLineHistBase
{
public:
    typedef char    *(*el_pfunc_t)(EditLine *);
    CEditLineHist(const char *pgmName, myofstream *fCommandLog)
        : CEditLineHistBase(pgmName, fCommandLog)
    {
        m_pEditLine = el_init(pgmName, stdin, stdout, stderr);
        if (!m_pEditLine)
        {
            throw "Unable to initialize line editor";
        }
        el_set(m_pEditLine, EL_EDITOR, "emacs");

        m_pHistory = history_init();
        if (!m_pHistory)
        {
            throw "Unable to initialize history";
        }
        history(m_pHistory, &m_histEvent, H_SETSIZE, 800);
        el_set(m_pEditLine, EL_HIST, history, m_pHistory);
    }

    ~CEditLineHist()
    {
        if (m_pHistory)
        {
            history_end(m_pHistory);
        }
        if (m_pEditLine)
        {
            el_end(m_pEditLine);
        }
    }

protected:
    void addHistory(char* pLine)
    {
        if (pLine)
        {
            history(m_pHistory, &m_histEvent, H_ENTER, pLine);
        }
    }

    EditLine *m_pEditLine;
    History *m_pHistory;
    HistEvent m_histEvent;
    static string m_strPrompt;

private:

};
#endif
