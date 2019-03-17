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
    bool dzwiek,
         dev,
         pauza,
         koniec;
} flagi = {true, false, false, false};
unsigned short plansza = 0;

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

void rysuj_basic(sf::Text mniamtext, sf::Sprite grasprite)
{
    WONSZ.clear(sf::Color(0, 0, 0));
	WONSZ.draw(grasprite);
	WONSZ.draw(mniamtext);
}

void rysuj_once(int dlugosc, sf::Sprite &headsprite, sf::Sprite &bellysprite, sf::Sprite &tailsprite, sf::Text wyniktext, vector<vector<unsigned char>> &poz)
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
        pozx=poz[dlugosc - 1][0];
        pozy=poz[dlugosc - 1][1];
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
    if (c == -stare) {
        c = stare;
        return;
    }
}

void rankingowanie(int hajskor[10], string nejm[10], int wynik, string nick)
{
    if (hajskor[0] < 1) {
        hajskor[0] = wynik;
        nejm[0] = nick;
        return;
    }
    for (int i = 0; i < 10; i++) {
        if (wynik > hajskor[i]) {
            for (int j = 0; j < 9 - i; j++) {
                hajskor[9 - j] = hajskor[9 - j - 1];
            }
            for (int j = 0; j < 9 - i; j++) {
                nejm[9 - j] = nejm[9 - j - 1];
            }
            hajskor[i] = wynik;
            nejm[i] = nick;
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
    /*fstream f_players;
    f_players.open("./Data/players.txt", ios::in);
    if(f_players.good())
        {f_players >> players;
        f_players.close();
        }

    if (players<1 || players>6)
        players=1;*/

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
                Sleep(10);
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
        Sleep(10);
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

    bool oddzielne;
    ifstream tog_sep;
    tog_sep.open("./Data/tog-sep.txt", ios::in);
    if (tog_sep.good()) {
        tog_sep >> oddzielne;
        tog_sep.close();
    }

    short raz[players];
    int decimal = 10, sen = 0, sseenn = 0, kon = players;
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
    ifstream pliksen;
    pliksen.open("./Data/sen.txt", ios::in);
    if (pliksen.good()) {
        pliksen >> sen;
        pliksen.close();
    }
    sseenn=sen;

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

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
                c[0] = -2;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                c[0] = -1;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                c[0] = 1;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
                c[0] = 2;
            }

            if (players > 1) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                    c[1] = -2;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                    c[1] = -1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                    c[1] = 1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    c[1] = 2;
                }
            }

            if (players > 2) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) {
                    c[2] = -2;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
                    c[2] = -1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H) {
                    c[2] = 1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
                    c[2] = 2;
                }
            }

            if (players>3) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I) {
                    c[3] = -2;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::J) {
                    c[3] = -1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                    c[3] = 1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::K) {
                    c[3] = 2;
                }
            }

            if (players>4) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Home) {
                    c[4] = -2;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                    c[4] = -1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::PageDown) {
                    c[4] = 1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::End) {
                    c[4] = 2;
                }
            }

            if (players>5) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Numpad8) {
                    c[5] = -2;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Numpad4) {
                    c[5] = -1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Numpad6) {
                    c[5] = 1;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Numpad5) {
                    c[5] = 2;
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                c[0] = 'p';
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                koniec=true;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2 && flagi.dev == true) {
                d[0] = 0;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                sseenn = sen/2;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                sseenn = sen*2;
            } else {
                sseenn = sen;
            }
        }

        if (c[0] == 'p' || flagi.pauza == true || not_moved(c)) {
            Sleep(10);
            continue;
        }

        for (int p = 0; p < players; p++) {
            opposite(c[p], stare[p]);
            if (w_grze[p]) {
                switch (c[p]) {
                    case -2: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 0, -1, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case -1: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, -1, 0, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case 1: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 1, 0, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    case 2: {
                        ruch(raz[p], pozx, pozy, tab, koniec, mniam, d[p], dlugosc[p], poz[p], zjadl, 0, 1, headsprite[p], bellysprite[p], tailsprite[p], mniamtext, p);
                        break;
                    }
                    default: {
                        continue;
                    }
                }
            }
            if (koniec && oddzielne) {
                w_grze[p] = false;
                kon--;
                koniec = false;
            }
        }
        if (kon == 0) {
            koniec = true;
        }

		if (flagi.dzwiek == true) {
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
            wynik[p] = dlugosc[p] - 3;
        }
        if (players > 1) {
            wynik[0] = wynik[1];
            wynik[1] = dlugosc[0] - 3;
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

        Sleep(sseenn);
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
                        Sleep(10);
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
                            user.erase(user.end()-1);
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
                            if (event.text.unicode < 128) {
                                user += static_cast<char>(event.text.unicode);
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
                Sleep(10);
            }
            if (!rajt) {
                plik.open("./Data/ranking.txt", ios::in | ios::out);
                if (plik.good() == true) {
                    for (int i = 0; i < 10; i++) {
                        plik >> hajskor[i];
                    }
                    for (int i = 0; i < 10; i++) {
                        plik >> nejm[i];
                    }
                    plik.seekg(0, ios_base::beg);
                    rankingowanie(hajskor, nejm, wynik[p], user);
                    for (int i = 0; i < 10; i++) {
                        plik << hajskor[i] << (char)spacja;
                    }
                    for (int i = 0; i < 10; i++) {
                        plik << nejm[i] << (char)spacja;
                    }
                    plik.close();
                }
            }
        }
    }
    Sleep(150);
}

void szybwczyt(int &szybkosc, sf::String &szybk)
{
    szybkosc /= -10;
    szybkosc += 30;
    szybk = to_string(szybkosc);
    if (szybk.getSize() == 1) {
        szybk.insert(0, " ");
    }
}

void szybmanip(int &szybkosc, sf::String &szybk)
{
    if (szybkosc > 30) {
        szybkosc = 1;
    }
    if (szybkosc < 1) {
        szybkosc = 30;
    }
    szybk = to_string(szybkosc);
    if (szybk.getSize() == 1) {
        szybk.insert(0, " ");
    }
}

void szybwyp(int &szybkosc)
{
    szybkosc *= -10;
    szybkosc += 300;
}

void opcje(sf::String &dzwiek01, sf::Font font, sf::String &szybk, int &szybkosc, sf::Event event, sf::Sound tlo)
{
	sf::Texture opcje;
    opcje.loadFromFile("./Data/opcje.png");
    sf::Sprite opcjesprite;
    opcjesprite.setTexture(opcje);
    opcjesprite.setPosition(48, 60);

    sf::Texture opcjerank;
    opcjerank.loadFromFile("./Data/opcjerank.png");
    sf::Sprite opcjeranksprite;
    opcjeranksprite.setTexture(opcjerank);
    opcjeranksprite.setPosition(48, 60);

    sf::Texture resettexture;
    resettexture.loadFromFile("./Data/reset.png");
    sf::Sprite resetsprite;
    resetsprite.setTexture(resettexture);
    resetsprite.setPosition(96, 180);

    sf::Text opcja1(dzwiek01, font);
    opcja1.setPosition(112, 142);
    opcja1.setCharacterSize(12);
    opcja1.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text opcja2(szybk, font);
    opcja2.setPosition(336, 190);
    opcja2.setCharacterSize(12);
    opcja2.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Texture up0;
    up0.loadFromFile("./Data/up.png");
    sf::Sprite upsprite;
    upsprite.setTexture(up0);

    sf::Texture down;
    down.loadFromFile("./Data/down.png");
    sf::Sprite downsprite;
    downsprite.setTexture(down);

    bool reset = false, opcje_koniec = false, oddzielne;

    ifstream tog_sep;
    tog_sep.open("./Data/tog-sep.txt", ios::in);
    if (tog_sep.good()) {
        tog_sep >> oddzielne;
        tog_sep.close();
    }

    string odd_stan;

    if (oddzielne) {
        odd_stan = "TAK";
    } else {
        odd_stan = "NIE";
    }

    sf::Text opcja3("ODDZIELNE ZYCIA: " + odd_stan, font);
    opcja3.setPosition(205, 286);
    opcja3.setCharacterSize(12);
    opcja3.setFillColor(sf::Color(255, 255, 255, 180));

	while (!opcje_koniec) {
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(opcjesprite);
        WONSZ.draw(opcja1);
        WONSZ.draw(opcja2);
        WONSZ.draw(opcja3);
        upsprite.setPosition(112, 134);
        WONSZ.draw(upsprite);
        pix_s = WONSZ.getSize();
        pix_w = pix_s.x;
        pix_h = pix_s.y;

        if (sf::Mouse::getPosition(WONSZ).x > (112*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (135*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (134*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (140*pix_h/416)) {
            WONSZ.draw(upsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (112*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (135*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (134*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (140*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            flagi.dzwiek = !flagi.dzwiek;
            if (flagi.dzwiek) {
                dzwiek01 = "TAK";
            } else {
                dzwiek01 = "NIE";
            }
            opcja1.setString(dzwiek01);
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        upsprite.setPosition(332, 182);
        WONSZ.draw(upsprite);
        if (sf::Mouse::getPosition(WONSZ).x > (332*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (355*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (182*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (188*pix_h/416)) {
            WONSZ.draw(upsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (332*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (355*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (182*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (188*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            szybkosc++;
            szybmanip(szybkosc, szybk);
            opcja2.setString(szybk);
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        upsprite.setPosition(341, 278);
        WONSZ.draw(upsprite);
        if (sf::Mouse::getPosition(WONSZ).x > (341*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (364*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (275*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (281*pix_h/416)) {
            WONSZ.draw(upsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (341*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (364*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (275*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (281*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            oddzielne =! oddzielne;
            if (oddzielne) {
                odd_stan = "ODDZIELNE ZYCIA: TAK";
            } else {
                odd_stan = "ODDZIELNE ZYCIA: NIE";
            }
            opcja3.setString(odd_stan);

            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        downsprite.setPosition(112, 160);
        WONSZ.draw(downsprite);
        if (sf::Mouse::getPosition(WONSZ).x > (112*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (135*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (160*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (168*pix_h/416)) {
            WONSZ.draw(downsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (112*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (135*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (160*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (168*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            flagi.dzwiek =! flagi.dzwiek;
            if (flagi.dzwiek) {
                dzwiek01 = "TAK";
            } else {
                dzwiek01 = "NIE";
            }
            opcja1.setString(dzwiek01);

            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        downsprite.setPosition(332, 208);
        WONSZ.draw(downsprite);
        if (sf::Mouse::getPosition(WONSZ).x > (332*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (355*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (208*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (214*pix_h/416)) {
            WONSZ.draw(downsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (332*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (355*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (208*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (214*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            szybkosc--;
            szybmanip(szybkosc, szybk);
            opcja2.setString(szybk);
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        downsprite.setPosition(341, 304);
        WONSZ.draw(downsprite);
        if (sf::Mouse::getPosition(WONSZ).x > (341*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (364*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (304*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (310*pix_h/416)) {
            WONSZ.draw(downsprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (341*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (364*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (304*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (310*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            oddzielne =! oddzielne;
            if (oddzielne) {
                odd_stan = "ODDZIELNE ZYCIA: TAK";
            } else {
                odd_stan = "ODDZIELNE ZYCIA: NIE";
            }
            opcja3.setString(odd_stan);
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        if (sf::Mouse::getPosition(WONSZ).x > (230*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (336*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (236*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (252*pix_h/416)) {
            WONSZ.draw(opcjeranksprite);
        }
        if (sf::Mouse::getPosition(WONSZ).x > (230*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (336*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (236*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (252*pix_h/416) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            reset = true;
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                continue;
            }
        }
        if (reset) {
            WONSZ.draw(resetsprite);
        }

        while (WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (flagi.dzwiek) {
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
                Sleep(10);
                continue;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T && reset) {
                ofstream ranks;
                ranks.open("./Data/ranking.txt", ios::out);
                if(ranks.good()) {
                    ranks << "-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 null null null null null null null null null null                                                                                                    ";
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
        Sleep(10);
    }

    szybwyp(szybkosc);

    ofstream plikdzwiek2;
    plikdzwiek2.open("./Data/sound.txt", ios::out);
    if (plikdzwiek2.good()) {
        if (flagi.dzwiek) {
            plikdzwiek2 << '1';
        } else {
            plikdzwiek2<<'0';
        }
        plikdzwiek2.close();
    }

    ofstream plikszybkosc2;
    plikszybkosc2.open("./Data/sen.txt", ios::out);
    if(plikszybkosc2.good()) {
        plikszybkosc2 << szybkosc;
        plikszybkosc2 << ' ';
        plikszybkosc2.close();
    }

    ofstream tog_sep2;
    tog_sep2.open("./Data/tog-sep.txt", ios::out);
    if(tog_sep2.good()) {
        tog_sep2 << oddzielne;
        tog_sep2 << ' ';
        tog_sep2.close();
    }

    szybwczyt(szybkosc, szybk);
}

void intro(sf::Texture &menu, sf::Font &font, sf::SoundBuffer &tlobuffer, sf::Texture &pomoctexture, sf::Event &introevent)
{
    int wonszintr = 128;

    sf::SoundBuffer introbuffer;
	introbuffer.loadFromFile("./Data/intro.wav");
	sf::Sound introsnd;
	introsnd.setBuffer(introbuffer);

    sf::Texture intro;
    intro.loadFromFile("./Data/intro.png");
    sf::Sprite introsprite;
    introsprite.setTexture(intro);
    introsprite.setPosition(72, 132);

    sf::Texture wonszintro;
    sf::Sprite wonszintrosprite;

    WONSZ.clear(sf::Color(0, 0, 0));
    WONSZ.draw(introsprite);
    WONSZ.display();

    intro.loadFromFile("./Data/intro2.png");
    introsprite.setTexture(intro);
    wonszintro.loadFromFile("./Data/introwonsz.png");
    wonszintrosprite.setTexture(wonszintro);
    wonszintrosprite.setPosition(wonszintr, 132);

    Sleep(1000);

    if (flagi.dzwiek) {
        introsnd.play();
    }
    for (int xd = 0; xd < 35; xd++) {
        while(WONSZ.pollEvent(introevent)) {
            if (introevent.type == sf::Event::Closed) {
                if (flagi.dzwiek) {
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
                introsnd.play();
            }
        }
        if (flagi.pauza) {
            Sleep(10);
            xd--;
            continue;
        }
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(introsprite);
        WONSZ.draw(wonszintrosprite);
        WONSZ.display();
        wonszintr += 8;
        wonszintrosprite.setPosition(wonszintr, 132);
        Sleep(100);
    }
    menu.loadFromFile("./Data/menu.png");
    font.loadFromFile("./Data/TerminalVector.ttf");
	tlobuffer.loadFromFile("./Data/tlo.wav");
    pomoctexture.loadFromFile("./Data/pomoc.png");
    Sleep(300);
}

void ranking(sf::Event event, sf::Sound tlo)
{
    sf::Font font;
    font.loadFromFile("./Data/TerminalVector.ttf");

    sf::Texture ranking;
    ranking.loadFromFile("./Data/ranking.png");
    sf::Sprite rankingsprite;
    rankingsprite.setTexture(ranking);
    rankingsprite.setPosition(178, 24);

    int haj[10];
    string hajskor[10];
    string nejm[10];

    ifstream plik;
    plik.open("./Data/ranking.txt", ios::in);
    if (plik.good()) {
        for (int i = 0; i < 10; i++) {
            plik >> haj[i];
        }
        for (int i = 0; i < 10; i++) {
            plik >> nejm[i];
        }
    }
    plik.close();

    int decimal = 10;
    string reszta;

    for (int i = 0; i < 10; i++) {
        if (haj[i] == -1) {
            hajskor[i] = "-1";
            continue;
        }
        while (haj[i] != 0) {
            switch ((haj[i] % decimal) / (decimal/10)) {
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
            hajskor[i] = reszta + hajskor[i];
            haj[i] -= (haj[i] % decimal);
            decimal = decimal * 10;
        }
        if (decimal == 100) {
            hajskor[i] = "0000" + hajskor[i];
        }
        if (decimal == 1000) {
            hajskor[i] = "000" + hajskor[i];
        }
        if (decimal == 10000) {
            hajskor[i] = "00" + hajskor[i];
        }
        if (decimal == 100000) {
            hajskor[i] = "0" + hajskor[i];
        }
        decimal = 10;
    }

    int lenght[10];
    for (int i = 0; i < 10; i++) {
        lenght[i] = (292 - (32 + (nejm[i].length() * 8) + hajskor[i].length() * 8) / 2);
    }

    sf::String frst;
    if (hajskor[0] == "-1") {
        frst = "1. -----";
        lenght[0] = 260;
    } else {
        frst = "1. " + nejm[0] + " " + hajskor[0];
    }

    sf::Text first(frst, font);
    first.setPosition(lenght[0], 132);
    first.setCharacterSize(12);
    first.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String scnd;
    if (hajskor[1] == "-1") {
        scnd = "2. -----";
        lenght[1] = 260;
    } else {
        scnd = "2. " + nejm[1] + " " + hajskor[1];
    }

    sf::Text second(scnd, font);
    second.setPosition(lenght[1], 156);
    second.setCharacterSize(12);
    second.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String thrd;
    if (hajskor[2] == "-1") {
        thrd = "3. -----";
        lenght[2] = 260;
    } else {
        thrd = "3. " + nejm[2] + " " + hajskor[2];
    }

    sf::Text third(thrd, font);
    third.setPosition(lenght[2], 180);
    third.setCharacterSize(12);
    third.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String frth;
    if (hajskor[3] == "-1") {
        frth = "4. -----";
        lenght[3] = 260;
    } else {
        frth = "4. " + nejm[3] + " " + hajskor[3];
    }

    sf::Text fourth(frth, font);
    fourth.setPosition(lenght[3], 204);
    fourth.setCharacterSize(12);
    fourth.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String ffth;
    if (hajskor[4] == "-1") {
        ffth = "5. -----";
        lenght[4] = 260;
    } else {
        ffth = "5. " + nejm[4] + " " + hajskor[4];
    }

    sf::Text fifth(ffth, font);
    fifth.setPosition(lenght[4], 228);
    fifth.setCharacterSize(12);
    fifth.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String sxth;
    if (hajskor[5] == "-1") {
        sxth = "6. -----";
        lenght[5] = 260;
    } else {
        sxth = "6. " + nejm[5] + " " + hajskor[5];
    }

    sf::Text sixth(sxth, font);
    sixth.setPosition(lenght[5], 252);
    sixth.setCharacterSize(12);
    sixth.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String svnth;
    if (hajskor[6] == "-1") {
        svnth = "6. -----";
        lenght[6] = 260;
    } else {
        svnth = "7. " + nejm[6] + " " + hajskor[6];
    }

    sf::Text seventh(svnth, font);
    seventh.setPosition(lenght[6], 276);
    seventh.setCharacterSize(12);
    seventh.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String ghth;
    if (hajskor[7] == "-1") {
        ghth = "8. -----";
        lenght[7] = 260;
    } else {
        ghth = "8. " + nejm[7] + " " + hajskor[7];
    }

    sf::Text eighth(ghth, font);
    eighth.setPosition(lenght[7], 300);
    eighth.setCharacterSize(12);
    eighth.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String nnth;
    if (hajskor[8] == "-1") {
        nnth = "9. -----";
        lenght[8] = 260;
    } else {
        nnth = "9. " + nejm[8] + " " + hajskor[8];
    }

    sf::Text ninth(nnth, font);
    ninth.setPosition(lenght[8], 324);
    ninth.setCharacterSize(12);
    ninth.setFillColor(sf::Color(255, 255, 255, 180));

    sf::String tnth;
    if (hajskor[9] == "-1") {
        tnth = "10. -----";
        lenght[9] = 256;
    } else {
        tnth = "10. " + nejm[9] + " " + hajskor[9];
        lenght[9]++;
    }

    sf::Text tenth(tnth, font);
    tenth.setPosition(lenght[9], 348);
    tenth.setCharacterSize(12);
    tenth.setFillColor(sf::Color(255, 255, 255, 180));

    bool ranking_koniec = false;

    while (!ranking_koniec) {
        WONSZ.clear(sf::Color(0, 0, 0));
        WONSZ.draw(rankingsprite);
        WONSZ.draw(first);
        WONSZ.draw(second);
        WONSZ.draw(third);
        WONSZ.draw(fourth);
        WONSZ.draw(fifth);
        WONSZ.draw(sixth);
        WONSZ.draw(seventh);
        WONSZ.draw(eighth);
        WONSZ.draw(ninth);
        WONSZ.draw(tenth);
        WONSZ.display();
        while(WONSZ.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (flagi.dzwiek) {
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
                Sleep(10);
                continue;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                ranking_koniec = true;
            }
        }
        Sleep(10);
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
                if (flagi.dzwiek) {
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
                Sleep(10);
                continue;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                pomoc_koniec = true;
            }
        }
        Sleep(10);
    }
}

int main()
{
    sf::Image wonsz_ico;
	if (wonsz_ico.loadFromFile("./Data/icon.png")) {
        WONSZ.setIcon(64, 64, wonsz_ico.getPixelsPtr());
	}

    sf::Texture menu;
    sf::Font font;
    sf::SoundBuffer tlobuffer;
    sf::Texture pomoctexture;
    sf::Event event;

	sf::String dzwiek01;
    ifstream plikdzwiek("./Data/sound.txt", ios::in);
    if (plikdzwiek.good()) {
        plikdzwiek >> flagi.dzwiek;
        plikdzwiek.close();
    } else {
        ;///error
    }

    if (flagi.dzwiek) {
        dzwiek01 = "TAK";
    }
    else {
        dzwiek01 = "NIE";
    }

                                                            ///INTRO

    intro(menu, font, tlobuffer, pomoctexture, event);


    sf::Sprite menusprite;
    menusprite.setTexture(menu);
    menusprite.setPosition(136, 48);

    sf::Sprite pomocsprite;
    pomocsprite.setTexture(pomoctexture);
    pomocsprite.setPosition(40, 36);


    sf::Text przycisk1("GRA", font);
    przycisk1.setPosition(280, 190);
    przycisk1.setCharacterSize(12);
    przycisk1.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text przycisk2("RANKING", font);
    przycisk2.setPosition(264, 226);
    przycisk2.setCharacterSize(12);
    przycisk2.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text przycisk3("OPCJE", font);
    przycisk3.setPosition(272, 262);
    przycisk3.setCharacterSize(12);
    przycisk3.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text przycisk4("POMOC", font);
    przycisk4.setPosition(272, 298);
    przycisk4.setCharacterSize(12);
    przycisk4.setFillColor(sf::Color(255, 255, 255, 180));

    sf::Text przycisk5(L"WYJŚCIE", font);
    przycisk5.setPosition(264, 334);
    przycisk5.setCharacterSize(12);
    przycisk5.setFillColor(sf::Color(255, 255, 255, 180));

	sf::Sound tlo;
	tlo.setBuffer(tlobuffer);
	tlo.setLoop(true);

    ifstream dev_file("dev", ios::in);
    if (dev_file.good()) {
        dev_file >> flagi.dev;
        dev_file.close();
    } else {
        flagi.dev = false;
    }

    int szybkosc;
    sf::String szybk;
    ifstream plikszybkosc;
    plikszybkosc.open("./Data/sen.txt", ios::in);
    if(plikszybkosc.good() == true) {
        plikszybkosc >> szybkosc;
        plikszybkosc.close();
    }
    szybwczyt(szybkosc, szybk);

    int esc_lvl = 1;
    int place = 0;

    if (flagi.koniec) {
        WONSZ.close();
        return 0;
    }

    if (flagi.dzwiek) {
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
                tlo.play();
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
            if (flagi.dzwiek == true) {
                tlo.stop();
            }
            WONSZ.close();
        }
        if (esc_lvl == 1) {
            place = 0;
        }

        if (flagi.pauza) {
            Sleep(10);
            continue;
        }

        switch (place) {
            case 0: {
                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (180*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (210*pix_h/416)) {
                    przycisk1.setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk1.setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (215*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (245*pix_h/416)) {
                    przycisk2.setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk2.setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (255*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (285*pix_h/416)) {
                    przycisk3.setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk3.setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (290*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (320*pix_h/416)) {
                    przycisk4.setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk4.setFillColor(sf::Color(255, 255, 255, 180));
                }

                if (sf::Mouse::getPosition(WONSZ).x > (250*pix_w/584) && sf::Mouse::getPosition(WONSZ).x < (330*pix_w/584) && sf::Mouse::getPosition(WONSZ).y > (325*pix_h/416) && sf::Mouse::getPosition(WONSZ).y < (355*pix_h/416)) {
                    przycisk5.setFillColor(sf::Color(255, 255, 255));
                } else {
                    przycisk5.setFillColor(sf::Color(255, 255, 255, 180));
                }

                WONSZ.clear(sf::Color(0, 0, 0));
                WONSZ.draw(menusprite);
                WONSZ.draw(przycisk1);
                WONSZ.draw(przycisk2);
                WONSZ.draw(przycisk3);
                WONSZ.draw(przycisk4);
                WONSZ.draw(przycisk5);
                WONSZ.display();
                break;
            }
            case 1: {
                WONSZ.clear(sf::Color(0, 0, 0));
                if (flagi.dzwiek) {
                    tlo.stop();
                }
                griel(event);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
                }
                esc_lvl--;
                if (flagi.dzwiek) {
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
                opcje(dzwiek01, font, szybk, szybkosc, event, tlo);
                if (flagi.koniec) {
                    WONSZ.close();
                    return 0;
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
        Sleep(10);
    }
    return 0;
}
