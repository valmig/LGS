#ifndef LGSNEW_H_INCLUDED
#define LGSNEW_H_INCLUDED

#include <wx/frame.h>
#include <val_basics.h>
#include <modq.h>
#include <rational.h>
#include <complex.h>
#include <matrix.h>
#include <val_utils.h>
#include <numbers.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <string>
#include <pol.h>
#include <val_filesys.h>

extern std::string filesep,settingsfile,settingsdir,valdir,iconpath, alticonpath;
//extern wxTextCtrl *MyOutput;
//extern wxButton *MyButton;
extern wxFrame* MyFrame;

class MyThreadEvent;
wxDECLARE_EVENT(MY_EVENT, MyThreadEvent);

class MyThreadEvent: public wxThreadEvent
{
public:
	MyThreadEvent(wxEventType commandType, int id = 0)
        		:  wxThreadEvent(commandType, id) { }

	// You *must* copy here the data to be transported
	MyThreadEvent(const MyThreadEvent& event)
        :  wxThreadEvent(event), message(event.message) {} //{ this->SetMessage(event.GetMessage()); }

	// Required for sending with wxPostEvent()
	wxEvent* Clone() const { return new MyThreadEvent(*this); }

	std::string GetMessage() const { return message; }
	void SetMessage(const std::string &s) { message = s; }

private:
	std::string message;
};

enum {IdMessage};


/*
template <class T>
std::string numbertostring(const T&);


template <class T>
T stringtonumber(const std::string& s);
*/

template <class T>
std::string ToString(const val::matrix<T>& A);

template <class T>
void SetLGS(std::string &s,val::matrix<T> &A);

int has_variable(const std::string &s);

void lgsmain(std::string& s,int &numberfield,int p);

void simplex(std::string& s);


template <class T>
std::string ToString(const val::matrix<T>& A)
{
    int i,j,k,maxlength=0,*leer=NULL,l,m=A.numberofrows(),n=A.numberofcolumns();
    std:: string s="",help;

    if (n) leer = new int[n];

    for (j=0;j<n;j++) {
        maxlength=0;
        for (i=0;i<m;i++) {
            help = val::ToString(A(i,j));
            //help = numbertostring(A(i,j));
            l=2*int(help.length());
            if (l && help[0]=='-') l--;
            if (l>maxlength) maxlength=l;
        }
        leer[j]=maxlength;
    }


    for (i=0;i<m;i++) {
        for (j=0;j<n;j++) {
            help = val::ToString(A(i,j));
            //help = numbertostring(A(i,j));
            l=2*int(help.length());
            if (l && help[0]=='-') l--;
            for (k=0;k<4+leer[j]-l;k++) s+=" ";
            s+=help;
        }
        s+='\n';
    }

    if (leer!=NULL) delete[] leer;
    return s;
}



/*
template <class T>//,typename std::enable_if_t<std::is_arithmetic<T>::value || val::is_nongeneric_ring<T>::value,int> = 0>
std::string PolToString(const val::pol<T> &f)
{
    int i;
    val::polIterator<T> it;
    std::string s="";
    T one=val::unity_element<T>(),minusone=-one,zero=val::zero_element<T>();

    for (i=0,it=f;it;it++,++i) {
        if (i!=0 && it.actualcoef()>zero) s+='+';
        if (it.actualcoef()==minusone) {
            s+='-';
            if (it.actualdegree()==0) s+='1';
        }
        else if (it.actualcoef()!=one) s+=val::ToString(it.actualcoef());
        if (it.actualdegree()) {
            if (it.actualcoef()!=one && it.actualcoef()!=minusone) s+='*';
            s+='x';
            if (it.actualdegree()!=1) s+="^" + val::ToString(it.actualdegree());
        }
    }
    return s;
}
*/



#endif // LGSNEW_H_INCLUDED
