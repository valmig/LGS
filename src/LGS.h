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
#include <n_polynom.h>

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

enum field_type{RATIONAL, MODQ, DOUBLE, COMPLEX, INTEGER, GAUSSNUMBER, RFUNCTION};

template <class T>
std::string ToString(const val::matrix<T>& A);

template <class T>
void SetLGS(std::string &s,val::matrix<T> &A);

int has_variable(const std::string &s);

void lgsmain(std::string& s,int &numberfield,int p);

void simplex(std::string& s);

void evaluation(std::string &s,std::string expression ,int numberfield, int p);

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


template <class T>
int sum_matrix(const val::matrix<T>& A, const val::matrix<T> &B, val::matrix<T> &result)
{
    if (A.numberofrows() != B.numberofrows() || A.numberofcolumns() != B.numberofcolumns()) return 0;
    result = A + B;
    return 1;
}

template <class T>
int prod_matrix(const val::matrix<T> &A, const val::matrix<T> &B, val::matrix<T> &result)
{
    if (A.numberofcolumns() != B.numberofrows()) return 0;
    result = A * B;
    return 1;
}

template <class T>
val::matrix<T> power_matrix(const val::matrix<T> &a, int n)
{
    val::matrix<T> result, y;

    result = val::matrix<T>(a.numberofrows());
    result.make_identity();

    if (a.iszero()) return  a;
    if (n==0) return result;
    //if (z>0) y=a;
    //else y=1.0/a;
    y = a;

    while (n!=0) {
       if (n%2!=0) {
           result *= y;
       }
       y *= y;
       n = n/2;
    }
    return result;
}

template <class T>
int evaluate_expression(const val::n_polynom<T> &p, const val::Glist<val::matrix<T>> &A, std::string &s)
{
    int n = val::Min(A.length(),p.getdim()), k = 0;
    val::matrix<T> result, h, h1;

    /*
    for (const auto &a : A) {
        std::cout << std::endl << ToString(a);
    }
    */

    for (const auto & m : p) {
        for (int i = 0; i < n; ++i) {
            if (m.actualterm()[i] != 0) {
                if (m.actualterm()[i] == 1) h1 = A[i];
                else if (A[i].numberofrows() != A[i].numberofcolumns()) {
                    s += "\nFehler Potenz: Matrix ist nicht quadratisch!";
                    return 0;
                }
                else h1 = power_matrix(A[i], m.actualterm()[i]);
            }
            if (i != 0) {
                if (h.numberofcolumns() != h1.numberofrows()) {
                     s += "\nFehler Multiplikation: Dimension der Matrizen nicht zulässig!";
                    return 0;
                }
                else h *= h1;
            }
            else h = h1;
        }
        h *= m.actualcoef();
        if (k != 0) {
            if (result.numberofrows() != h.numberofrows() || result.numberofcolumns() != h.numberofcolumns()) {
                s += "\nFehler Addition: Dimension der Matrizen ist nicht zulässig!";
                return 0;
            }
            else result += h;
        }
        else result = h;
        ++k;
    }
    s += "\n" + ToString(result);
    return 1;
}



#endif // LGSNEW_H_INCLUDED
