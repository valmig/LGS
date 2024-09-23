/***************************************************************
 * Name:      LGSMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Miguel Valbuena ()
 * Created:   2015-12-17
 * License:   GPL v3.0
 **************************************************************/

#include "LGSMain.h"
#include <wx/msgdlg.h>
//#include "LGS.h"
#include <thread>
#include <fstream>



//(*InternalHeaders(LGSFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "valDialogs.cpp"



/*
//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}
*/

//(*Idinit(LGSFrame)
const long LGSFrame::ID_BUTTON1 = wxNewId();
const long LGSFrame::ID_BUTTON2 = wxNewId();
const long LGSFrame::ID_CHOICE1 = wxNewId();
const long LGSFrame::ID_TEXTCTRL3 = wxNewId();
const long LGSFrame::ID_PANEL1 = wxNewId();
const long LGSFrame::ID_TEXTCTRL1 = wxNewId();
const long LGSFrame::ID_TEXTCTRL2 = wxNewId();
const long LGSFrame::idMenuQuit = wxNewId();
const long LGSFrame::ID_MENUITEM1 = wxNewId();
const long LGSFrame::ID_MENUITEM2 = wxNewId();
const long LGSFrame::idMenuAbout = wxNewId();
const long LGSFrame::ID_STATUSBAR1 = wxNewId();
//*)


// ==================================================
wxDEFINE_EVENT(MY_EVENT, MyThreadEvent);
//===================================================


LGSFrame::LGSFrame(wxWindow* parent,wxWindowID id)
{
    //*Initialize(LGSFrame)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer3;
    wxBoxSizer* BoxSizer4;
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, id, _("LGS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    {
        if (val::FileExists(iconpath)) {
            wxIcon FrameIcon;
            FrameIcon.CopyFromBitmap(wxBitmap(wxImage(iconpath)));
            SetIcon(FrameIcon);
        }
        else if (val::FileExists(alticonpath)) {
            wxIcon FrameIcon;
            FrameIcon.CopyFromBitmap(wxBitmap(wxImage(alticonpath)));
            SetIcon(FrameIcon);
        }
    }
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    ComputeButton = new wxButton(Panel1, ID_BUTTON1, _("Compute"), wxPoint(8,0), wxSize(120,28), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    ComputeButton->SetToolTip(_("Ctrl-L"));
    ClearButton = new wxButton(Panel1, ID_BUTTON2, _("Clear"), wxPoint(140,0), wxSize(120,28), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    ClearButton->SetToolTip(_("Ctrl-D"));
    Choice = new wxChoice(Panel1, ID_CHOICE1, wxPoint(270,0), wxSize(132,28), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice->SetSelection( Choice->Append(_("rational")) );
    Choice->Append(_("modq"));
    Choice->Append(_("double"));
    Choice->Append(_("complex"));
    Choice->Append(_("integer"));
    Choice->Append(_("gauss numbers"));
    Choice->Append(_("rational function"));
    Choice->SetToolTip(_("number field"));
    TextCtrlmodq = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("30011"), wxPoint(410,0), wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    TextCtrlmodq->SetToolTip(_("prime for modq"));
    TextCtrlmodq->SetHelpText(_("prime for modq"));
    BoxSizer2->Add(Panel1, 1, wxALL|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    I_TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(480,169), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    I_TextCtrl->SetToolTip(_("Hier LGS eintragen"));
    BoxSizer3->Add(I_TextCtrl, 1, wxALL|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    O_TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(480,189), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    O_TextCtrl->SetToolTip(_("Lösungsmenge hier"));
    BoxSizer4->Add(O_TextCtrl, 1, wxALL|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer4, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("Increase Font Size\tCtrl-+"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu3, ID_MENUITEM2, _("Decrease Font Size\tCtrl--"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuBar1->Append(Menu3, _("Settings"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();

    /*
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LGSFrame::OnComputeButtonClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LGSFrame::OnClearButtonClick);
    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&LGSFrame::OnChoiceSelect);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&LGSFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&LGSFrame::OnMenuIncrease);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&LGSFrame::OnMenuDecrease);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&LGSFrame::OnAbout);
    */

    Bind(wxEVT_COMMAND_BUTTON_CLICKED,&LGSFrame::OnComputeButtonClick,this,ID_BUTTON1);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED,&LGSFrame::OnClearButtonClick, this, ID_BUTTON2);
    Bind(wxEVT_COMMAND_CHOICE_SELECTED,&LGSFrame::OnChoiceSelect, this, ID_CHOICE1);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnQuit, this, idMenuQuit);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnMenuIncrease, this, ID_MENUITEM1);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnMenuDecrease, this, ID_MENUITEM2);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnAbout, this, idMenuAbout);
    //*)

    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1001);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1002);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1003);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1004);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1005);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1006);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::OnNumberfieldChoice,this,1007);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::ComputeSimplex,this,1008);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&LGSFrame::ComputeEvaluation,this,1009);


    wxAcceleratorEntry entries[12];

    entries[0].Set(wxACCEL_CTRL, (int) 'L',ID_BUTTON1);
    entries[1].Set(wxACCEL_CTRL, (int) 'D',ID_BUTTON2);
    entries[2].Set(wxACCEL_CTRL,WXK_RETURN,ID_BUTTON1);
    entries[3].Set(wxACCEL_ALT,(int) 'R',1001);
    entries[4].Set(wxACCEL_ALT,(int) 'M',1002);
    entries[5].Set(wxACCEL_ALT,(int) 'D',1003);
    entries[6].Set(wxACCEL_ALT,(int) 'C',1004);
    entries[7].Set(wxACCEL_ALT,(int) 'I',1005);
    entries[8].Set(wxACCEL_ALT,(int) 'G',1006);
    entries[9].Set(wxACCEL_ALT,(int) 'F',1007);
    entries[10].Set(wxACCEL_CTRL,(int) 'S',1008);
    entries[11].Set(wxACCEL_CTRL,(int) 'E',1009);
    //entries[2].Set(wxACCEL_CTRL, (int) '+',ID_MENUITEM1);
    //entries[3].Set(wxACCEL_CTRL, (int) '-',ID_MENUITEM2);
    wxAcceleratorTable accel(12,entries);
    SetAcceleratorTable(accel);


    Bind(MY_EVENT,&LGSFrame::OnMessageEvent,this,IdMessage);
    GetSizeSettings();
    if (xsize!=-1) {
        SetClientSize(wxSize(xsize,ysize));
        Move(wxPoint(xpos,ypos));
        wxFont myfont(I_TextCtrl->GetFont());
        myfont.SetPointSize(fontsize);
        I_TextCtrl->SetFont(myfont);
        O_TextCtrl->SetFont(myfont);
    }
    I_TextCtrl->SetFocus();
}

LGSFrame::~LGSFrame()
{
    using namespace std;
    //std::ofstream file(val::GetExeDir()+filesep+"settings.txt",ios::out | ios::trunc);
    std::ofstream file(settingsfile,ios::out | ios::trunc);
    if (file) {
        wxSize MySize=GetClientSize();
        wxPoint MyPoint=GetPosition();
        file<<MyPoint.x<<endl;
        file<<MyPoint.y<<endl;
        file<<MySize.x<<endl;
        file<<MySize.y<<endl;
        file<<I_TextCtrl->GetFont().GetPointSize();
        file.close();
    }
    //(*Destroy(LGSFrame)
    //*)
}

void LGSFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void LGSFrame::OnAbout(wxCommandEvent& event)
{
    //wxString msg = wxbuildinfo(long_f);
    //wxMessageBox(msg, _("Welcome to..."));
    wxMessageBox(_("programmed by Miguel Valbuena."),_("LGS"));
}

void LGSFrame::GetSizeSettings()
{
    //std::ifstream file(val::GetExeDir()+filesep+"settings.txt",std::ios::in);
    std::ifstream file(settingsfile,std::ios::in);
    if (!file) {
        if (!val::DirExists(valdir)) {
            if (!val::CreateDir(valdir)) wxMessageBox("Cannot Create\n" + valdir);
        }
        if (!val::DirExists(settingsdir)) {
             if (!val::CreateDir(settingsdir)) wxMessageBox("Cannot Create\n" + settingsdir);
        }
        return;
    }
    file>>xpos>>ypos>>xsize>>ysize>>fontsize;
    file.close();
}




void LGSFrame::OnComputeButtonClick(wxCommandEvent& event)
{
    Compute();
}

void LGSFrame::Compute()
{
    if (iscomputing) return;

    iscomputing = 1;

    std::string s;
    s=TextCtrlmodq->GetValue();
    //int p=stringtonumber<int>(s);
    int p=val::FromString<int>(s);

    if (p<0) p*=-1;

    if (!val::isprime(p)) p = val::nextprime(p);

    TextCtrlmodq->SetValue(val::ToString(p));

    IOText=I_TextCtrl->GetValue();

    //lgsmain(s);

    O_TextCtrl->Clear();

    //MyOutput=O_TextCtrl;
    //MyButton = ComputeButton;
    MyFrame = this;
    ComputeButton->Disable();
    numberfield = Choice->GetSelection();
    l_computation = lcomputation::LGS;

    std::thread t(lgsmain,std::ref(IOText),std::ref(numberfield),p);
    t.detach();
}

void LGSFrame::ComputeSimplex(wxCommandEvent &)
{
    if (iscomputing) return;
    iscomputing = 1;

    IOText=I_TextCtrl->GetValue();
    O_TextCtrl->Clear();
    MyFrame = this;
    ComputeButton->Disable();
    std::thread t(simplex,std::ref(IOText));
    t.detach();
}


void LGSFrame::ComputeEvaluation(wxCommandEvent &)
{
    if (iscomputing) return;
    val::MultiLineDialog dialog(this,"","Entry Expression",240,fontsize + 10,"Evaluate Matrix-Expression",fontsize,1);

    if (dialog.ShowModal() == wxID_CANCEL) return;
    std::string expression(dialog.GetValue());
    int p=val::FromString<int>(std::string(TextCtrlmodq->GetValue()));

    if (p<0) p*=-1;

    if (!val::isprime(p)) p = val::nextprime(p);

    TextCtrlmodq->SetValue(val::ToString(p));

    iscomputing = 1;

    IOText=I_TextCtrl->GetValue();
    O_TextCtrl->Clear();
    MyFrame = this;
    ComputeButton->Disable();
    l_computation = lcomputation::EVAL;
    lexpression = expression;
    std::thread t(evaluation, std::ref(IOText), expression, numberfield,p);
    t.detach();
}


void LGSFrame::OnClearButtonClick(wxCommandEvent& event)
{
    I_TextCtrl->Clear();
    O_TextCtrl->Clear();
}


void LGSFrame::OnMessageEvent(MyThreadEvent& event)
{
     Choice->SetSelection(numberfield);
     O_TextCtrl->Clear();
     O_TextCtrl->WriteText(event.GetMessage());
     O_TextCtrl->SetInsertionPoint(0);

     ComputeButton->Enable();
     iscomputing = 0;
     //wxMessageBox(event.GetMessage());
}

void LGSFrame::OnChoiceSelect(wxCommandEvent& event)
{
    numberfield = Choice->GetSelection();
    switch (l_computation)
    {
        case lcomputation::UNDEF: break;
        case lcomputation::LGS:
        {
            Compute();
        } break;
        case lcomputation::EVAL:
        {
            if (iscomputing) return;
            int p = val::FromString<int>(std::string(TextCtrlmodq->GetValue()));

            if (p < 0) p *= -1;

            if (!val::isprime(p)) p = val::nextprime(p);

            TextCtrlmodq->SetValue(val::ToString(p));

            iscomputing = 1;

            IOText = I_TextCtrl->GetValue();
            O_TextCtrl->Clear();
            MyFrame = this;
            ComputeButton->Disable();
            l_computation = lcomputation::EVAL;
            std::thread t(evaluation, std::ref(IOText), lexpression, numberfield, p);
            t.detach();
        } break;
        default: break;
    }
    //Compute();
}

void LGSFrame::OnMenuIncrease(wxCommandEvent& event)
{
    wxFont myfont(I_TextCtrl->GetFont());
    int mysize=myfont.GetPointSize();

    mysize+=1;
    if (mysize>20) mysize=20;
    myfont.SetPointSize(mysize);
    I_TextCtrl->SetFont(myfont);
    O_TextCtrl->SetFont(myfont);
    I_TextCtrl->Refresh();
    O_TextCtrl->Refresh();
}

void LGSFrame::OnMenuDecrease(wxCommandEvent& event)
{
    wxFont myfont(I_TextCtrl->GetFont());
    int mysize=myfont.GetPointSize();

    mysize-=1;
    if (mysize<9) mysize=9;
    myfont.SetPointSize(mysize);
    I_TextCtrl->SetFont(myfont);
    O_TextCtrl->SetFont(myfont);
    I_TextCtrl->Refresh();
    O_TextCtrl->Refresh();
}

void LGSFrame::OnNumberfieldChoice(wxCommandEvent &event)
{
    int id = event.GetId();
    id %= 1000;
    Choice->SetSelection(id-1);
    numberfield = Choice->GetSelection();
    switch (l_computation)
    {
        case lcomputation::UNDEF: break;
        case lcomputation::LGS:
        {
            Compute();
        } break;
        case lcomputation::EVAL:
        {
            if (iscomputing) return;
            int p = val::FromString<int>(std::string(TextCtrlmodq->GetValue()));

            if (p < 0) p *= -1;

            if (!val::isprime(p)) p = val::nextprime(p);

            TextCtrlmodq->SetValue(val::ToString(p));

            iscomputing = 1;

            IOText = I_TextCtrl->GetValue();
            O_TextCtrl->Clear();
            MyFrame = this;
            ComputeButton->Disable();
            l_computation = lcomputation::EVAL;
            std::thread t(evaluation, std::ref(IOText), lexpression, numberfield, p);
            t.detach();
        } break;
        default: break;
    }
}
