#pragma once

class CEditLineHistBase
{
public:
    CEditLineHistBase(const char *pgmName, myofstream *fCommandLog)
        : m_pfCommandLog(fCommandLog)

    {
    }
    void GetUserInput(string strPrompt, string *strInput, bool filename = false)
    {
        getUserInput(strPrompt, strInput, filename);
        if (*m_pfCommandLog)
        {
            (*m_pfCommandLog) << *strInput << endl;
        }
    }
protected:
    virtual void getUserInput(string strPrompt, string *strInput, bool filename) = 0;
    myofstream *m_pfCommandLog;
};

#ifdef _WINDOWS
#include "editline/readline.h"

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
    void getUserInput(string strPrompt, string *strInput, bool filename)
    {
        char* pLine;
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
        if (pLine)
        {
            add_history(pLine);
            *strInput = pLine;
            strInput->erase(std::remove(strInput->begin(), strInput->end(), '\n'), strInput->end());
            free(pLine);
        }
    }
};

#else
#include "histedit.h"
#include "myostream.h"

const static char *prompt(EditLine *el)
{
    return " ";
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
        el_set(m_pEditLine, EL_PROMPT, &prompt);
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
    void getUserInput(string strPrompt, string *strInput, bool filename)
    {
        const char *pLine;
        int nCount;
        cout<<strPrompt;
        pLine = el_gets(m_pEditLine, &nCount);
        if (nCount > 0)
        {
            history(m_pHistory, &m_histEvent, H_ENTER, pLine);
            *strInput = pLine;
            strInput->erase(std::remove(strInput->begin(), strInput->end(), '\n'), strInput->end());
        }
        else
        {
            *strInput = "q";
            throw "Stardard input failure";            
        }
    }

    EditLine *m_pEditLine;
    History *m_pHistory;
    HistEvent m_histEvent;
    static string m_strPrompt;

private:

};
#endif