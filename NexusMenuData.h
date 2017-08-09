#include "NexusMenuBase.h"

class CNexusMenuData
{
public:
    CNexusMenuData(string strMenuTitle);
    ~CNexusMenuData();
    void AddMenuItem(CNexusMenuBase* pMenuItem);
    bool RunSelections(string strInput, CNexusUserInterface *pNexusUserInterface);
    bool Help(bool bForceShowMenu);
    string GetPrompt();

protected:
    bool RunSelection(string strInput, CNexusUserInterface *pNexusUserInterface);
    vector<string> GetCommandList(string strInput);
    void SplitInput(string strInput, string *command, string *value);
    void PrintError(string strInput, vector<CNexusMenuBase*> pMenuItems);
    vector<CNexusMenuBase*> GetMenuSelection(string strInput);

    bool bMenuShown;
    string m_strMenuTitle;
    vector <CNexusMenuBase*> m_vMenu;
};
