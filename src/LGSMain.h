/***************************************************************
 * Name:      LGSMain.h
 * Purpose:   Defines Application Frame
 * Author:    Miguel Valbuena ()
 * Created:   2015-12-17
 * License:   GPL v3.0
 **************************************************************/

#ifndef LGSMAIN_H
#define LGSMAIN_H

//(*Headers(LGSFrame)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
//*)

#include "LGS.h"

// =========================================================================================

enum lcomputation{UNDEF, LGS, EVAL};


class LGSFrame: public wxFrame
{
    public:

        LGSFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~LGSFrame();

    private:

        //(*Handlers(LGSFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnComputeButtonClick(wxCommandEvent& event);
        void OnClearButtonClick(wxCommandEvent& event);
        void OnChoiceSelect(wxCommandEvent& event);
        void OnMenuIncrease(wxCommandEvent& event);
        void OnMenuDecrease(wxCommandEvent& event);
        //*)

        //(*Identifiers(LGSFrame)
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_CHOICE1;
        static const long ID_TEXTCTRL3;
        static const long ID_PANEL1;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL2;
        static const long idMenuQuit;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(LGSFrame)
        wxButton* ClearButton;
        wxButton* ComputeButton;
        wxChoice* Choice;
        wxMenu* Menu3;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem4;
        wxPanel* Panel1;
        wxStatusBar* StatusBar1;
        wxTextCtrl* I_TextCtrl;
        wxTextCtrl* O_TextCtrl;
        wxTextCtrl* TextCtrlmodq;
        //*)

        void OnMessageEvent(MyThreadEvent& event);
        void OnNumberfieldChoice(wxCommandEvent &event);
        void Compute();
        void ComputeSimplex(wxCommandEvent&);
        void ComputeEvaluation(wxCommandEvent&);

        int xsize=-1,ysize=-1,xpos=-1,ypos=-1,fontsize=-1, iscomputing = 0, numberfield = 0, l_computation = UNDEF;
        std::string IOText, lexpression;
        void GetSizeSettings();
};

#endif // LGSMAIN_H
