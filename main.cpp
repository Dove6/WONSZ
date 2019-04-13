#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <fstream>
#include <windows.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <random>
#include <algorithm>

using namespace std;

/* refaktoryzacja */
struct {
    bool dev,
         pauza,
         koniec;
} flagi = {false, false, false};

struct {
    bool dzwiek,
         oddzielne_zycia;
    int szybkosc;
} opcje = {true, true, 15};

sf::Keyboard::Key sterowanie[6][4] = {
    {sf::Keyboard::Up,      sf::Keyboard::Left,    sf::Keyboard::Down,    sf::Keyboard::Right},
    {sf::Keyboard::W,       sf::Keyboard::A,       sf::Keyboard::S,       sf::Keyboard::D},
    {sf::Keyboard::T,       sf::Keyboard::F,       sf::Keyboard::G,       sf::Keyboard::H},
    {sf::Keyboard::I,       sf::Keyboard::J,       sf::Keyboard::K,       sf::Keyboard::L},
    {sf::Keyboard::Home,    sf::Keyboard::Delete,  sf::Keyboard::End,     sf::Keyboard::PageDown},
    {sf::Keyboard::Numpad8, sf::Keyboard::Numpad4, sf::Keyboard::Numpad5, sf::Keyboard::Numpad6}
};

enum kierunek {
    gora = 0,
    lewo = 1,
    dol = 2,
    prawo = 3
};

unsigned short plansza = 0;
sf::Clock zegar;

random_device rd;
mt19937 maszyna_losujaca(rd());
uniform_int_distribution<int> los_cyfra(1, 9);
uniform_int_distribution<int> los_wysokosc(1, 31);
uniform_int_distribution<int> los_szerokosc(1, 71);
/* */

sf::RenderWindow WONSZ(sf::VideoMode(584, 424, 32), "WONSZ");
int pix_h, pix_w;
sf::Vector2u pix_s;
short players;

void opcje_wczyt()
{
    ifstream p_opcje("./Data/opcje.cfg", ios::in);
    if (p_opcje.good()) {
        string wiersz;
        while (!p_opcje.eof()) {
            getline(p_opcje, wiersz);
            if (wiersz[0] != '[') {
                size_t poz_spacja = wiersz.find(' ');
                if (poz_spacja != string::npos) {
                    string nazwa_opcji = wiersz.substr(0, poz_spacja);
                    if (nazwa_opcji == "dzwiek") {
                        if (wiersz.size() >= 10) {
                            opcje.dzwiek = wiersz[9] - 48;
                        }
                    } else if (nazwa_opcji == "oddzielne_zycia") {
                        if (wiersz.size() >= 19) {
                            opcje.oddzielne_zycia = wiersz[18] - 48;
                        }
                    } else if (nazwa_opcji == "szybkosc") {
                        if (wiersz.size() >= 12) {
                            opcje.szybkosc = stoi(wiersz.substr(11));
                        }
                    }
                }
            }
        }
    } else {
        cerr << "[file][r]Blad przy otwieraniu pliku opcji [Dane/opcje.cfg]\n";
    }
}

void opcje_zapis()
{
    ofstream p_opcje("./Data/opcje.cfg", ios::out);
    if (p_opcje.good()) {
        p_opcje << "[BOOL]\n";
        p_opcje << "dzwiek = " << opcje.dzwiek << '\n';
        p_opcje << "oddzielne_zycia = " << opcje.oddzielne_zycia << '\n';

        p_opcje << "[INT]\n";
        p_opcje << "szybkosc = " << opcje.szybkosc << '\n';
    } else {
        cerr << "[file][w]Blad przy otwieraniu pliku opcji [Dane/opcje.cfg]\n";
    }
}

void ranking_wczyt(vector<pair<int, string>> &ranking)
{
    ranking.resize(10);
    ifstream p_ranking("./Data/ranking.arr", ios::in | ios::binary);
    if (p_ranking.good()) {
        for (int i = 0; i < 10; i++) {
            p_ranking.read((char *)(&ranking[i].first), sizeof(int));
            getline(p_ranking, ranking[i].second);
        }
        p_ranking.close();
    } else {
        p_ranking.open("./Data/ranking.txt", ios::in);
        if (p_ranking.good()) {
            for (int i = 0; i < 10; i++) {
                p_ranking >> ranking[i].first;
            }
            for (int i = 0; i < 10; i++) {
                p_ranking >> ranking[i].second;
            }
            p_ranking.close();
        } else {
            cerr << "[file][r]Blad przy otwieraniu plikow rankingu [Dane/ranking.arr; Dane/ranking.txt]\n";
        }
    }
}

void ranking_zapis(const vector<pair<int, string>> &ranking)
{
    ofstream p_ranking("./Data/ranking.arr", ios::out | ios::binary);
    if (p_ranking.good()) {
        for (int i = 0; i < 10; i++) {
            p_ranking.write((char *)(&ranking[i].first), sizeof(int));
            p_ranking << ranking[i].second << '\n';
        }
        p_ranking.close();
    } else {
        cerr << "[file][w]Blad przy otwieraniu pliku rankingu [Dane/ranking.arr]\n";
    }
}

string zera_wiodace(const string &str, int maks_dlugosc)
{
    if (str.size() < maks_dlugosc) {
        return (string(maks_dlugosc - str.size(), '0').append(str));
    } else {
        return str;
    }
}

wstring zera_wiodace(const wstring &str, int maks_dlugosc)
{
    if (str.size() < maks_dlugosc) {
        return (wstring(maks_dlugosc - str.size(), '0').append(str));
    } else {
        return str;
    }
}

void losu(vector<vector<unsigned char>> &tab, unsigned char &mniam, sf::Text &mniamtext)
{
    unsigned char x, y;
    mniam = los_cyfra(maszyna_losujaca) + 48;
    do {
        x = los_szerokosc(maszyna_losujaca);
        y = los_wysokosc(maszyna_losujaca);
    } while (tab[x][y] != 32);
    tab[x][y] = mniam;
    mniamtext.setPosition(x * 8, (y * 12) - 2);
    mniamtext.setString(to_string(mniam - 48));
}

void poczatek(vector<vector<unsigned char>> &tab, unsigned char &mniam, sf::Text &mniamtext)
{
	for (int i = 0; i < 73; i++) {
		tab[i][0] = 205;
		tab[i][32] = 205;
    }
	for (int i = 0; i < 33; i++) {
		tab[0][i] = 186;
		tab[72][i] = 186;
    }
	tab[0][0] = 201;
	tab[0][32] = 200;
	tab[72][0] = 187;
	tab[72][32] = 188;

    losu(tab, mniam, mniamtext);
	WONSZ.display();
}

void rysuj_basic(sf::Text &mniamtext, sf::Sprite &grasprite)
{
    WONSZ.clear(sf::Color(0, 0, 0));
	WONSZ.draw(grasprite);
	WONSZ.draw(mniamtext);
}

void rysuj_once(int dlugosc, sf::Sprite &headsprite, sf::Sprite &bellysprite, sf::Sprite &tailsprite, sf::Text &wyniktext, vector<vector<unsigned char>> &poz)
{
	WONSZ.draw(headsprite);
	for (int i = 2; i < dlugosc; i++) {
        bellysprite.setPosition(poz[i - 1][0] * 8, poz[i - 1][1] * 12);
        WONSZ.draw(bellysprite);
    }
	WONSZ.draw(wyniktext);
	WONSZ.draw(tailsprite);
}

void ruch(short &raz, unsigned char &pozx, unsigned char &pozy, vector<vector<unsigned char>> &tab, bool &koniec, unsigned char &mniam, int &d, int &dlugosc, vector<vector<unsigned char>> &poz, bool &zjadl, int glpdX, int glpdY, sf::Sprite &headsprite, sf::Sprite &bellysprite, sf::Sprite &tailsprite, sf::Text &mniamtext, int ID)
{
    short pla[6][2];
    switch (players) {
        case 1: {
            pla[0][0] = 36;///
            pla[0][1] = 16;
            break;
        }
        case 2: {
            pla[1][0] = 24;///
            pla[1][1] = 16;
            pla[0][0] = 48;///
            pla[0][1] = 16;
            break;
        }
        case 3: {
            pla[1][0] = 18;///
            pla[1][1] = 16;
            pla[2][0] = 36;///
            pla[2][1] = 16;
            pla[0][0] = 54;///
            pla[0][1] = 16;
            break;
        }
        case 4: {
            pla[1][0] = 24;///
            pla[1][1] = 10;
            pla[2][0] = 48;///
            pla[2][1] = 10;
            pla[3][0] = 24;///
            pla[3][1] = 22;
            pla[0][0] = 48;///
            pla[0][1] = 22;
            break;
        }
        case 5: {
            pla[1][0] = 18;///
            pla[1][1] = 8;
            pla[2][0] = 54;///
            pla[2][1] = 8;
            pla[3][0] = 36;///
            pla[3][1] = 16;
            pla[0][0] = 18;///
            pla[0][1] = 24;
            pla[4][0] = 54;///
            pla[4][1] = 24;
            break;
        }
        case 6: {
            pla[1][0] = 18;///
            pla[1][1] = 10;
            pla[2][0] = 36;///
            pla[2][1] = 10;
            pla[3][0] = 54;///
            pla[3][1] = 10;
            pla[0][0] = 18;///
            pla[0][1] = 22;
            pla[4][0] = 36;///
            pla[4][1] = 22;
            pla[5][0] = 54;///
            pla[5][1] = 22;
            break;
        }
    }

    if (raz == 0) {
        if (glpdY == -1) {
            tab[(pla[ID][0])][(pla[ID][1])] = 'O';
            poz[2][0] = pla[ID][0];
            poz[2][1] = pla[ID][1];

            tab[(pla[ID][0])][(pla[ID][1]) + 1] = 254;
            poz[1][0] = poz[2][0];
            poz[1][1] = poz[2][1] + 1;

            tab[(pla[ID][0])][(pla[ID][1]) + 2] = 4;
            poz[0][0] = poz[1][0];
            poz[0][1] = poz[1][1] + 1;
        }
        if (glpdX == -1) {
            tab[(pla[ID][0])][(pla[ID][1])] = 'O';
            poz[2][0] = pla[ID][0];
            poz[2][1] = pla[ID][1];

            tab[(pla[ID][0]) + 1][(pla[ID][1])] = 254;
            poz[1][0] = poz[2][0] + 1;
            poz[1][1] = poz[2][1];

            tab[(pla[ID][0]) + 2][(pla[ID][1])] = 4;
            poz[0][0] = poz[1][0] + 1;
            poz[0][1] = poz[1][1];
        }
        if (glpdX == 1) {
            tab[(pla[ID][0])][(pla[ID][1])] = 'O';
            poz[2][0] = pla[ID][0];
            poz[2][1] = pla[ID][1];

            tab[(pla[ID][0]) - 1][(pla[ID][1])] = 254;
            poz[1][0] = poz[2][0] - 1;
            poz[1][1] = poz[2][1];

            tab[(pla[ID][0]) - 2][(pla[ID][1])] = 4;
            poz[0][0] = poz[1][0] - 1;
            poz[0][1] = poz[1][1];
        }
        if (glpdY == 1) {
            tab[(pla[ID][0])][(pla[ID][1])] = 'O';
            poz[2][0] = pla[ID][0];
            poz[2][1] = pla[ID][1];

            tab[(pla[ID][0])][(pla[ID][1]) - 1] = 254;
            poz[1][0] = poz[2][0];
            poz[1][1] = poz[2][1] - 1;

            tab[(pla[ID][0])][(pla[ID][1]) - 2] = 4;
            poz[0][0] = poz[1][0];
            poz[0][1] = poz[1][1] - 1;
        }
        headsprite.setPosition(poz[2][0] * 8, poz[2][1] * 12);
        tailsprite.setPosition(poz[0][0] * 8, poz[0][1] * 12);
    } else {
        pozx = poz[dlugosc - 1][0];
        pozy = poz[dlugosc - 1][1];
        if (tab[pozx + glpdX][pozy + glpdY] == 186) {
            koniec = true;
            return;
        }
        if (tab[pozx + glpdX][pozy + glpdY] == 205) {
            koniec = true;
            return;
        }
        if (tab[pozx + glpdX][pozy + glpdY] == 254) {
            koniec = true;
            return;
        }
        if (tab[pozx + glpdX][pozy + glpdY] == 4 && d > 0) {
            koniec = true;
            return;
        }
        if (tab[pozx + glpdX][pozy + glpdY] >= 49 && tab[pozx + glpdX][pozy + glpdY] <= 57) {
            losu(tab, mniam, mniamtext);
            d += tab[pozx + glpdX][pozy + glpdY] - 48;
            zjadl = true;
        }
        if (d > 0) {
            pozx = poz[dlugosc - 1][0];
            pozy = poz[dlugosc - 1][1];
            tab[pozx][pozy] = 254;
            tab[pozx + glpdX][pozy + glpdY] = 'O';
            poz[dlugosc][0] = pozx + glpdX;
            poz[dlugosc][1] = pozy + glpdY;
            headsprite.setPosition((pozx + glpdX) * 8, (pozy + glpdY) * 12);
            d--;
            dlugosc++;
            return;
        }
        pozx = poz[0][0];
        pozy = poz[0][1];
        tab[pozx][pozy] = 32;
        for (int i = 0; i < dlugosc - 1; i++) {
            poz[i][0] = poz[i + 1][0];
            poz[i][1] = poz[i + 1][1];
        }
        pozx = poz[0][0];
        pozy = poz[0][1];
        tab[pozx][pozy] = 4;
        tailsprite.setPosition(pozx * 8, pozy * 12);
        pozx = poz[dlugosc - 1][0];
        pozy = poz[dlugosc - 1][1];
        tab[pozx][pozy] = 254;
        pozx += glpdX;
        pozy += glpdY;
        tab[pozx][pozy] = 'O';
        headsprite.setPosition(pozx * 8, pozy * 12);
        poz[dlugosc - 1][0] += glpdX;
        poz[dlugosc - 1][1] += glpdY;
    }
    raz++;
}

void opposite(char &c, int stare)
{
    if (abs(c - stare) == 2) {
        c = stare;
        return;
    }
}

void rankingowanie(vector<pair<int, string>> &ranking, int wynik, string nick)
{
    if (ranking[0].first < 1) {
        ranking[0].first = wynik;
        ranking[0].second = nick;
        return;
    }
    for (int i = 0; i < 10; i++) {
        if (wynik > ranking[i].first) {
            for (int j = 0; j < 9 - i; j++) {
                ranking[9 - j].first = ranking[9 - j - 1].first;
            }
            for (int j = 0; j < 9 - i; j++) {
                ranking[9 - j].second = ranking[9 - j - 1].second;
            }
            ranking[i].first = wynik;
            ranking[i].second = nick;
            return;
        }
    }
}

bool not_moved(char c[])
{
    for (int i = 0; i < players; i++) {
        if (c[i] == '!') {
            return true;
        }
    }
    return false;
}

void griel(sf::Event event)
{
    bool koniec = false, pla_cho = false;

    sf::Font font;
    font.loadFromFile("./Data/TerminalVector.ttf");

    sf::RectangleShape line1(sf::Vector2f(4, 424)), line2(sf::Vector2f(4, 424)), line3(sf::Vector2f(584, 4));
    line1.setPosition(192, 0);
    line1.setFillColor(sf::Color(255, 255, 255, 180));
    line2.setPosition(388, 0);
    line2.setFillColor(sf::Color(255, 255, 255, 180));
    line3.setPosition(0, 210);
    line3.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text le[6], gr[6];
    for (int l = 0; l < 6; l++) {\
        le[l].setFont(font);
        le[l].setCharacterSize(120);
        le[l].setFillColor(sf::Color(255, 255, 255, 180));
    }
    le[0].setString("1");
    le[0].setPosition(70, 25);
    le[1].setString("2");
    le[1].setPosition(261, 25);
    le[2].setString("3");
    le[2].setPosition(457, 25);
    le[3].setString("4");
    le[3].setPosition(60, 245);
    le[4].setString("5");
    le[4].setPosition(261, 245);
    le[5].setString("6");
    le[5].setPosition(457, 245);

    gr[0].setFont(font);
    gr[0].setCharacterSize(20);
    gr[0].setFillColor(sf::Color(255, 255, 255, 180));
    gr[0].setString("GRACZ");
    for (int k = 1; k < 6; k++) {
        gr[k].setFont(font);
        gr[k].setCharacterSize(20);
        gr[k].setFillColor(sf::Color(255, 255, 255, 180));
        gr[k].setString("GRACZY");
    }
    gr[0].setPosition(65, 155);
    gr[1].setPosition(252, 155);
    gr[2].setPosition(451, 155);
    gr[3].setPosition(57, 375);
    gr[4].setPosition(252, 375);
    gr[5].setPosition(451, 375);


    while (!koniec) {
        while (WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                flagi.koniec = true;
                return;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
                koniec = true;
            }
            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
            }
            if (flagi.pauza) {
                sf::sleep(sf::milliseconds(10));
                continue;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x < (195 * pix_w / 584)) {
                    players = 1;
                    pla_cho = true;
                }
                if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (195 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (390 * pix_w / 584)) {
                    players = 2;
                    pla_cho = true;
                }
                if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (390 * pix_w / 584)) {
                    players = 3;
                    pla_cho = true;
                }
                if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h/424) && sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x < (195 * pix_w / 584)) {
                    players = 4;
                    pla_cho = true;
                }
                if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (195 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (390 * pix_w / 584)) {
                    players = 5;
                    pla_cho = true;
                }
                if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (390 * pix_w / 584)) {
                    players = 6;
                    pla_cho = true;
                }
            }
        }

        if (pla_cho) {
            break;
        }

        if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x < (195 * pix_w / 584)) {
            le[0].setFillColor(sf::Color(255, 255, 255));
            gr[0].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[0].setFillColor(sf::Color(255, 255, 255, 180));
            gr[0].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (195 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (390 * pix_w / 584)) {
            le[1].setFillColor(sf::Color(255, 255, 255));
            gr[1].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[1].setFillColor(sf::Color(255, 255, 255, 180));
            gr[1].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (390 * pix_w / 584)) {
            le[2].setFillColor(sf::Color(255, 255, 255));
            gr[2].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[2].setFillColor(sf::Color(255, 255, 255, 180));
            gr[2].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x < (195 * pix_w / 584)) {
            le[3].setFillColor(sf::Color(255, 255, 255));
            gr[3].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[3].setFillColor(sf::Color(255, 255, 255, 180));
            gr[3].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (195 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (390 * pix_w / 584)) {
            le[4].setFillColor(sf::Color(255, 255, 255));
            gr[4].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[4].setFillColor(sf::Color(255, 255, 255, 180));
            gr[4].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (sf::Mouse::getPosition(WONSZ).y > (212 * pix_h / 424) && sf::Mouse::getPosition(WONSZ).x > (390 * pix_w / 584)) {
            le[5].setFillColor(sf::Color(255, 255, 255));
            gr[5].setFillColor(sf::Color(255, 255, 255));
        } else {
            le[5].setFillColor(sf::Color(255, 255, 255, 180));
            gr[5].setFillColor(sf::Color(255, 255, 255, 180));
        }

        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(line1);
        WONSZ.draw(line2);
        WONSZ.draw(line3);
        for (int e = 0; e < 6; e++) {
            WONSZ.draw(le[e]);
            WONSZ.draw(gr[e]);
        }
        WONSZ.display();
        sf::sleep(sf::milliseconds(10));
    }

    WONSZ.clear(sf::Color(0, 0, 0));
    if (koniec) {
        return;
    }

    sf::Texture gra;
    gra.loadFromFile("./Data/gra.png");
    sf::Sprite grasprite;
    grasprite.setTexture(gra);
    grasprite.setPosition(0, 3);
    WONSZ.draw(grasprite);

    sf::Texture head;
    head.loadFromFile("./Data/glowa.png");
    sf::Sprite headsprite[players];
    for (int p = 0; p < players; p++) {
        headsprite[p].setTexture(head);
    }

    sf::Texture belly;
    belly.loadFromFile("./Data/brzuch.png");
    sf::Sprite bellysprite[players];
    for (int p = 0; p < players; p++) {
        bellysprite[p].setTexture(belly);
    }

    sf::Texture tail;
    tail.loadFromFile("./Data/ogon.png");
    sf::Sprite tailsprite[players];
    for (int p = 0; p < players; p++) {
        tailsprite[p].setTexture(tail);
    }

    sf::Texture nicktexture;
    nicktexture.loadFromFile("./Data/nick.png");
    sf::Sprite nicksprite;
    nicksprite.setTexture(nicktexture);
    nicksprite.setPosition(169, 159);

    sf::Text wyniktext[players], okr[players];
    for (int p = 0; p < players; p++) {
        wyniktext[p].setString("");
        okr[p].setPosition(274, 163);
        wyniktext[p].setFont(font);
        okr[p].setFont(font);
        wyniktext[p].setCharacterSize(12);
        okr[p].setCharacterSize(12);
        wyniktext[p].setFillColor(sf::Color(255, 255, 255, 180));
        okr[p].setFillColor(sf::Color(255, 255, 255, 180));
    }
    switch (players) {
        case 1: {
            wyniktext[0].setPosition(244, 399);
            okr[0].setString(L"↑←→↓");
            break;
        }
        case 2: {
            wyniktext[0].setPosition(0, 399);
            wyniktext[1].setPosition(488, 399);
            okr[0].setString("WADS");
            okr[1].setString(L"↑←→↓");
            break;
        }
        case 3: {
            wyniktext[0].setPosition(0, 399);
            wyniktext[1].setPosition(244, 399);
            wyniktext[2].setPosition(488, 399);
            okr[0].setString("WADS");
            okr[1].setString(L"↑←→↓");
            okr[2].setString("TFHG");
            break;
        }
        case 4: {
            wyniktext[0].setPosition(0, 393);
            wyniktext[1].setPosition(488, 393);
            wyniktext[2].setPosition(0, 405);
            wyniktext[3].setPosition(488, 405);
            okr[0].setString("WADS");
            okr[1].setString(L"↑←→↓");
            okr[2].setString("TFHG");
            okr[3].setString("IJLK");
            break;
        }
        case 5: {
            wyniktext[0].setPosition(0, 393);
            wyniktext[1].setPosition(488, 393);
            wyniktext[2].setPosition(244, 399);
            wyniktext[3].setPosition(0, 405);
            wyniktext[4].setPosition(488, 405);
            okr[0].setString("WADS");
            okr[1].setString(L"↑←→↓");
            okr[2].setString("TFHG");
            okr[3].setString("IJLK");
            okr[4].setString("HomeDelPgdnEnd");
            okr[4].setPosition(234, 163);
            break;
        }
        case 6: {
            wyniktext[0].setPosition(0, 393);
            wyniktext[1].setPosition(244, 393);
            wyniktext[2].setPosition(488, 393);
            wyniktext[3].setPosition(0, 405);
            wyniktext[4].setPosition(244, 405);
            wyniktext[5].setPosition(488, 405);
            okr[0].setString("WADS");
            okr[1].setString(L"↑←→↓");
            okr[2].setString("TFHG");
            okr[3].setString("IJLK");
            okr[4].setString("HomeDelPgdnEnd");
            okr[4].setPosition(234, 163);
            okr[5].setString("8465");
            break;
        }
    }

    sf::Text mniamtext("", font);
    mniamtext.setCharacterSize(12);
    mniamtext.setFillColor(sf::Color(255, 255, 255, 180));

    sf::SoundBuffer krokbuffer;
	krokbuffer.loadFromFile("./Data/krok.wav");
	sf::Sound krok;
	krok.setBuffer(krokbuffer);

    sf::SoundBuffer stukbuffer;
	stukbuffer.loadFromFile("./Data/stuk.wav");
	sf::Sound stuk;
	stuk.setBuffer(stukbuffer);

    sf::SoundBuffer hapsbuffer;
	hapsbuffer.loadFromFile("./Data/mniam.wav");
	sf::Sound haps;
	haps.setBuffer(hapsbuffer);

    sf::SoundBuffer introbuffer;
	introbuffer.loadFromFile("./Data/intro.wav");
	sf::Sound intro;
	intro.setBuffer(introbuffer);

    sf::SoundBuffer tlobuffer;
	tlobuffer.loadFromFile("./Data/tlo.wav");
	sf::Sound tlo;
	tlo.setBuffer(tlobuffer);

    short raz[players];
    int decimal = 10, sen = 0, kon = players;
    char sseenn = '=';
    int wynik[players], score[players], dlugosc[players], d[players];
    string reszta = "", wyn[players];
    char c[players], stare[players];
    vector<vector<unsigned char>> tab(73, vector<unsigned char>(33, ' '));
    vector<vector<vector<unsigned char>>> poz(players, vector<vector<unsigned char>>(2201, vector<unsigned char>(2)));
    unsigned char mniam = 0, pozx = 0, pozy = 0;
    bool zjadl = false, w_grze[players];
    for (int l = 0; l < players; l++) {
        raz[l] = 0;
        wynik[l] = 0;
        score[l] = 0;
        dlugosc[l] = 3;
        d[l] = 0;
        wyn[l] = "";
        c[l] = '!';
        stare[l] = '1';
        w_grze[l] = true;
    }
    poczatek(tab, mniam, mniamtext);

    do {
        while(WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                flagi.koniec = true;
                return;
            }

            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
            }
            if (flagi.pauza) {
                break;
            }

            if (event.type == sf::Event::KeyPressed) {
                for (int i = 0; i < players; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (event.key.code == sterowanie[i][j]) {
                            c[i] = j;
                        }
                    }
                }
                if (event.key.code == sf::Keyboard::P) {
                    c[0] = 'p';
                }
                if (event.key.code == sf::Keyboard::F2 && flagi.dev) {
                    d[0] += 10;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                koniec = true;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                sseenn = '/';
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                sseenn = '*';
            } else {
                sseenn = '=';
            }
        }

        if (c[0] == 'p' || flagi.pauza == true || not_moved(c)) {
            sf::sleep(sf::milliseconds(10));
            continue;
        }

        for (int p = 0; p < players; p++) {
            opposite(c[p], stare[p]);
            if (w_grze[p]) {
                switch (c[p]) {
                    case kierunek::gora: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 0, -1, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case kierunek::lewo: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, -1, 0, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case kierunek::dol: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 0, 1, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case kierunek::prawo: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 1, 0, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    default: {
                        continue;
                    }
                }
            }
            if (koniec && opcje.oddzielne_zycia) {
                w_grze[p] = false;
                kon--;
                koniec = false;
            }
        }
        if (kon == 0) {
            koniec = true;
        }

		if (opcje.dzwiek == true) {
            if (koniec == true) {
                stuk.play();
            } else {
                if (zjadl == true) {
                    haps.play();
                } else {
                    krok.play();
                }
            }
		}
        zjadl = false;

        for (int p = 0; p < players; p++) {
            stare[p] = c[p];
        }
        for (int p = 0; p < players; p++) {
            wynik[p] = (dlugosc[p] - 3) * opcje.szybkosc;
        }
        if (players > 1) {
            wynik[0] = wynik[1];
            wynik[1] = (dlugosc[0] - 3) * opcje.szybkosc;
        }
        for (int p=0; p<players; p++) {
            score[p] = wynik[p];
        }

        for (int p = 0; p < players; p++) {
            wyn[p] = "";
            if (score[p] == 0) {
                wyn[p] = "00000";
            } else {
                while (score[p]!=0) {
                    switch ((score[p] % decimal) / (decimal / 10)) {
                        case 0: {
                            reszta = "0";
                            break;
                        }
                        case 1: {
                            reszta = "1";
                            break;
                        }
                        case 2: {
                            reszta = "2";
                            break;
                        }
                        case 3: {
                            reszta = "3";
                            break;
                        }
                        case 4: {
                            reszta = "4";
                            break;
                        }
                        case 5: {
                            reszta = "5";
                            break;
                        }
                        case 6: {
                            reszta = "6";
                            break;
                        }
                        case 7: {
                            reszta = "7";
                            break;
                        }
                        case 8: {
                            reszta = "8";
                            break;
                        }
                        case 9: {
                            reszta = "9";
                            break;
                        }
                    }
                    wyn[p] = reszta + wyn[p];
                    score[p] -= (score[p] % decimal);
                    decimal = decimal * 10;
                }
            }
            if (decimal == 100) {
                wyn[p] = "0000" + wyn[p];
            }
            if (decimal == 1000) {
                wyn[p] = "000" + wyn[p];
            }
            if (decimal == 10000) {
                wyn[p] = "00" + wyn[p];
            }
            if (decimal == 100000) {
                wyn[p] = "0" + wyn[p];
            }
            decimal = 10;
            wyn[p] = "WYNIK:" + wyn[p];
            wyniktext[p].setString(wyn[p]);
        }

        rysuj_basic(mniamtext, grasprite);
        for (int p = 0; p < players; p++) {
            rysuj_once(dlugosc[p], headsprite[p], bellysprite[p], tailsprite[p], wyniktext[p], poz[p]);
        }
        WONSZ.display();

        sf::Time czas = zegar.restart();
        sf::Time czas_snu;
        if (czas.asMilliseconds() < 17) {
            czas_snu = sf::milliseconds(16 - czas.asMilliseconds());
            czas_snu += sf::milliseconds((31 - opcje.szybkosc) * 50 / 3);
        } else {
            czas_snu = sf::milliseconds((31 - opcje.szybkosc) * 50 / 3);
        }
        switch (sseenn) {
            case '*': {
                czas_snu *= sf::Int64(2);
                break;
            }
            case '/': {
                czas_snu /= sf::Int64(2);
                break;
            }
            default: {
                break;
            }
        }
        /* debug *
        cout << '[' << opcje.szybkosc << ']' << czas_snu.asMilliseconds() << "ms\n";
        /* */
        sf::sleep(czas_snu);
    } while (koniec != true);

    char spacja = 32;
    string user;
    bool backspc, enter, rajt;

    sf::Text nicktext("", font);
    nicktext.setPosition(310, 189);
    nicktext.setCharacterSize(12);
    nicktext.setFillColor(sf::Color(255, 255, 255, 180));
    int hajskor[10];
    string nejm[10];
    fstream plik;

    for (int p = 0; p < players; p++) {
        if (wynik[p] > 0) {
            user = "";
            backspc = false;
            nicktext.setString(user);
            enter = false;
            rajt = false;
            while (enter == false || user.length() == 0) {
                while(WONSZ.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        flagi.koniec = true;
                        return;
                    }
                    if (event.type == sf::Event::LostFocus) {
                        flagi.pauza = true;
                    }
                    if (event.type == sf::Event::GainedFocus) {
                        flagi.pauza = false;
                    }
                    if (flagi.pauza) {
                        sf::sleep(sf::milliseconds(10));
                        continue;
                    }
                    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
                        rajt = true;
                    }
                    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return) {
                        enter = true;
                    }
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                        if (user.length() > 0) {
                            user.erase(user.end() - 1);
                            nicktext.setString(user);
                            backspc = true;
                        } else {
                            backspc = true;
                        }
                    }
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                        user += " ";
                        nicktext.setString(user);
                        backspc = true;
                    }
                    if (backspc != true) {
                        if (event.type == sf::Event::TextEntered) {
                            if (event.text.unicode < 128 && event.text.unicode > 31) {
                                user += char(event.text.unicode);
                                nicktext.setString(user);
                            }
                        }
                    }
                }
                if (rajt) {
                    break;
                }
                backspc = false;
                if (user.length() > 12) {
                    user.erase(user.end() - 1);
                    nicktext.setString(user);
                }
                WONSZ.clear(sf::Color(0, 0, 0));
                WONSZ.draw(grasprite);
                for (int r = 0; r < players; r++) {
                    WONSZ.draw(headsprite[r]);
                }
                for (int r = 0; r < players; r++) {
                    for (int i = 2; i < dlugosc[r]; i++) {
                        bellysprite[r].setPosition(poz[r][i - 1][0] * 8, poz[r][i - 1][1] * 12);
                        WONSZ.draw(bellysprite[r]);
                    }
                }
                for (int r = 0; r < players; r++) {
                    WONSZ.draw(wyniktext[r]);
                }
                WONSZ.draw(mniamtext);
                for (int r = 0; r < players; r++) {
                    WONSZ.draw(tailsprite[r]);
                }
                WONSZ.draw(nicksprite);
                WONSZ.draw(okr[p]);
                WONSZ.draw(nicktext);
                WONSZ.display();
                sf::sleep(sf::milliseconds(10));
            }
            if (!rajt) {
                vector<pair<int, string>> ranking;
                ranking_wczyt(ranking);
                rankingowanie(ranking, wynik[p], user);
                ranking_zapis(ranking);
            }
        }
    }
    sf::sleep(sf::milliseconds(150));
}

void szybmanip(wstring &szybk)
{
    if (opcje.szybkosc > 30) {
        opcje.szybkosc = 1;
    }
    if (opcje.szybkosc < 1) {
        opcje.szybkosc = 30;
    }
    szybk = zera_wiodace(to_wstring(opcje.szybkosc), 2);
}

void plansza_opcje(sf::Font font, sf::Event event, sf::Sound tlo)
{
    wstring szybk;
    szybmanip(szybk);

	sf::Texture t_opcje;
    t_opcje.loadFromFile("./Data/opcje.png");
    sf::Sprite opcjesprite;
    opcjesprite.setTexture(t_opcje);
    opcjesprite.setPosition(208, 60);

    sf::Texture t_reset;
    t_reset.loadFromFile("./Data/reset.png");
    sf::Sprite resetsprite;
    resetsprite.setTexture(t_reset);
    resetsprite.setPosition(96, 180);

    sf::Texture t_strzalka_gora;
    t_strzalka_gora.loadFromFile("./Data/strzalka_gora.png");
    sf::Sprite strzalka_gora;
    strzalka_gora.setTexture(t_strzalka_gora);
    sf::Sprite strzalka_dol;
    strzalka_dol.setTexture(t_strzalka_gora);
    strzalka_dol.rotate(180);

    vector<sf::Text> opcja(4);
    opcja[0].setString(wstring(L"DŹWIĘK: ") + wstring(opcje.dzwiek ? L"TAK" : L"NIE"));
    opcja[0].setPosition(248, 142);
    opcja[1].setString(wstring(L"SZYBKOŚĆ WĘŻA: ") + szybk);
    opcja[1].setPosition(216, 190);
    opcja[2].setString(L"RESET WYNIKÓW");
    opcja[2].setPosition(240, 238);
    opcja[3].setString(wstring(L"ODDZIELNE ZYCIA: ") + wstring(opcje.oddzielne_zycia ? L"TAK" : L"NIE"));
    opcja[3].setPosition(205, 286);
    for (int i = 0; i < 4; i++) {
        opcja[i].setFont(font);
        opcja[i].setCharacterSize(12);
        opcja[i].setFillColor(sf::Color(255, 255, 255, 180));
    }

    bool reset = false, opcje_koniec = false;

	while (!opcje_koniec) {
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(opcjesprite);
        for (int i = 0; i < 4; i++) {
            WONSZ.draw(opcja[i]);
        }
        pix_s = WONSZ.getSize();
        pix_w = pix_s.x;
        pix_h = pix_s.y;

        strzalka_gora.setPosition(312, 134);
        WONSZ.draw(strzalka_gora);
        if (sf::Mouse::getPosition(WONSZ).x > (312 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (335 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (130 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (140 * pix_h / 416)) {
            WONSZ.draw(strzalka_gora);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.dzwiek = !opcje.dzwiek;
                opcja[0].setString(wstring(L"DŹWIĘK: ") + wstring(opcje.dzwiek ? L"TAK" : L"NIE"));
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        strzalka_gora.setPosition(332, 182);
        WONSZ.draw(strzalka_gora);
        if (sf::Mouse::getPosition(WONSZ).x > (332 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (355 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (176 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (186 * pix_h / 416)) {
            WONSZ.draw(strzalka_gora);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.szybkosc++;
                szybmanip(szybk);
                opcja[1].setString(wstring(L"SZYBKOŚĆ WĘŻA: ") + szybk);
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        strzalka_gora.setPosition(341, 278);
        WONSZ.draw(strzalka_gora);
        if (sf::Mouse::getPosition(WONSZ).x > (341 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (364 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (270 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (280 * pix_h / 416)) {
            WONSZ.draw(strzalka_gora);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.oddzielne_zycia =! opcje.oddzielne_zycia;
                opcja[3].setString(wstring(L"ODDZIELNE ŻYCIA: ") + wstring(opcje.oddzielne_zycia ? L"TAK" : L"NIE"));
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        strzalka_dol.setPosition(335, 165);
        WONSZ.draw(strzalka_dol);
        if (sf::Mouse::getPosition(WONSZ).x > (312 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (335 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (154 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (164 * pix_h / 416)) {
            WONSZ.draw(strzalka_dol);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.dzwiek =! opcje.dzwiek;
                opcja[0].setString(wstring(L"DŹWIĘK: ") + wstring(opcje.dzwiek ? L"TAK" : L"NIE"));

                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        strzalka_dol.setPosition(355, 213);
        WONSZ.draw(strzalka_dol);
        if (sf::Mouse::getPosition(WONSZ).x > (332 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (355 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (202 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (212 * pix_h / 416)) {
            WONSZ.draw(strzalka_dol);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.szybkosc--;
                szybmanip(szybk);
                opcja[1].setString(wstring(L"SZYBKOŚĆ WĘŻA: ") + szybk);
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        strzalka_dol.setPosition(364, 309);
        WONSZ.draw(strzalka_dol);
        if (sf::Mouse::getPosition(WONSZ).x > (341 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (364 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (296 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (306 * pix_h / 416)) {
            WONSZ.draw(strzalka_dol);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                opcje.oddzielne_zycia =! opcje.oddzielne_zycia;
                opcja[3].setString(wstring(L"ODDZIELNE ŻYCIA: ") + wstring(opcje.oddzielne_zycia ? L"TAK" : L"NIE"));
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        }
        if (sf::Mouse::getPosition(WONSZ).x > (230 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).x < (336 * pix_w / 584) && sf::Mouse::getPosition(WONSZ).y > (232 * pix_h / 416) && sf::Mouse::getPosition(WONSZ).y < (250 * pix_h / 416)) {
            opcja[2].setFillColor(sf::Color(255, 255, 255, 255));
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                reset = true;
                while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::sleep(sf::milliseconds(10));
                    continue;
                }
            }
        } else {
            opcja[2].setFillColor(sf::Color(255, 255, 255, 180));
        }
        if (reset) {
            WONSZ.draw(resetsprite);
        }

        while (WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (opcje.dzwiek) {
                    tlo.stop();
                }
                flagi.koniec = true;
                return;
            }
            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
            }
            if (flagi.pauza) {
                sf::sleep(sf::milliseconds(10));
                continue;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T && reset) {
                ofstream ranks;
                ranks.open("./Data/ranking.arr", ios::out | ios::binary);
                if (ranks.good()) {
                    string wiersz = "\0\0\0\0\n";
                    for (int i = 0; i < 10; i++) {
                        ranks << wiersz;
                    }
                    ranks.close();
                }
                reset = false;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N && reset==true) {
                reset = false;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                opcje_koniec = true;
            }
        }
        WONSZ.display();
        sf::sleep(sf::milliseconds(10));
    }

    opcje_zapis();
}

void intro(sf::Texture &menu, sf::Font &font, sf::SoundBuffer &tlobuffer, sf::Texture &pomoctexture, sf::Event &introevent)
{
    sf::SoundBuffer introbuffer;
	introbuffer.loadFromFile("./Data/intro.wav");
	sf::Sound introsnd;
	introsnd.setBuffer(introbuffer);

    sf::Texture intro;
    intro.loadFromFile("./Data/intro.png");
    sf::Sprite introsprite;
    introsprite.setTexture(intro);
    introsprite.setPosition(72, 155);

    sf::Texture t_wonszintro1, t_wonszintro2;
    sf::Sprite wonszintrosprite1, wonszintrosprite2;

    WONSZ.clear(sf::Color(0, 0, 0));
    WONSZ.draw(introsprite);
    WONSZ.display();

    int wonszintr = 128;

    t_wonszintro1.loadFromFile("./Data/introwonsz1.png");
    wonszintrosprite1.setTexture(t_wonszintro1);
    wonszintrosprite1.setPosition(136, 132);
    t_wonszintro2.loadFromFile("./Data/introwonsz2.png");
    wonszintrosprite2.setTexture(t_wonszintro2);
    wonszintrosprite2.setPosition(wonszintr, 132);

    for (int xd = -10; xd < 35; xd++) {
        while(WONSZ.pollEvent(introevent)) {
            if (introevent.type == sf::Event::Closed) {
                if (opcje.dzwiek) {
                    introsnd.stop();
                }
                flagi.koniec = true;
                return;
            }
            if (introevent.type == sf::Event::KeyPressed && introevent.key.code == sf::Keyboard::Space) {
                menu.loadFromFile("./Data/menu.png");
                font.loadFromFile("./Data/TerminalVector.ttf");
                tlobuffer.loadFromFile("./Data/tlo.wav");
                pomoctexture.loadFromFile("./Data/pomoc.png");
                return;
            }
            if (introevent.type == sf::Event::MouseButtonReleased) {
                menu.loadFromFile("./Data/menu.png");
                font.loadFromFile("./Data/TerminalVector.ttf");
                tlobuffer.loadFromFile("./Data/tlo.wav");
                pomoctexture.loadFromFile("./Data/pomoc.png");
                return;
            }
            if (introevent.type == sf::Event::LostFocus) {
                flagi.pauza = true;
                introsnd.pause();
            }
            if (introevent.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
                if (opcje.dzwiek) {
                    introsnd.play();
                }
            }
        }
        if (flagi.pauza) {
            sf::sleep(sf::milliseconds(10));
            xd--;
            continue;
        }
        if (xd == 0) {
            if (opcje.dzwiek) {
                introsnd.play();
            }
        }
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(introsprite);
        if (xd >= 0) {
            WONSZ.draw(wonszintrosprite2);
            wonszintr += 8;
            wonszintrosprite2.setPosition(wonszintr, 132);
        } else {
            WONSZ.draw(wonszintrosprite1);
        }
        WONSZ.display();
        sf::sleep(sf::milliseconds(100));
    }
    menu.loadFromFile("./Data/menu.png");
    font.loadFromFile("./Data/TerminalVector.ttf");
	tlobuffer.loadFromFile("./Data/tlo.wav");
    pomoctexture.loadFromFile("./Data/pomoc.png");
    sf::sleep(sf::milliseconds(300));
}

void ranking(sf::Event event, sf::Sound tlo)
{
    sf::Font font;
    font.loadFromFile("./Data/TerminalVector.ttf");

    sf::Texture tekstura_ranking;
    tekstura_ranking.loadFromFile("./Data/ranking.png");
    sf::Sprite rankingsprite;
    rankingsprite.setTexture(tekstura_ranking);
    rankingsprite.setPosition(178, 24);

    vector<pair<int, string>> ranking;
    string hajskor[10];

    ranking_wczyt(ranking);

    vector<sf::Text> lista(10);
    for (int i = 0; i < 10; i++) {
        lista[i].setCharacterSize(12);
        lista[i].setFont(font);
        lista[i].setFillColor(sf::Color(255, 255, 255, 180));
        if (ranking[i].first > 0) {
            string wpis = to_string(i + 1) + ". " + ranking[i].second + " " + zera_wiodace(to_string(ranking[i].first), 5);
            lista[i].setString(wpis);
            lista[i].setPosition((292 - wpis.size() * 4), 132 + i * 24);
        } else {
            lista[i].setString(to_string(i + 1) + ". -----");
            lista[i].setPosition(260 - (i == 9) * 4, 132 + i * 24);
        }
    }

    bool ranking_koniec = false;

    while (!ranking_koniec) {
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(rankingsprite);
        for (int i = 0; i < 10; i++) {
            WONSZ.draw(lista[i]);
        }
        WONSZ.display();
        while(WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (opcje.dzwiek) {
                    tlo.stop();
                }
                flagi.koniec = true;
                return;
            }
            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
            }
            if (flagi.pauza) {
                sf::sleep(sf::milliseconds(10));
                continue;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                ranking_koniec = true;
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
}

void pomoc(sf::Event event, sf::Sprite pomocsprite, sf::Sound tlo)
{
    bool pomoc_koniec = false;
    while (!pomoc_koniec) {
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(pomocsprite);
        WONSZ.display();
        while(WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (opcje.dzwiek) {
                    tlo.stop();
                }
                flagi.koniec = true;
                return;
            }
            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
            }
            if (flagi.pauza) {
                sf::sleep(sf::milliseconds(10));
                continue;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                pomoc_koniec = true;
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
}

int main()
{
    WONSZ.setVerticalSyncEnabled(true);

    sf::Texture menu;
    sf::Font font;
    sf::SoundBuffer tlobuffer;
    sf::Texture pomoctexture;
    sf::Event event;

	opcje_wczyt();

                                                            ///INTRO

    intro(menu, font, tlobuffer, pomoctexture, event);


    sf::Sprite menusprite;
    menusprite.setTexture(menu);
    menusprite.setPosition(136, 48);

    sf::Sprite pomocsprite;
    pomocsprite.setTexture(pomoctexture);
    pomocsprite.setPosition(40, 36);


    vector<sf::Text> przycisk(5);
    przycisk[0].setString("GRA");
    przycisk[0].setPosition(280, 190);
    przycisk[1].setString("RANKING");
    przycisk[1].setPosition(264, 226);
    przycisk[2].setString("OPCJE");
    przycisk[2].setPosition(272, 262);
    przycisk[3].setString("POMOC");
    przycisk[3].setPosition(272, 298);
    przycisk[4].setString(L"WYJŚCIE");
    przycisk[4].setPosition(264, 334);
    for (int i = 0; i < 5; i++) {
        przycisk[i].setFont(font);
        przycisk[i].setCharacterSize(12);
        przycisk[i].setFillColor(sf::Color(255, 255, 255, 180));
    }

	sf::Sound tlo;
	tlo.setBuffer(tlobuffer);
	tlo.setLoop(true);

    ifstream dev_file("./Data/dev", ios::in);
    if (dev_file.good()) {
        dev_file >> flagi.dev;
        dev_file.close();
        cout << "Developer mode!\n";
    } else {
        flagi.dev = false;
    }

    int esc_lvl = 1;
    int place = 0;

    if (flagi.koniec) {
        WONSZ.close();
        return 0;
    }

    if (opcje.dzwiek) {
        tlo.play();
    }

    while (WONSZ.isOpen()) {
        pix_s = WONSZ.getSize();
        pix_w = pix_s.x;
        pix_h = pix_s.y;
        while (WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                WONSZ.close();
            }
            if (event.type == sf::Event::LostFocus) {
                flagi.pauza = true;
                tlo.pause();
            }
            if (event.type == sf::Event::GainedFocus) {
                flagi.pauza = false;
                if (opcje.dzwiek) {
                    tlo.play();
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (180*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (210*pix_h/416)) {
                    place = 1;
                    esc_lvl++;
                }
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (215*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (245*pix_h/416)) {
                    place = 2;
                    esc_lvl++;
                }
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (255*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (285*pix_h/416)) {
                    place = 3;
                    esc_lvl++;
                }
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (290*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (320*pix_h/416)) {
                    place = 4;
                    esc_lvl++;
                }
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (325*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (355*pix_h/416)) {
                    esc_lvl--;
                }
            }
        }

        if (esc_lvl == 0) {
            if (opcje.dzwiek == true) {
                tlo.stop();
            }
            WONSZ.close();
        }
        if (esc_lvl == 1) {
            place = 0;
        }

        if (flagi.pauza) {
            sf::sleep(sf::milliseconds(10));
            continue;
        }

        switch (place) {
            case 0: {
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (180*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (210*pix_h/416)) {
                    przycisk[0].setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk[0].setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (215*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (245*pix_h/416)) {
                    przycisk[1].setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk[1].setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (255*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (285*pix_h/416)) {
                    przycisk[2].setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk[2].setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (290*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (320*pix_h/416)) {
                    przycisk[3].setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk[3].setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (325*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (355*pix_h/416)) {
                    przycisk[4].setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk[4].setFillColor(sf::Color(255, 255, 255, 180));
                }

                WONSZ.clear(sf::Color(0, 0, 0));
                WONSZ.draw(menusprite);
                for (int i = 0; i < 5; i++) {
                    WONSZ.draw(przycisk[i]);
                }
                WONSZ.display();
                break;
            }
            case 1: {
                WONSZ.clear(sf::Color(0, 0, 0));
                if (opcje.dzwiek) {
                    tlo.stop();
                }
                griel(event);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
                }
                esc_lvl--;
                if (opcje.dzwiek) {
                    tlo.play();
                }
                break;
            }
            case 2: {
                WONSZ.clear(sf::Color(0, 0, 0));
                ranking(event, tlo);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
                }
                esc_lvl--;
                break;
            }
            case 3: {
                WONSZ.clear(sf::Color(0, 0, 0));
                plansza_opcje(font, event, tlo);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
                }
                tlo.pause();
                if (opcje.dzwiek) {
                    tlo.play();
                } else {
                    tlo.stop();
                }
                esc_lvl--;
                break;
            }
            case 4: {
                WONSZ.clear(sf::Color(0, 0, 0));
                pomoc(event, pomocsprite, tlo);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
                }
                esc_lvl--;
                break;
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
    return 0;
}
