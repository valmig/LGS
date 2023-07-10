/***************************************************************
 * Name:      LGSApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Miguel Valbuena ()
 * Created:   2015-12-17
 * Copyright: Miguel Valbuena ()
 * License:
 **************************************************************/

#include "LGSApp.h"

//(*AppHeaders
#include "LGSMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(LGSApp)

bool LGSApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	LGSFrame* Frame = new LGSFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
