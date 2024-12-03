#include "LGS.h"
#include <sstream>
#include <pol_arithmetic.h>
#include <LA.h>
#include <valfunction.h>
#include <val_utils.h>

#ifdef _WIN32
std::string filesep="\\",filedir=val::CurrentHomeDir(),settingsfile=val::CurrentHomeDir()+"\\AppData\\Local\\MVPrograms\\LGS\\settings.txt",
            settingsdir = val::CurrentHomeDir()+"\\AppData\\Local\\MVPrograms\\LGS",valdir = val::CurrentHomeDir()+"\\AppData\\Local\\MVPrograms",
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

std::string ansmatrix = "";

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


val::Glist<std::string> getmatstrings(const std::string &s)
{
    val::Glist<std::string> values;
    int i = 0 ,n = s.length();
    if (n==0) return values;
    std::string sm="";

    while (i < n  && s[i] == '\n') ++i;

    while (i < n) {
        if (i > 0 && s[i] == '\n' && s[i-1] == '\n') {
            values.push_back(std::move(sm));
            while (i < n && s[i] == '\n') ++i;
        }
        else {
            sm += s[i];
            ++i;
        }
    }

    if (sm != "") values.push_back(std::move(sm));

    return values;
}



int has_variable(const std::string &s)
{
    int n = s.length();

    for (int i = 0; i < n; ++i) {
        if (s[i] == 'x') return 1;
    }

    return 0;
}

int substitutepar(std::string &s)
{
    int j = 1, n, i, found =0;
    std::string rw, rs;
    val::Glist<char> VarList;


    rw = "#";

    if (val::replace<char>(s, "ans", rw)) found = 1;

    n = s.length();
    for (i = 0; i < n; ++i) {
        if ((s[i] >= 65 && s[i] <= 90) || (s[i] >= 97 && s[i] <= 122 && s[i] != 'i' && s[i] != 'e')) {
            if (!val::isinContainer(s[i], VarList)) VarList.sinsert(s[i]);
        }
    }

    for (const auto &v : VarList) {
        rw = "A" + val::ToString(j);
        rs = v;
        ++j;
        val::replace(s, rs, rw);
        n = s.length();
    }

    //std::cout << "\n s = " << s;
    val::replace(s, std::string("A"), std::string("x"));

    i = VarList.length() + 1;
    rw = "#";
    rs = "x" + val::ToString(i);
    val::replace<char>(s, rw, rs);

    // std::cout << "\n s = " << s;
    return found;
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



void lgsmain(std::string& s,int &numberfield,int p)
{
    val::modq::q=p;
    int n,r,i,j;

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


void simplex(std::string &s)
{
    using namespace val;
    matrix<rational> A = val::FromString<matrix<rational>>(s);
    int m = A.numberofrows(), n = A.numberofcolumns(), i ,j, pi = 0, pj = 0, pfound = 0, nel = 0;
    rational zero, min, inf(1,0), r;

    MyThreadEvent event(MY_EVENT,IdMessage);

    event.SetMessage("");

    s = "Simplex-Algorithm:\n";

    if (n < m+1) {
        s += "\n Wrong dimensions!\n";
        event.SetMessage(s);
        MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
        return;
    }

    --n;

    for (i = 0; i < m; ++i) {
        if (A(i,n) < zero) {
            s += "\n Matrix not valid!\n";
            event.SetMessage(s);
            MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
            return;
        }
    }


    do {
        pfound = 0;
        min = zero;
        // find min element < 0 in last row
        for (j = 0; j < n; ++j) {
            if (A(m-1,j) < zero) {
                pfound = 1;
                if (A(m-1,j) < min) {
                    pj = j;
                    min = A(m-1,j);
                }
            }
        }
        if (!pfound) break;
        min = inf;
        // find pivot in pj column:
        pfound = 0;
        for (i = 0; i < m-1; ++i) {
            if (A(i,pj) <= zero) continue;
            if ( (r = A(i,n)/A(i,pj)) < min) {
                min = r;
                pi = i;
                pfound = 1;
            }
        }
        if (!pfound) break;
        // divide pi-th row by A(pi,pj);
        r = A(pi,pj);
        for (j = 0; j <= n; ++j) A(pi,j) /= r;
        // Elimination:
        for (i = 0; i < m; ++i) {
            if (i == pi) continue;
            r = A(i,pj);
            for (j = 0; j <= n; ++j) {
                A(i,j) -= r*A(pi,j);
            }
        }
        ++nel;
        s += "\n Pivot: " + ToString(pi+1) + " , " + ToString(pj+1);
        s += "\n A after " + ToString(nel) + ". elimination:\n" + ToString(A) +"\n";
    }
    while (pfound);

    event.SetMessage(s);
    MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
}


void evaluation(std::string &s, std::string expression, int numberfield, int p)
{
    MyThreadEvent event(MY_EVENT,IdMessage);
    val::Glist<std::string> values = getmatstrings(s);
    std::string oexpr = expression;

    /*
    if (expression.find("ans") != std::string::npos) {
        val::replace<char>(expression, "ans", "t");
        val::valfunction f(expression);
        int n = f.numberofvariables();
        if (f.isconst()) n = 0;
        n = val::Max(n,values.length());
        std::string x_s = "x" + val::ToString(n+1);
        val::replace<char>(expression, "t", x_s);
        values.push_back(ansmatrix);
    }
    */
    if (substitutepar(expression)) values.push_back(ansmatrix);

    val::valfunction f(expression, 0);



    /*
    s = "Expression: " + f.getinfixnotation();
    s += "\nAnzahl Matrizen: " + val::ToString(values.length()) + "\n";
    for (const auto & v: values) {
        s += v + "\n";
    }
    */

    switch (numberfield) {
        case RATIONAL: case INTEGER: case GAUSSNUMBER:
        {
            val::Glist<val::matrix<val::gauss_number>> A;
            val::n_polynom<val::gauss_number> P = f.getn_polynom<val::gauss_number>();

            for (const auto &v : values) A.push_back(val::FromString<val::matrix<val::gauss_number>>(v));
            s = "Result of expression: " + oexpr + "\n";
            evaluate_expression(P, A, s);
        } break;
        case DOUBLE: case COMPLEX:
        {
            val::Glist<val::matrix<val::complex>> A;
            val::n_polynom<val::complex> P = f.getn_polynom<val::complex>();
            for (const auto &v : values) A.push_back(val::FromString<val::matrix<val::complex>>(v));
            s = "Result of expression: " + oexpr + "\n";
            evaluate_expression(P, A, s);
        } break;
        case MODQ:
        {
            val::modq::q = p;
            val::Glist<val::matrix<val::modq>> A;
            val::n_polynom<val::modq> P = f.getn_polynom<val::modq>();

            for (const auto &v : values) A.push_back(val::FromString<val::matrix<val::modq>>(v));
            s = "Result of expression: " + oexpr + "\n";
            evaluate_expression(P, A, s);
        } break;
        /*
        case RFUNCTION:
        {
            val::Glist<val::matrix<val::rationalfunction>> A;
            val::n_polynom<val::rationalfunction> P = f.getn_polynom<val::rationalfunction>();
            for (const auto &v : values) A.push_back(val::FromString<val::matrix<val::rationalfunction>>(v));
            evaluate_expression(P, A, s);
        } break;
        */
        default: break;
    }

    event.SetMessage(s);
    MyFrame->GetEventHandler()->QueueEvent(event.Clone() );
}
