/*
 * fen.cpp
 *
 *  Created on: Apr 6, 2015
 *      Author: alex
 */
#include "fen.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

/*Fen::Fen() {
	Fen::fen[0] = "rnbqkbnr";
	Fen::fen[1] = "pppppppp";
	Fen::fen[2] = "asdf";
	fen[3] = "8";
	fen[4] = "8";
	fen[5] = "8";
	fen[6] = "8";
	fen[7] = "PPPPPPPP";
	fen[8] = "RNBQKBNR";
}
Fen::Fen(std::vector<string> fenstrings) {
	for(int i = 0; i < 8; i++) {
		cout << fenstrings[i] << endl;
		Fen::fen[i] = fenstrings[i];
		cout << Fen::fen.size() << endl;
	}
}*/
Fen::Fen() {
    fenstring = {"rnbqkbnr", "pppppppp", "8", "8", "8", "8", "PPPPPPPP", "RNBQKBNR", "w", "KQkq", "-", "1", "1"};
}
Fen::Fen(std::vector<string> fenstrings) {
    //fenstring = {"", "", "", "", "", "", "", ""};
    for (int i = 0; i < 8; i++) {
        makeFen(fenstrings[i]);
        fenstring.push_back(makeFen(fenstrings[i]));
    }

    if(fenstrings.size() < 13) {
        cout << "Fenstring-Error! (Fenstring must be a vector of strings)" << endl;
    } else {
        for(int i = 8; i < 13; i++) {
            fenstring.push_back(fenstrings[i]);
        }
    }
}

bool Fen::isNumber(char c) {
	int c_ascii = static_cast<int>(c);
	if(c_ascii > 48 && c_ascii < 58) return true;
	return false;
}

string Fen::getPos(int i) {
	string pos = "xxxxxxxx";
	int p = 0;
	for (int j = 0; j < fenstring[i].size(); j++) {
		if(isNumber(fenstring[i][j])) {
			for(int z = 0; z < static_cast<int>(fenstring[i][j]) - 48; z++) {
				pos[p + z] = '0';
			}
			p += static_cast<int>(fenstring[i][j]) - 48;
		} else {
			pos[p] = fenstring[i][j];
			p++;
		}
	}
	return pos;
}

string Fen::getRow(int i) {
	string row = "xxxxxxxx";
	int p = 0;
	for (int j = 0; j < fenstring[i].size(); j++) {
		if(isNumber(fenstring[i][j])) {
			for(int z = 0; z < static_cast<int>(fenstring[i][j]) - 48; z++) {
				row[p + z] = '0';
			}
			p += static_cast<int>(fenstring[i][j]) - 48;
		} else {
			row[p] = fenstring[i][j];
			p++;
		}
	}
	return row;
}

char Fen::getActiveColor() {
    return getFen(8)[0];
}

int Fen::getMoveNr() {
    return boost::lexical_cast<int>(getFen(12));
}

string Fen::makeFen (string row) {
	string fen;
    fen.clear();
	for(int i = 0; i < 8; i++) {
        /* row is already a fen string */
        if(row[i] >= '1' && row[i] <= '9') {
            fen = row;
            break;
        }
        if(row[i] == '-' || row[i] == '0') {
			int c = 0; // Leerfeld-Zähler
            while(i < 8 && (row[i] == '-' || row[i] == '0')) {
				i++; c++;
			}
			i--; // Zähler wird sonst durch for-Schleife ein zweites Mal erhöht
			fen += static_cast<char>(c + 48);
		} else {
			fen += row[i];
		}
	}
	return fen;
}
std::vector<int> Fen::searchChessman(char man) {
	vector<int> pos{ -1, -1 };
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			string row = getRow(i);
			if(row[j] == man) {
				pos[0] = j; pos[1] = i; // Koordinaten der gefundenen Schachfigur
				return pos;
			}
		}
	}
	return pos;
}
char Fen::getChessman(int x, int y) {
	string pos = getRow(y);
	return pos[x];
}

void Fen::setChessman(char man, int x, int y) {
	string row = getRow(y);
	row[x] = man;
	//cout << row << endl;
	changeFen(y, row);
}

void Fen::changeFen(int i, string row) {
	fenstring[i] = makeFen(row);
}

string Fen::getFen(int i) {
	return fenstring[i];
}

void Fen::print() {
	for (int i = 0; i < 8; i++) {
		cout << fenstring[i] << endl;
	}
	/*for (int i = 0; i < 8; i++) {
		cout << fen[i] << endl;
	}*/
}

/*Fen::Fen(vector<string> fenstrings) {
	for(int i = 0; i < 8; i++) {
		fen[i] = fenstrings[i];
	}
}*/
/*Fen::Fen(std::vector<string> fenstrings) {
	vector<string> fen(8);
	for(int i = 0; i < 8; i++) {
		fen[i] = fenstrings[i];
	}
	fen[1] = "pppppppp";
	fen[2] = "asdf";
	fen[3] = "8";
	fen[4] = "8";
	fen[5] = "8";
	fen[6] = "8";
	fen[7] = "PPPPPPPP";
	fen[8] = "RNBQKBNR";
}*/
