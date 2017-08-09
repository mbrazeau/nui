#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>

#include "NexusMenuData.h"
#include "EditLineHist.h"
#include "mpl.h"

CNexusMenuData::CNexusMenuData(string strMenuTitle)
{
    bMenuShown = false;
    m_strMenuTitle = strMenuTitle;
}

CNexusMenuData::~CNexusMenuData()
{
    vector<CNexusMenuBase*>::iterator it;
    CNexusMenuBase* pMenuItem;
   
    for (it = m_vMenu.begin(); it < m_vMenu.end(); it++)
    {
        pMenuItem = *it;
        if (pMenuItem)
        {
            delete(pMenuItem);
        }
    }
    m_vMenu.clear();
}

void CNexusMenuData::AddMenuItem(CNexusMenuBase* pMenuItem)
{
    m_vMenu.push_back(pMenuItem);
}

bool CNexusMenuData::RunSelections(string strInput, CNexusUserInterface *pNexusUserInterface)
{
    bool ret = true;
    vector<string> commands = GetCommandList(strInput);
    vector<string>::reverse_iterator it;
    for (it = commands.rbegin(); it != commands.rend(); ++it)
    {
        ret = RunSelection(*it, pNexusUserInterface);
        if (ret == false)
        {
            break;
        }
    }

    return ret;
}

bool CNexusMenuData::RunSelection(string strInput, CNexusUserInterface *pNexusUserInterface)
{
    vector<CNexusMenuBase*> pMenuItems;
    string command;
    string value;
    SplitInput(strInput, &command, &value);
    pMenuItems = GetMenuSelection(command);
    if (pMenuItems.size() == 1)
    {
        bool bRet = true;
        cout<<endl;
        try
        {
            switch (pMenuItems[0]->RunCommand(pNexusUserInterface, value))
            {
            case ENXS_MCS_COMMAND_FAIL:
                bRet = false;
                break;

            case ENXS_MCS_INVALID_PARAM:
                PrintError(strInput, pMenuItems);
                break;

            default:
                break;
            }
        }
        catch (const char *e)
        {
            cout<<"NUI Error: "<<e<<endl;
        }
        cout<<endl;
        return bRet;
    }
    PrintError(strInput, pMenuItems);

    return true;
}

bool CNexusMenuData::Help(bool bForceShowMenu)
{
    if ((bMenuShown == false) || (bForceShowMenu == true))
    {
        vector<CNexusMenuBase*>::iterator it;
        CNexusMenuBase* pMenu;
        string output;
       
        for (it = m_vMenu.begin(); it < m_vMenu.end(); it++)
        {
            pMenu = *it;
            if (pMenu)
            {
                pMenu->GetMenuOutput(&output);
                cout<<output<<endl;
            }
        }
        cout<<endl;
        bMenuShown = true;
    }
    return true;
}

string CNexusMenuData::GetPrompt()
{
    return m_strMenuTitle;
}

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

vector<string> CNexusMenuData::GetCommandList(string strInput)
{
    vector<string> ret = split(strInput, ';');
    
    return ret;
}

void CNexusMenuData::SplitInput(string strInput, string *command, string *value)
{
    *command = CEditLineHist::trim(strInput);
    size_t index = strInput.find('=');
    if (index != string::npos)
    {
        *command = strInput.substr(0, index);
        *value = strInput.substr(index+1);
        CEditLineHist::trim(*command);
        CEditLineHist::trim(*value);
    }
}

void CNexusMenuData::PrintError(string strInput, vector<CNexusMenuBase*> pMenuItems)
{
    vector<CNexusMenuBase*>::iterator it;
    cout<<" Unknown command: '"<<strInput<<"'"<<endl<<endl;
    CNexusMenuBase* pMenuItem;
    string params;
    string output;
    for (it = pMenuItems.begin(); it < pMenuItems.end(); it++)
    {
        pMenuItem = *it;
        pMenuItem->GetMenuOutput(&output);
        pMenuItem->GetValidParams(&params);
        cout<<output<<endl;
        if (params.length() > 0)
        {
            cout<<setw(COMMAND_WIDTH - 2)<<" "<<"Possible values:"<<endl;
            cout<<setw(COMMAND_WIDTH)<<" "<<params<<endl;
        }
    }
    cout<<endl;
}

vector<CNexusMenuBase*> CNexusMenuData::GetMenuSelection(string strInput)
{
    vector<CNexusMenuBase*>::iterator it;
    CNexusMenuBase* pMenuItem;
    vector<CNexusMenuBase*> ret;
    for (it = m_vMenu.begin(); it < m_vMenu.end(); it++)
    {
        pMenuItem = *it;
        if ((pMenuItem) && (pMenuItem->IsSelection(strInput)))
        {
            ret.push_back(pMenuItem);
        }
    }
    return ret;
}
