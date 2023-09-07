#include "LGS.h"
#include <sstream>
#include <pol_arithmetic.h>
#include <LA.h>
#include <valfunction.h>

#ifdef _WIN32
std::string filesep="\\",filedir=val::CurrentHomeDir(),settingsfile=val::CurrentHomeDir()+"\\AppData\\Local\\MVPrograms\\LGS\\settings.txt",
            settingsdir = val::CurrentHomeDir()+"\\AppData\\Local\\MVPrograms\\LGS",valdir = val::CurrentHomeDir()+"\\AppData\\Local\\LGS",
            iconpath="C:\\gccprogrammes\\LA\\LGS\\icon\\LGS.xpm",
            alticonpath = val::GetExeDir() + "\\LGS.xpm";
#endif // _WIN32
#ifdef __LINUX__
std::string filesep="/", valdir = val::CurrentHomeDir() + "/.config/MVPrograms", settingsdir = valdir + "/LGS",
            settingsfile = settingsdir + "/settings.conf",
            iconpath = val::CurrentHomeDir() + "/.local/share/icons/LGS/LGS.xpm",
            alticonpath = "";
#endif // __LINUX__
#ifdef __APPLE__
std::string filesep="/", valdir = val::CurrentHomeDir() + "/Library/Application Support",
            settingsdir = valdir + "/LGS", settingsfile = settingsdir + "/settings.conf",
            iconpath = val::CurrentHomeDir() + "/.local/share/icons/LGS/LGS.xpm",
            alticonpath = val::GetExeDir() + "/../Resources/LGS.xpm";
#endif // __APPLE__



//wxTextCtrl *MyOutput=NULL;
//wxButton *MyButton=NULL;
wxFrame* MyFrame = nullptr;



namespace val
{
    std::istream& operator >>(std::istream& is,valfunction &f)
    {
        std::string s;
        is >> s;
        f = valfunction(s);
        return is;
    }

    std::istream& operator >>(std::istream &is, rationalfunction &f)
    {
        valfunction F;
        is >> F;
        f = F.getrationalfunction();
        return is;
    }

    std::ostream& operator <<(std::ostream &os, const rationalfunction &value)
    {
        std::string s;

        if (value.nominator().degree() <= 0 && value.denominator().degree() == 0) {
            s = ToString(value.nominator().LC()/value.denominator().LC());
            os << s;
            return os;
        }

        rational cont, c1;
        pol<integer> nom, denom;

        primitivpart(value.nominator(),nom,cont);
        primitivpart(value.denominator(),denom,c1);

        cont /= c1;
        if (denom.LC().signum() == -1) {
            nom *= integer(-1);
            denom *= integer(-1);
        }
        if (nom.LC().signum() == -1) {
            nom *= integer(-1);
            cont.changesign();
        }

        if (cont.signum() == -1 || cont.denominator() != integer(1)) {
            if (cont == rational(-1)) s += "-";
            else {
                s += "(" + ToString(cont) + ")*";
            }
        }
        else if (cont != rational(1)) s += ToString(cont);
        if (nom.length()>1) {
            s += "(" + val::PolToString(nom) + ")";
        }
        else s += val::PolToString(nom);
        if (denom.degree()>0) s += "/";
        if (denom.length()>1) {
            s += "(" + val::PolToString(denom) + ")";
        }

        os << s;
        return os;
    }
} // end namespace val


int has_variable(const std::string &s)
{
    int n = s.length();

    for (int i = 0; i < n; ++i) {
        if (s[i] == 'x') return 1;
    }

    return 0;
}


void makeprimitiv(val::matrix<val::rational> &A)
{
    int n = A.numberofcolumns(), m = A.numberofrows(), i ,j;
    if (m < 2) return;
    val::integer ggt, kgv;
    val::rational r;

    for (i = 1; i < m; ++i) {
        ggt = A(i,0).nominator(); kgv = A(i,0).denominator();
        for (j = 1; j < n; ++j) {
            ggt = val::gcd(ggt,A(i,j).nominator());
            kgv = val::lcm(kgv,A(i,j).denominator());
        }
        r = val::rational(kgv,ggt);
        for (j = 0; j < n; ++j) A(i,j) *= r;
    }
}


void makeprimitiv(val::matrix<val::gauss_number> &A)
{
    int n = A.numberofcolumns(), m = A.numberofrows(), i ,j;
    if (m < 2) return;
    val::integer ggt, kgv, one(1);
    val::gauss_number r;

    for (i = 1; i < m; ++i) {
        ggt = A(i,0).real().nominator(); kgv = A(i,0).real().denominator();
        ggt = val::gcd(ggt,A(i,0).imaginary().nominator()); kgv = val::lcm(kgv,A(i,0).imaginary().denominator());
        for (j = 1; j < n; ++j) {
            ggt = val::gcd(ggt,A(i,j).real().nominator());
            ggt = val::gcd(ggt,A(i,j).imaginary().nominator());
            kgv = val::lcm(kgv,A(i,j).real().denominator());
            kgv = val::lcm(kgv,A(i,j).imaginary().denominator());
        }
        r = val::rational(kgv,ggt);
        for (j = 0; j < n; ++j) A(i,j) *= r;
    }
}




/*
std::string rationalfunctionTostring(const val::rationalfunction &value)
{
    using namespace val;
    std::string s;

    if (value.nominator().degree() <= 0 && value.denominator().degree() == 0) {
        return ToString(value.nominator().LC()/value.denominator().LC());
    }

    rational cont, c1;
    pol<integer> nom, denom;

    primitivpart(value.nominator(),nom,cont);
    primitivpart(value.denominator(),denom,c1);

    cont /= c1;
    if (denom.LC().signum() == -1) {
        nom *= integer(-1);
        denom *= integer(-1);
    }
    if (nom.LC().signum() == -1) {
        nom *= integer(-1);
        cont.changesign();
    }

    if (cont.signum() == -1 || cont.denominator() != integer(1)) {
        if (cont == rational(-1)) s += "-";
        else {
            s += "(" + ToString(cont) + ")*";
        }
    }
    else if (cont != rational(1)) s += ToString(cont);
    if (nom.length()>1) {
        s += "(" + val::PolToString(nom) + ")";
    }
    else s += val::PolToString(nom);
    if (denom.degree()>0) s += "/";
    if (denom.length()>1) {
        s += "(" + val::PolToString(denom) + ")";
    }

    return s;
}



template <>
std::string numbertostring(const val::modq& wert)
{
    return val::ToString(int(wert));
}

template <>
std::string numbertostring(const val::rationalfunction& wert)
{
    return rationalfunctionTostring(wert);
}


template <class T>
std::string numbertostring(const T& wert)
{
    return val::ToString(wert);
}


template <>
val::integer stringtonumber(const std::string& s)
{
    return val::FromString<val::integer>(s);
}

template <>
val::rational stringtonumber(const std::string& s)
{
    return val::FromString<val::rational>(s);//val::string_to_rational(s);
}

template <>
double stringtonumber(const std::string& s)
{
    return double( val::FromString<val::rational>(s) ); //double(val::string_to_rational(s));
}

template<>
val::complex stringtonumber(const std::string& s)
{
    return val::FromString<val::complex>(s);
}

template<>
val::modq stringtonumber(const std::string& s)
{
    std::stringstream ss;
    val::modq x;

    ss.str(s);
    ss>>x;
    return x;
}

template<>
int stringtonumber(const std::string& s)
{
    std::stringstream ss;
    int x;

    ss.str(s);
    ss>>x;
    return x;
}

template<>
val::gauss_number stringtonumber(const std::string& s)
{
    return val::FromString<val::gauss_number>(s);
}

template<>
val::rationalfunction stringtonumber(const std::string& s)
{
    val::valfunction f(s);
    return f.getrationalfunction();
}
*/




/*
template <class T>
void SetLGS(std::string &s,val::matrix<T> &A)
{
    int l=s.length(),first=0,i,j,k,nzeichen=0,m,n;
    std::string zeichen="";

    for (i=0;i<l;i++)
        if (s[i]!='\n' && s[i]!=' ' ) break;
    first=i;
    m=n=0;
    //Bestimme Spaltenzahl n
    for (;i<l;) {
        if (i<l && s[i]=='\n') {break;}
        while (i<l && (s[i]!=' ' && s[i] != '\n' )) i++;
        n++;
        while (i<l && s[i]==' ') i++;
    }
    //Bestimme Spaltenzahl m
    if (n<=1) {m=0;return;}
    for (i=first;i<l;) {
        if (s[i]!=' ' && s[i]!='\n') {
            while (i<l && s[i]!=' ' && s[i]!='\n') i++;
            nzeichen++;
        }
        while (i<l && (s[i]==' ' || s[i] =='\n')) i++;
    }
    m=nzeichen/n;
    if (nzeichen%n!=0) m++;

    A=val::matrix<T>(m,n);
    A.make_zero();

    i=j=0;
    for (k=first;k<l;) {
        if (s[k]!=' ' && s[k]!='\n') {
            while (k<l && s[k]!=' ' && s[k]!='\n') {
                zeichen+=s[k];
                k++;
            }
            //A(i,j)=val::string_to_rational(zeichen);
            A(i,j)=stringtonumber<T>(zeichen);
            zeichen="";
            j++;
            if (j>=n) {j=0;i++;}
        }
        while (k<l && (s[k]==' ' || s[k] =='\n')) {
               k++;
        }
    }
}

*/


void lgsmain(std::string& s,int &numberfield,int p)
{
    val::modq::q=p;
    int n,r,i,j;
    //val::rational detrat;
    //val::modq detmodq;
    //val::complex detcom;
    //val::gauss_number detgauss;
    //val::rationalfunction detratfunc;
    //double detdoub;
    //val::rational **a=NULL, **X=NULL,det;
    //val::matrix<val::modq> Amodq,Xmodq;
    //val::matrix<val::integer> Aint,Xint;
    //val::matrix<val::rational> Arat,Xrat;
    //val::matrix<val::complex> Acom,Xcom;
    //val::matrix<double> Adoub,Xdoub;
    //val::matrix<val::gauss_number> Agauss,Xgauss;
    //val::matrix<val::rationalfunction> Aratfunc, Xratfunc;

    MyThreadEvent event(MY_EVENT,IdMessage);

    event.SetMessage("");

    if (has_variable(s)) numberfield = 6;  // rational function.

    //SetLGS(s,m,n,a);
    if (numberfield==0) {
        val::matrix<val::rational> A,X;
        val::rational det;
        //SetLGS(s,Arat);
        A = val::FromString<val::matrix<val::rational> >(s);
        //val::pol<val::rational> f = val::char_pol(Arat);
        val::pol<val::rational> f = val::char_pol(A);
        s="";
        //if (Arat.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les(Arat,Xrat,detrat);
        //s=ToString(Arat);
        r=val::les(A,X,det);
        s=ToString(A);
        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xrat.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xrat(0,j)) + "  ";
            for (j=0;j<n;j++) s+=ToString(X(0,j)) + "  ";
            if (r>1) {
                makeprimitiv(X);
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xrat(i,j));
                    for (j=0;j<n;j++) s+="  " + ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + numbertostring(detrat);
        s+="\n det = " + val::ToString(det);
        s+="\nCharakteristisches Polynom:\n" + val::PolToString(f);
    }
    else if (numberfield==1)
    {
        val::matrix<val::modq> A,X;
        val::modq det;
        //SetLGS(s,Amodq);
        A = val::FromString<val::matrix<val::modq>>(s);
        //val::pol<val::modq> f = val::char_pol(Amodq);
        val::pol<val::modq> f = val::char_pol(A);
        s="";
        //if (Amodq.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les(Amodq,Xmodq,detmodq);
        //s=ToString(Amodq);
        r=val::les(A,X,det);
        s=ToString(A);
        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xmodq.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xmodq(0,j)) + "  ";
            for (j=0;j<n;j++) s += val::ToString(X(0,j)) + "  ";
            if (r>1) {
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xmodq(i,j));
                    for (j=0;j<n;j++) s+="  " + val::ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + numbertostring(detmodq);
        s+="\n det = " + val::ToString(det);
        s+="\nCharakteristisches Polynom:\n" + val::PolToString(f);
    }
    else if (numberfield==2)
    {
        val::matrix<double> A,X;
        double det;
        //SetLGS(s,Adoub);
        A = val::FromString<val::matrix<double>>(s);
        //val::pol<double> f = val::char_pol(Adoub);
        s="";
        //if (Adoub.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les_double(Adoub,Xdoub,detdoub);
        //s=ToString(Adoub);
        r=val::les_double(A,X,det);
        s=ToString(A);
        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xdoub.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xdoub(0,j)) + "  ";
            for (j=0;j<n;j++) s += val::ToString(X(0,j)) + "  ";
            if (r>1) {
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xdoub(i,j));
                    for (j=0;j<n;j++) s+="  " + val::ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + numbertostring(detdoub);
        s+="\n det = " + val::ToString(det);
        //s+="\nCharakteristisches Polynom:\n" + val::PolToString(f);
        //s+="\n" + val::ToString(f);
    }

    else if (numberfield==3) {
        val::matrix<val::complex> A,X;
        val::complex det;
        //SetLGS(s,Acom);
        A = val::FromString<val::matrix<val::complex>>(s);
        //val::pol<val::complex> f = val::char_pol(Acom);
        s="";
        //if (Acom.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les_double(Acom,Xcom,detcom);
        //s=ToString(Acom);
        r=val::les_double(A,X,det);
        s=ToString(A);
        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xcom.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xcom(0,j)) + "  ";
            for (j=0;j<n;j++) s += val::ToString(X(0,j)) + "  ";
            if (r>1) {
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xcom(i,j));
                    for (j=0;j<n;j++) s+="  " + val::ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + numbertostring(detcom);
        s+="\n det = " + val::ToString(det);
        //s+="\nCharakteristisches Polynom:\n" + val::ToString(f);
    }
    else if (numberfield==4) {
        val::matrix<val::integer> A,X;

        //SetLGS(s,Aint);
        A = val::FromString<val::matrix<val::integer>>(s);
        s="";
        //if (Aint.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les_integer(Aint,Xint);
        r=val::les_integer(A,X);
        s="Matrix nach Elementarteiler-Alg:\n";
        //s+=ToString(Aint);
        s+=ToString(A);
        //s+= "\n" + val::ToString(r) + "  " + val::ToString(Xint.numberofrows());

        if (r) {
            s+="\nSpezielle Loesung:\n";
            //n=Xint.numberofcolumns();
            n=X.numberofcolumns();
            //for (i=0;i<n;i++) s+= "  " + val::ToString(Xint(0,i));
            for (i=0;i<n;i++) s+= "  " + val::ToString(X(0,i));
            //r=Xint.numberofrows();
            r=X.numberofrows();
            if (r>1) {
                s+="\nLoesungsraum des homogenen LGS:\n";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++)  s+= "  " + val::ToString(Xint(i,j));
                    for (j=0;j<n;j++)  s+= "  " + val::ToString(X(i,j));
                    s+="\n";
                }
            }
        }
        else {
            s+="\nLGS nicht loesbar!";
        }
    }
    else if (numberfield == 5) {
        val::matrix<val::gauss_number> A,X;
        val::gauss_number det;

        //SetLGS(s,Agauss);
        A = val::FromString<val::matrix<val::gauss_number>>(s);
        //val::pol<val::gauss_number> f = val::char_pol(Agauss);
        val::pol<val::gauss_number> f = val::char_pol(A);
        s="";
        //if (Agauss.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les(Agauss,Xgauss,detgauss);
        //s=ToString(Agauss);
        r=val::les(A,X,det);
        s=::ToString(A);
        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xgauss.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xgauss(0,j)) + "  ";
            for (j=0;j<n;j++) s+=val::ToString(X(0,j)) + "  ";
            if (r>1) {
                makeprimitiv(X);
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xgauss(i,j));
                    for (j=0;j<n;j++) s+="  " + val::ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + numbertostring(detgauss);
        s+="\n det = " + val::ToString(det);
        s+="\nCharakteristisches Polynom:\n" + val::ToString(f);
    }
    else {
        val::matrix<val::rationalfunction> A,X;
        val::rationalfunction det;

        //SetLGS(s,Aratfunc);
        A = val::FromString<val::matrix<val::rationalfunction>>(s);
        s="";
        //if (Aratfunc.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        if (A.iszero()) {MyFrame->GetEventHandler()->QueueEvent(event.Clone() );return;}
        //r=val::les(Aratfunc,Xratfunc,detratfunc);
        //s=ToString(Aratfunc);
        r=val::les(A,X,det);
        s=ToString(A);

        if (r==0) s+="\n\nLgs ist unloesbar!";
        else {
            s+="\n r = "+ val::ToString(r);
            s+="\nLoesung lautet:\n";
            //n=Xratfunc.numberofcolumns();
            n=X.numberofcolumns();
            //for (j=0;j<n;j++) s+=numbertostring(Xratfunc(0,j)) + "  ";
            for (j=0;j<n;j++) s += val::ToString(X(0,j)) + "  ";
            if (r>1) {
                s+=" + <";
                for (i=1;i<r;i++) {
                    //for (j=0;j<n;j++) s+="  " + numbertostring(Xratfunc(i,j));
                    for (j=0;j<n;j++) s+="  " + val::ToString(X(i,j));
                    if (i!=r-1) s+="\n   ";
                }
                s+=" >";
            }
        }
        //s+="\n det = " + val::ToString(detratfunc);
        s+="\n det = " + val::ToString(det);
    }


    event.SetMessage(s);
    MyFrame->GetEventHandler()->QueueEvent(event.Clone() );

    //MyOutput->WriteText(s);

    //MyButton->Enable();

}


