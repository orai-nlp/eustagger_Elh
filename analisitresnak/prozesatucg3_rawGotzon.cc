//////////////////////////////////////////////////////////////////
//
//  EUSTAGGER LITE
//
//  Copyright (C) 1996-2013  IXA Taldea
//                           EHU-UPV
//
//  This file is part of EUSTAGGER LITE.
//
//  EUSTAGGER LITE is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  EUSTAGGER LITE is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
//  Contact: Ixa Taldea (ixa@ehu.es)
//           649 Posta kutxa
//           20080 Donostia (Basque Country)
//
//////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "formatua.h"
#include "ald_orok.h"
#include "freeling.h"
#include "freeling/morfo/util.h"
#include <pcre++.h>

#include "cgmanager.h"
#include "euparole.h"

using namespace std;
using namespace pcrepp;
using namespace freeling;


string getEnvVar(string const& key);
string kendu_cg3rako_etiketak(string info);
string PrintResults (list < sentence > &ls, int level);
string desHMM(int maila, string &desIrteera, string &oinIzen);


string prozesatuCG3Raw(int maila, string &oinIzen) {
    string tmpVar = getEnvVar("IXA_PREFIX");
    string tmpName;
    string gramatika;
    string desSarrera = oinIzen + PHAT;
    string desIrteera = oinIzen ;
    string results = "";


    cout <<  "KAKAKAKA" <<endl;

    if (maila != 0) {
        if (tmpVar.length()>0) {
            tmpVar += "/var/eustagger_lite/mg/";
            gramatika = tmpVar + "gramatika4.4.1.cg3.dat";
        }
        else {
            cerr << "prozesatuCG3Raw => ERRORE LARRIA: 'IXA_PREFIX' ingurune aldagaia ezin daiteke atzitu" << endl;
            exit(EXIT_FAILURE);
        }
        desIrteera += ".irteera";
        // LEM formatua bada, .irteera ipini hemen eta bukaeran bihurketa egin
        cgManager cgApp;
        int sekzioak = 11;
        if (maila == 1) sekzioak = 3;
        else if (maila == 2) sekzioak = 4;
        else if (maila == 3) sekzioak = 5;

        if (cgApp.initGrammar(gramatika,sekzioak,'@',0) == 0) {// gramatika, zenbat sekzio, @ aurrizkia eta trace=0
            cerr << "Error in initGrammar: "<<gramatika << endl;
            return "";
        }
        if (cgApp.initIO(desSarrera,desIrteera) == 0) {
            cerr << "Error in initIO " << desSarrera << " or " << desIrteera<< endl;
            return "";
        }
        cgApp.applyGrammar();// aplikatu sekzio eta mapaketa guztiak
        cgApp.clean();
        unlink(desSarrera.c_str());
        results = results + desHMM(maila,desIrteera,oinIzen);
        unlink(desIrteera.c_str());
    }
    else {
        results = results + desHMM(maila,desSarrera,oinIzen);
        unlink(desSarrera.c_str());
    }
    return results;
}

string desHMM(int maila, string &desIrteera,string &oinIzen) {
    euParole parolera(maila);
    list<sentence> ls;
    sentence lws;
    ifstream in(desIrteera.c_str());
    string sars;
    string tmpVar = getEnvVar("IXA_PREFIX");
    string tmpName;
    string results = "";
    if (tmpVar.length()>0) {
        tmpVar += "/var/eustagger_lite/mg/";
        tmpName = tmpVar + "eustaggerhmmf.dat";
    }
    else {
        cerr << "prozesatuCG3Raw => ERRORE LARRIA: 'IXA_PREFIX' ingurune aldagaia ezin daiteke atzitu" << endl;
        exit(EXIT_FAILURE);
    }
    wstring tagDat = util::string2wstring(tmpName);
    hmm_tagger taggerEu(tagDat, false, FORCE_TAGGER,1);
    tmpName = tmpVar + "probabilitateak.dat";
    wstring probDat = util::string2wstring(tmpName);
    probabilities probs(probDat,0.001);
    stringstream irt;
    if (!in) {
        exit(EXIT_FAILURE);
    }
    getline(in,sars);
    while (sars.length()>0) {

      cout <<  sars <<endl;
      
        string formL;
        wstring form,lemma,tag,ambclass;
        Pcre firstL("\\\"<\\$?(.[^>]*)>\\\"");
        if (firstL.search(sars)) {
            string formTmp = firstL.get_match(0);
            form = util::string2wstring(formTmp);
        }
        word w(form);
        sars.clear();
        getline(in,sars);
        while (sars.length()>0 && sars[0] != '"') {
            parolera.getLemmaTag(sars,lemma,tag);
            if (lemma.length()>0) {
                analysis an(lemma,tag);
                w.add_analysis(an);
                if (ambclass.length()) ambclass += L"-";
                ambclass += tag;
            }
            else if (tag.length()>0) {
                lemma = form;
                analysis an(lemma,tag);
                w.add_analysis(an);
                if (ambclass.length()) ambclass += L"-";
                    ambclass += tag;
                }
                sars.clear();
                getline(in,sars);
        }
        //set prob
        wstring fakeform = ambclass + L"&-&";
        fakeform = fakeform + form;
        w.set_form(fakeform);
        probs.annotate_word(w);
        w.set_form(form);

        lws.push_back(w);
        if (form == L"." || form == L"!" || form == L"?") {
            ls.push_back(lws);
            lws.clear();
            taggerEu.analyze(ls);
            results = results + PrintResults(ls,maila);
            ls.clear();
        }
        if (sars[0] != '"') {
            sars.clear();
            getline(in,sars);
        }
    }

    if (lws.size() > 0) {
        ls.push_back(lws);
        lws.clear();
    }
    if (ls.size() > 0) {
        taggerEu.analyze(ls);
        results = results + PrintResults(ls,maila);
        ls.clear();
    }
    return results;
}
string PrintResults (list < sentence > &ls, int level) {
    sentence::const_iterator w;
    list < sentence >::iterator si;
    wstring lemma;
    //    wstring tag;
    string results;
    results = "";
    for (si = ls.begin (); si != ls.end (); si++) {
        for (w = si->begin (); w != si->end (); w++) {
	  //	  wcout << w->get_form ();
	  if (level > 0) {
	    for (word::const_iterator an = w->selected_begin (); an != w->selected_end (); an++) {
	      //	      wcout << L" " << an->get_lemma () << L" " << an->get_tag () ;
	      lemma = an->get_lemma();
	      //	      tag = an->get_tag();
	      //	      results = results + "T[" + string(tag.begin(), tag.end()) + "] " + string(lemma.begin(), lemma.end()) + " \n";
	      results = results + string(lemma.begin(), lemma.end()) + " ";
	    }
	    //	    wcout << endl;
	  }
	  else {
	    for (word::const_iterator an = w->analysis_begin();an != w->analysis_end();an++) {
	      //	      wcout << L" " << an->get_lemma () << L" " << an->get_tag () ;
	      lemma = an->get_lemma();
	      //	      tag = an->get_tag();
	      //	      results = results + "T[" + string(tag.begin(), tag.end()) + "] " + string(lemma.begin(), lemma.end()) + " \n";
	      results = results + string(lemma.begin(), lemma.end()) + " ";
	    }
	    //	    wcout << endl;
            }
        }
        // sentence separator: blank line.
        //	wcout << endl;
    }
    //    cout << "\n";    
    //    wcout.flush();
    return results;
}
