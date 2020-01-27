#include <sys/stat.h>
#include "NexusUserInterface.h"

/* 
 * Use the preprocessor to define a derived class for each command,
 * the derived class implements what some people would call a functionoid.
 * A functionoid is basically a pure virtual function in a base class
 * that is overloaded a bunch of times in derived classes, with each
 * derived class implementing different functionality. In this case
 * the different functionality is all of the commands... The overloaded
 * functioniod MenuFunction simply calls a function in the CNexusUserInterface
 * class, that function is different for each derived class and is always takes
 * the name of the "type" parameter (Ex: Spacer, OpenNexusFile, SaveFile, etc...)
 *
 * The behavior of the MenuFunction is defined to return true if the
 * program is to continue to operate, and false if the program should
 * terminate. Possibly due to errors, or if the user selects the quit 
 * command.
 *
 * Note: The preprocessor ## concatenation operator is used to create
 * the class name based on the value of the "type" macro parameter
 */

#ifdef _WINDOWS
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#define NEW_COMMAND_DEFINE_CAST(type, cast) \
    class type : public CNexusMenuBase \
    { \
    public:\
        type(const char * strCommand, const char * strHelpText) : CNexusMenuBase(strCommand, strHelpText){}\
        type(const char * strCommand, const char * strHelpText, map<const char*, int, ltstr> assignments) : CNexusMenuBase(strCommand, strHelpText, assignments){}\
        type(const char * strCommand, const char * strHelpText, vector<int>    assignments) : CNexusMenuBase(strCommand, strHelpText, assignments){}\
        bool MenuFunction(CNexusUserInterface *pNexusUserInterface, string *value, int nMappedVal)\
        {\
            return pNexusUserInterface->f##type(value, (cast)nMappedVal);\
        }\
    };
#define NEW_COMMAND_DEFINE(type) NEW_COMMAND_DEFINE_CAST(type, int)

#define MAKE_STR_VECTOR(slist)  vector<string>(slist, slist + (sizeof(slist) / sizeof(slist[0])))
#define MAKE_INT_VECTOR(slist)  vector<int>(slist, slist + (sizeof(slist) / sizeof(slist[0])))

/*
 * The followind actually defines the derived class for each command on the main menu
 */
NEW_COMMAND_DEFINE(CNexusMenuSpacer         )
NEW_COMMAND_DEFINE(CNexusMenuOpenNexusFile  )
NEW_COMMAND_DEFINE(CNexusMenuSaveFile       )
NEW_COMMAND_DEFINE(CNexusMenuCloseNexusFile )

NEW_COMMAND_DEFINE(CNexusMenuHelp           )
NEW_COMMAND_DEFINE(CNexusMenuQuit           )
NEW_COMMAND_DEFINE(CNexusMenuAbout          )
NEW_COMMAND_DEFINE(CNexusMenuCommandLog     )
NEW_COMMAND_DEFINE(CNexusMenuStatus         )
NEW_COMMAND_DEFINE(CNexusMenuChdir          )

NEW_COMMAND_DEFINE(CNexusMenuExclude        )
NEW_COMMAND_DEFINE(CNexusMenuInclude        )
NEW_COMMAND_DEFINE(CNexusMenuOutgroup       )
NEW_COMMAND_DEFINE(CNexusMenuIngroup        )
NEW_COMMAND_DEFINE(CNexusMenuChar           )

NEW_COMMAND_DEFINE(CNexusMenuHeuristicSearch)
NEW_COMMAND_DEFINE(CNexusMenuExhaust        )
NEW_COMMAND_DEFINE(CNexusMenuBNB            )
NEW_COMMAND_DEFINE(CNexusMenuStepwise       )
NEW_COMMAND_DEFINE(CNexusMenuBootstrap      )
NEW_COMMAND_DEFINE(CNexusMenuJackknife      )
NEW_COMMAND_DEFINE(CNexusMenuSTR            )

NEW_COMMAND_DEFINE(CNexusMenuConsens        )
//NEW_COMMAND_DEFINE(CNexusMenuCollapse       )
NEW_COMMAND_DEFINE(CNexusMenuReport         )

/*
 * The following actually defines the derived class for each command on the set menu
 */
NEW_COMMAND_DEFINE_CAST(CNexusMenuBranchSwapType, mpl_bbreak_t)
NEW_COMMAND_DEFINE_CAST(CNexusMenuAddSeqType    , mpl_addseq_t)
NEW_COMMAND_DEFINE_CAST(CNexusMenuRatchetSearch , bool)
//NEW_COMMAND_DEFINE_CAST(CNexusMenuCollapseAt    , mfl_set_collapse_at_t)
//NEW_COMMAND_DEFINE_CAST(CNexusMenuCollapseZero  , bool)
NEW_COMMAND_DEFINE(CNexusMenuNumReplicates)
NEW_COMMAND_DEFINE(CNexusMenuTreeLimit)
//NEW_COMMAND_DEFINE_CAST(CNexusMenuRatchetSearch , bool)
NEW_COMMAND_DEFINE_CAST(CNexusMenuGap,            mpl_gap_t)
NEW_COMMAND_DEFINE_CAST(CNexusMenuHold,           int)

NEW_COMMAND_DEFINE(CNexusMenuMainMenu       )

/*
 * The UI constructor puts the menu together, it stores each
 * menu option in a stl vector.
 */
CNexusUserInterface::CNexusUserInterface()
{
    m_mflHandle = NULL;
    m_pNexusParse = NULL;
    m_strCwd = "./";
    m_pMainMenu = new CNexusMenuData("morphy>");
    if (!m_pMainMenu)
    {
        throw "Unable to allocate memory for main menu";
    }
    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "File"));
    m_pMainMenu->AddMenuItem(new CNexusMenuOpenNexusFile   ("open"          , "Open a nexus file"));
    m_pMainMenu->AddMenuItem(new CNexusMenuCloseNexusFile  ("close"         , "Close a nexus file"));
    m_pMainMenu->AddMenuItem(new CNexusMenuSaveFile        ("save"          , "Save according to the options"));

    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "Program"));
    m_pMainMenu->AddMenuItem(new CNexusMenuCommandLog      ("log"           ,"Toggles record of commands, variable states, etc"));
    m_pMainMenu->AddMenuItem(new CNexusMenuStatus          ("status"        ,"Prints status of all current settings, eg. logmode on/off"));
    m_pMainMenu->AddMenuItem(new CNexusMenuChdir           ("cd"            ,"change working directory"));
    m_pMainMenu->AddMenuItem(new CNexusMenuHelp            ("help"          , "Help"));
    m_pMainMenu->AddMenuItem(new CNexusMenuAbout           ("about"         , "About"));
    m_pMainMenu->AddMenuItem(new CNexusMenuQuit            ("quit"          , "Quit"));

    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "Data"));
    m_pMainMenu->AddMenuItem(new CNexusMenuExclude         ("exclude"       , "Exclude taxa or characters"));
    m_pMainMenu->AddMenuItem(new CNexusMenuInclude         ("include"       , "Include excluded taxa or characters"));
    m_pMainMenu->AddMenuItem(new CNexusMenuOutgroup        ("outGroup"      , "Assign taxa to outgroup"));
    m_pMainMenu->AddMenuItem(new CNexusMenuIngroup         ("inGroup"       , "Return taxa from outgroup to ingroup"));
    m_pMainMenu->AddMenuItem(new CNexusMenuChar            ("char"          , "Modify a character's type"));

    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "Analysis"));
    m_pMainMenu->AddMenuItem(new CNexusMenuHeuristicSearch ("heuristic"     , "Begin a heuristic search"));
//    m_pMainMenu->AddMenuItem(new CNexusMenuRatchetSearch   ("ratchet"       , "Begin a ratchet search"));
    m_pMainMenu->AddMenuItem(new CNexusMenuExhaust         ("exhaustive"    , "Begin an exhaustive search"));
    m_pMainMenu->AddMenuItem(new CNexusMenuBNB             ("branchbound"   , "Begin a branch-and-bound search"));
    m_pMainMenu->AddMenuItem(new CNexusMenuStepwise        ("stepwise"      , "Begin a stepwise search"));
    m_pMainMenu->AddMenuItem(new CNexusMenuBootstrap       ("bootStrap"     , "Begin a bootstrap analysis"));
    m_pMainMenu->AddMenuItem(new CNexusMenuJackknife       ("jackKnife"     , "Begin a jackknife analysis"));
    m_pMainMenu->AddMenuItem(new CNexusMenuSTR             ("reduction"     , "Perform a safe taxonomic reduction"));

    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "Results"));
    m_pMainMenu->AddMenuItem(new CNexusMenuConsens         ("consensus"     , "Compute consensus tree for trees in memory"));
    //m_pMainMenu->AddMenuItem(new CNexusMenuCollapse        ("collapse"      , "Collapse zero-length branches, condense the tree set"));
    m_pMainMenu->AddMenuItem(new CNexusMenuReport          ("report"        , "Print a report about the current open nexus file"));

    const int   NumReps       [] = {0, 10000000};
    const int   TreeLimit     [] = {0, 10000000};
	const int	holdLimit	  [] = {0, 10000000};

    m_pMainMenu->AddMenuItem(new CNexusMenuSpacer      (NULL, "Parameters"));
    ConfigMenuBranchSwapType();
    ConfigMenuAddSeqType();
    ConfigMenuRatchetSearch();
    //CJD FIXME: ConfigMenuCollapseAt();
    //CJD FIXME: ConfigMenuCollapseZero();
    m_pMainMenu->AddMenuItem(new CNexusMenuNumReplicates    ("nreps"         , "Set the number of replications for a heuristic search", MAKE_INT_VECTOR(NumReps)));
    m_pMainMenu->AddMenuItem(new CNexusMenuTreeLimit        ("treeLimit"     , "Set the maximum number of trees allowed to be stored in memory", MAKE_INT_VECTOR(TreeLimit)));
	m_pMainMenu->AddMenuItem(new CNexusMenuHold				("hold"	         , "Set the number of trees to hold during stepwise addition", MAKE_INT_VECTOR(holdLimit)));
    //CJD FIXME: ConfigMenuRatchetSearch();
    ConfigMenuGap();

    m_ioCommands = new CEditLineHist("nui1234567890", &m_fCommandLog);
    m_ioInputFiles = new CEditLineHist("nui12345678901234567890", &m_fCommandLog);
    m_ioLogFiles = new CEditLineHist("nui1234567890", &m_fCommandLog);
    m_ioWorkingDir = new CEditLineHist("nui1234567890", &m_fCommandLog);
    m_ioNumericSubCommands = new CEditLineHist("nui1234567890", &m_fCommandLog);
}

void CNexusUserInterface::ConfigMenuBranchSwapType()
{
    map<const char*, int, ltstr> selections;
    selections["TreeBisection"] = MPL_TBR_T;
    selections["TBR"] = MPL_TBR_T;
    selections["SubtreePruning"] = MPL_SPR_T;
    selections["SPR"] = MPL_SPR_T;
    m_pMainMenu->AddMenuItem(new CNexusMenuBranchSwapType   ("branchSwap"    , "Set branch swap type for heuristic searches", selections));
}

void CNexusUserInterface::ConfigMenuAddSeqType()
{
    map<const char*, int, ltstr> selections;
//    selections["Simple"] = PAWM_AST_SIMPLE;
    selections["Random"] = MPL_AST_RANDOM;
    selections["AsIs"] = MPL_AST_ASIS;
    selections["None"] = MPL_AST_INMEM;
//    selections["Closest"] = PAWM_AST_CLOSEST;
    m_pMainMenu->AddMenuItem(new CNexusMenuAddSeqType       ("addSeq"        , "Selects the manner in which branches are added during the generation of starting trees", selections));
}

/* CJD FIXME:
void CNexusUserInterface::ConfigMenuCollapseAt()
{
    map<const char*, int, ltstr> selections;
    selections["MaxIs0"] = MFL_SC_MAX_LEN;
    selections["MinIs0"] = MFL_SC_MIN_LEN;
    selections["Equal"] = MFL_SC_EQUAL_RECONSTRUCTION_SETS;
    m_pMainMenu->AddMenuItem(new CNexusMenuCollapseAt       ("collapseAt"    , "Configure when to collapse nodes", selections));
}
*/
/* CJD FIXME:
void CNexusUserInterface::ConfigMenuCollapseZero()
{
    map<const char*, int, ltstr> selections;
    selections["Collapse"] = true;
    selections["NoCollapse"] = false;
    m_pMainMenu->AddMenuItem(new CNexusMenuCollapseZero     ("collapseZero"  , "Enable collapsing of zero length branches during search", selections));
}
*/
/* CJD FIXME:*/
// MDB: Fixed?
void CNexusUserInterface::ConfigMenuRatchetSearch()
{
    map<const char*, int, ltstr> selections;
    selections["Yes"] = true;
    selections["No"] = false;
    m_pMainMenu->AddMenuItem(new CNexusMenuRatchetSearch    ("ratchet"       , "Set the ratchet search parameter", selections));
}

void CNexusUserInterface::ConfigMenuGap()
{
    map<const char*, int, ltstr> selections;
    selections["Inapplicable"] = GAP_INAPPLIC;
    selections["Missing"] = GAP_MISSING;
    selections["State"] = GAP_NEWSTATE;
    m_pMainMenu->AddMenuItem(new CNexusMenuGap              ("gap"           , "Set whether gap symbol ('-') will be treated as inapplicability or as missing data", selections));
}

void CNexusUserInterface::ChangeMenu(CNexusMenuData *pMenu)
{
    m_pMenu = pMenu;
//    fCNexusMenuHelp(NULL, 0, false);
}

void CNexusUserInterface::Delete(CEditLineHist *pMem)
{
    if (pMem)
    {
        delete (pMem);
    }
}

/*
 * The UI destructor deletes all memory.
 */
CNexusUserInterface::~CNexusUserInterface()
{
    delete m_pMainMenu;
    fCNexusMenuCloseNexusFile(NULL, 0, false);
    if (m_fCommandLog)
    {
        m_fCommandLog.close();
    }
    DestroyHandle();
    Delete(m_ioCommands);
    Delete(m_ioInputFiles);
    Delete(m_ioLogFiles);
    Delete(m_ioWorkingDir);
    Delete(m_ioNumericSubCommands);
}

/*
 * Actually read input from the user, and issue the selected commands
 */
void CNexusUserInterface::DoMenu()
{
    bool ret = true;
    string strInput;
    fCNexusMenuAbout(NULL, 0, false);
    ChangeMenu(m_pMainMenu);
    do
    {
        try
        {
            strInput.clear();
            m_ioCommands->GetUserInput(m_pMenu->GetPrompt(), &strInput, false);
            ret = m_pMenu->RunSelections(strInput, this);
        }
        catch (const char *e)
        {
            cout<<endl<<"Error: "<<e<<endl;
        }
        catch (ifstream::failure &e)
        {
            cout<<endl<<"Error: "<<e.what()<<endl;
        }
        catch (NxsException &e)
        {
            cout<<endl<<"Error: "<<e.nxs_what()<<endl;
        }
        catch (const std::exception &e)
        {
            cout<<endl<<"Error: "<<e.what()<<endl;
        }
        catch (...)
        {
            cout<<endl<<"Error: Unknown exception"<<endl;
        }
    } while (ret);
}

/*
 * Run the open file command, just prompt the user for input
 * and attempt to read the nexus file
 */
bool CNexusUserInterface::fCNexusMenuOpenNexusFile(string *value, int nMappedVal)
{
    string strFilename = *value;

    if (!m_pNexusParse)
    {
        if (strFilename.length() == 0)
        {
            m_ioInputFiles->GetUserInput(" Enter filename: " + m_strCwd, &strFilename, true);
        }
        
        // Check if the input isn't a path by checking for slashes
        if (strFilename.find_first_of("\\/") == string::npos)
        {
            strFilename = m_strCwd + strFilename;
        }
        
        m_pNexusParse = new CNexusParse();
        if (m_pNexusParse)
        {
            if (m_pNexusParse->ReadNexusFile(&strFilename, NULL))
            {
                CreateHandle();
                cout<<" "<<strFilename<<" opened successfully"<<endl;
            }
            else
            {
                fCNexusMenuCloseNexusFile(NULL, false);
                cout<<" Error: Unable to read "<<strFilename<<endl;
            }
        }
        else
        {
            cout<<" Error: Unable to open "<<strFilename<<endl;
        }
    }
    else
    {
        cout<<" Error: Nexus file "<<m_pNexusParse->m_cNexusReader->GetInFileName()<<" is already open"<<endl;
    }
    return true;
}

void CNexusUserInterface::DestroyHandle()
{
    if (m_mflHandle)
    {
//        delete m_mflHandle;
//        m_mflHandle = NULL;
        mpl_handle_delete(&m_mflHandle);
    }
}

void CNexusUserInterface::CreateHandle()
{
    DestroyHandle();

    int err = 0;
    
    stringstream ss;
    int nTax = m_pNexusParse->m_cTaxa->GetNTax();
    int nChar = m_pNexusParse->m_cChars->GetNCharTotal();
    int i;
    std::string tmp;
    const char* cstr;

    m_mflHandle = mpl_handle_new();//new PAWM(nTax, nChar);
    
    if (m_mflHandle != NULL)
    {
        err = 0;
        err = mpl_set_dimensions(nTax, nChar, m_mflHandle);
        if (err < 0)
        {
            cout << "Unable to set dimensions. Likely causes: " << endl;
            if (nTax < 3 && nTax > 0)
            {
                cout << "- Input specifies fewer than 3 taxa." << endl;
            }
            else if (nChar < 1 || nTax < 0)
            {
                cout << "- Number of characters less than 1" << endl;
                cout << "- Illogical (i.e. negative) matrix dimensions." << endl;
            }
            else
            {
                cout << "- Insufficient memory." << endl;
            }
        }
    }
    else
    {
        cout << "Error: unable to initialise Morphy. " << endl <<
        "Likely insufficient memory for operation." << endl;
        return;
    }
    
    for (i = 0; i < nTax; i++)
    {
        m_pNexusParse->m_cChars->WriteStatesForTaxonAsNexus(ss, i, 0, nChar);
    }
    ss << ";";
    
    tmp = ss.str();
    cstr = tmp.c_str();
    
    err = 0;
    err = mpl_attach_rawdata(MPL_DISCR_T, cstr, m_mflHandle);
    if (err < 0)
    {
        cout << "Unable to read data. Check your file for errors." << endl;
    }
        
}

bool CNexusUserInterface::SaveTranslateTable(myofstream &fSave)
{
    int it;
    int n_taxa = m_pNexusParse->m_cTaxa->GetNumTaxonLabels();
    for (it = 0; it < n_taxa; it++) 
    {
        fSave<<"\t\t"<<it + 1<<" ";
        if (m_pNexusParse->m_cTaxa->NeedsQuotes(it)) 
        {
            fSave<<"'"<<m_pNexusParse->m_cTaxa->GetTaxonLabel(it)<<"'";
        }
        else 
        {
            fSave<<m_pNexusParse->m_cTaxa->GetTaxonLabel(it);
        }
        if (it < n_taxa-1) 
        {
            fSave<<",";
        }
        fSave<<endl;
    }
    return true;
}

bool CNexusUserInterface::SaveNewickStrings(myofstream &fSave)
{
    // TODO: Re-implement
    if (m_mflHandle)
    {
        std::vector<std::string> newicks;
        if (mpl_get_num_trees(m_mflHandle) > 0)
        {
            int i = 0;
            for (i = 0; i < mpl_get_num_trees(m_mflHandle); i++)
            {
                char* newick = mpl_get_newick(i, m_mflHandle);
                fSave<<"\t\tTREE Morphy_"<< i+1 <<" = [&U] " << newick <<endl;
                // TODO: Free the string
            }
        }
        else
        {
            cout<<"Unable to get newick trees."<<endl;
        }
    }
    else
    {
        cout<<"No file open."<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuSaveFile(string *value, int nMappedVal)
{
    string strFilename = *value;
    myofstream fSave;

    if (strFilename.length() == 0)
    {
        m_ioLogFiles->GetUserInput(" Enter save filename: " + m_strCwd, &strFilename, true);
    }
    strFilename = m_strCwd + strFilename;
    fSave.open(strFilename.c_str());
    if (fSave)
    {
        fSave<<"#NEXUS"<<endl;
        fSave<<"BEGIN TREES;"<<endl<<"\tTRANSLATE"<<endl;

        SaveTranslateTable(fSave);

        fSave<<"\t\t;"<<endl<<endl;

        SaveNewickStrings(fSave);

        fSave<<"END;";
        fSave.close();
        cout<<" Successfully opened '"<<strFilename<<"'"<<endl;
    }
    else
    {
        cout<<" Error: Unable to open '"<<strFilename<<"'"<<endl;
    }
    return true;
}

/*
 * Close a file opened with the OpenNexusFile command
 * bVerbose is defaulted to = true
 */
bool CNexusUserInterface::fCNexusMenuCloseNexusFile(string *value, int nMappedVal, bool bVerbose)
{
    if (m_pNexusParse)
    {
        delete m_pNexusParse;
        m_pNexusParse = NULL;
        if (bVerbose)
        {
            cout<<" Successfully closed file..."<<endl;
        }
    }
    else if (bVerbose)
    {
        cout<<" Error: No file is currently open"<<endl;
    }
    DestroyHandle();

    return true;
}

/*
 * Print the menu
 */
bool CNexusUserInterface::fCNexusMenuHelp           (string *value, int nMappedVal, bool bForceShowMenu)
{
    m_pMenu->Help(bForceShowMenu);
    return true;
}

bool CNexusUserInterface::fCNexusMenuQuit           (string *value, int nMappedVal)
{
    cout<<"Goodbye!"<<endl;
    return false;
}

bool CNexusUserInterface::fCNexusMenuAbout          (string *value, int nMappedVal, bool bShowBuildTime)
{
    cout<<endl;
    cout<<"Morphy NUI Version: "<<morphy_nui_v_MAJOR_VERSION <<"."<<morphy_nui_v_MINOR_VERSION;
#ifdef BETA
    cout << " beta";
#endif
    cout<<endl;
    cout<<"Copyright 2019 (C) Martin Brazeau and Chris Desjardins. All rights reserved."<<endl;
    cout<<"This program uses the NCL by Paul O. Lewis."<<endl<<endl;
    cout<<"For a list of commands enter the command \"help\""<<endl<<endl;
    if (bShowBuildTime)
    {
        cout<<"Build time: "<<__DATE__<<" "<<__TIME__<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuCommandLog     (string *value, int nMappedVal)
{
    string strFilename = *value;
    
    if (!m_fCommandLog)
    {
        if (strFilename.length() == 0)
        {
            m_ioLogFiles->GetUserInput(" Enter log filename: " + m_strCwd, &strFilename, true);
        }
        strFilename = m_strCwd + strFilename;
       
        m_fCommandLog.open(strFilename.c_str());
        if (m_fCommandLog)
        {
            cout<<" Successfully opened '"<<strFilename<<"'"<<endl;
        }
        else
        {
            cout<<" Error: Unable to open '"<<strFilename<<"'"<<endl;
        }
    }
    else
    {
        cout<<" Error: Log file "<<m_fCommandLog.GetFileName()<<" is already open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuStatus         (string *value, int nMappedVal)
{
    string strNexusFile = "File not open";

    if ((m_pNexusParse) && (m_pNexusParse->m_cNexusReader))
    {
        strNexusFile = m_pNexusParse->m_cNexusReader->GetInFileName();
    }
    cout<<endl;
    cout<<"Nexus file: "<<strNexusFile<<endl;
    cout<<"Command log file: "<<m_fCommandLog.GetFileName()<<endl;
    cout<<"Working directory: "<<m_strCwd<<endl;
    cout<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuChdir          (string *value, int nMappedVal)
{
    string strCwd = *value;
    struct stat st;

    if (strCwd.length() == 0)
    {
        m_ioWorkingDir->GetUserInput(" Enter new working directory: ", &strCwd);
    }
    if (strCwd[strCwd.length() - 1] != '/')
    {
        strCwd += "/";
    }
    if ((stat(strCwd.c_str(), &st) == 0) && (S_ISDIR(st.st_mode)))
    {
        cout<<" Setting working directory to: "<<strCwd<<endl;
        m_strCwd = strCwd;
    }
    else
    {
        cout<<" Error: Invalid directory '"<<strCwd<<"'"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuExclude        (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuInclude        (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuOutgroup       (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuIngroup        (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuChar           (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

void CNexusUserInterface::PrintIslandData()
{
    /* CJD FIXME:
    mfl_add_sequence_t addseq_type = (mfl_add_sequence_t)(long int)mfl_get_parameter(m_mflHandle, MFL_PT_ADD_SEQUENCE_TYPE);
    if (addseq_type == MFL_AST_RANDOM)
    {
        int count = mfl_get_resultant_data(m_mflHandle, MFL_RT_ISLAND_COUNT,  0);
        int size;
        int len;
        cout<<left<<setw(10)<<"Island"<<setw(10)<<"Size"<<setw(10)<<"Length"<<endl;
        for (int i = 0; i < count; i++)
        {
            size = mfl_get_resultant_data(m_mflHandle, MFL_RT_ISLAND_SIZE ,  i);
            len  = mfl_get_resultant_data(m_mflHandle, MFL_RT_ISLAND_LENGTH, i);
            cout<<setw(10)<<i<<setw(10)<<size<<setw(10)<<len<<endl;
        }
    }
    */
}

void CNexusUserInterface::PrintHsearchData()
{
#if 0
    CJD FIXME:
    cout <<endl<<endl<<"Heuristic search completed" << endl;
    //int rearr = mfl_get_resultant_data(m_mflHandle, MFL_RT_NUM_REARRANGMENTS, 0);
    int savedtr = mfl_get_resultant_data(m_mflHandle, MFL_RT_NUM_SAVED_TREES, 0);
    int bestlen = mfl_get_resultant_data(m_mflHandle, MFL_RT_SHORTEST_TREE_LEN, 0);
    /* We don't want to report rearrangements until we can set user-defined 
     * seeds. Otherwise, the number may vary between machines.*/
    //cout << "    Number of rearrangements tried: "<<rearr<<endl;
    cout << "    Number of trees saved: "<<savedtr<<endl;
    cout << "    Length of shortest tree: "<<bestlen<<endl<<endl;
#endif
}

bool CNexusUserInterface::fCNexusMenuHeuristicSearch(string *value, int nMappedVal)
{
    int err = 0;
    
    if (m_mflHandle)
    {
        // TODO: Make sure this is straightened out
        err = mpl_do_search(m_mflHandle);
        if (err == 0)
        {
            PrintHsearchData();
            PrintIslandData();
        }
        else
        {
            cout << "Unable to initiate search." << endl;
        }
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuExhaust        (string *value, int nMappedVal)
{
    // TODO: Exhaustive probably will never be implemented
    cout<<"Not implemented"<<endl;
    return true;
//    if (m_mflHandle)
//    {
//        m_mflHandle->searchType(PAWM_SEARCH_EXHAUSTIVE);
//        m_mflHandle->doSearch();
//    }
//    else
//    {
//        cout<<"No file open"<<endl;
//    }
    return true;
}

// TODO: Implement this one day
bool CNexusUserInterface::fCNexusMenuBNB            (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
//    if (m_mflHandle)
//    {
//        m_mflHandle->searchType(PAWM_SEARCH_EXHAUSTIVE);
//        m_mflHandle->doSearch();
//    }
//    else
//    {
//        cout<<"No file open"<<endl;
//    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuStepwise            (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
//    if (m_mflHandle)
//    {
//        m_mflHandle->searchType(PAWM_SEARCH_STEPWISE);
//        m_mflHandle->doSearch();
//    }
//    else
//    {
//        cout<<"No file open"<<endl;
//    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuBootstrap      (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuJackknife      (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuSTR            (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuConsens        (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuCollapse       (string *value, int nMappedVal)
{
    cout<<"Not implemented"<<endl;
    return true;
}

bool CNexusUserInterface::fCNexusMenuReport(string *value, int nMappedVal)
{
    if (m_pNexusParse)
    {
        m_pNexusParse->Report();
    }
    else
    {
        cout<<"No file is currently open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuBranchSwapType (string *value, mpl_bbreak_t nMappedVal)
{
    // CJD FIXME: mfl_set_parameter(m_mflHandle, MFL_PT_BRANCH_SWAP_TYPE, (void*)nMappedVal);
    if (m_mflHandle)
        {
            mpl_set_brswap(nMappedVal, m_mflHandle);
        }
        else
        {
            cout<<"No file open"<<endl;
        }
    return true;
}

bool CNexusUserInterface::fCNexusMenuAddSeqType     (string *value, mpl_addseq_t nMappedVal)
{
    if (m_mflHandle)
    {
//        m_mflHandle->addSeqType(nMappedVal);
        mpl_set_addseq(nMappedVal, m_mflHandle);
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}
/*
bool CNexusUserInterface::fCNexusMenuCollapseAt           (string *value, mfl_set_collapse_at_t nMappedVal)
{
    mfl_set_parameter(m_mflHandle, MFL_PT_COLLAP_AT, (void*)nMappedVal);
    return true;
}
*/
/* CJD FIXME:
bool CNexusUserInterface::fCNexusMenuCollapseZero         (string *value, bool nMappedVal)
{
    mfl_set_parameter(m_mflHandle, MFL_PT_COLLAPSE, (void*)nMappedVal);
    return true;
}
*/
bool CNexusUserInterface::fCNexusMenuNumReplicates        (string *value, unsigned long nMappedVal)
{
    if (m_mflHandle)
    {
        mpl_set_numreps(nMappedVal, m_mflHandle);
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuTreeLimit        (string *value, unsigned long nMappedVal)
{
    if (m_mflHandle)
    {
        mpl_set_maxtrees(nMappedVal, m_mflHandle);
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}

/* CJD FIXME:*/
// MDB: Fixed?
bool CNexusUserInterface::fCNexusMenuRatchetSearch        (string *value, bool nMappedVal)
{
    mpl_use_ratchet(nMappedVal, m_mflHandle);
    return true;
}

bool CNexusUserInterface::fCNexusMenuGap                  (string *value, mpl_gap_t nMappedVal)
{
    if (m_mflHandle)
    {
//        m_mflHandle->gapMethod(nMappedVal);
        mpl_set_gap_handl(nMappedVal, m_mflHandle);
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuHold                  (string *value, int nMappedVal)
{
    if (m_mflHandle)
    {
//        m_mflHandle->hold(nMappedVal);
        mpl_set_hold(nMappedVal, m_mflHandle);
    }
    else
    {
        cout<<"No file open"<<endl;
    }
    return true;
}

bool CNexusUserInterface::fCNexusMenuMainMenu       (string *value, int nMappedVal)
{
    ChangeMenu(m_pMainMenu);
    return true;
}

