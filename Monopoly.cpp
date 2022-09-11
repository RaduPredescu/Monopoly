#include<bits/stdc++.h>

using namespace std;

class Proprietate;

class IJucator {
public:
    virtual void cumpara(Proprietate* proprietate) = 0;

    virtual int platesteChirie(int bani) = 0;

    virtual void primesteChirie(int bani) = 0;

    virtual vector<struct Proprietate*>& getProprietati() = 0;
};

class Camp {
public:
    virtual ~Camp() {

    }
};

class CampLiber : public Camp {
};

class Proprietate : public Camp {
protected:
    string nume;
    int cost;
    int chirie;
    IJucator* proprietar;

    virtual int calculeazaChirie() = 0;

public:
    Proprietate(const string& nume, int cost, int chirie) :
        nume(nume), cost(cost), chirie(chirie), proprietar(nullptr) {}

    int getCost() const {
        return cost;
    }

    void picaJucator(IJucator* jucator) {
        if (proprietar == nullptr) {
            jucator->cumpara(this);
        }
        else if (jucator != proprietar) {
            int plata = calculeazaChirie();
            int plataPosibila = jucator->platesteChirie(plata);
            proprietar->primesteChirie(plataPosibila);
        }
    }

    void elibereaza() {
        proprietar = nullptr;
    }

    const string& getNume() const {
        return nume;
    }

    void adaugaProprietar(IJucator* jucator) {
        proprietar = jucator;
    }
};

class Normala : public Proprietate {
public:
    Normala(const string& nume, int cost, int chirie) : Proprietate(nume, cost, chirie) {}

protected:
    int calculeazaChirie() override {
        return chirie;
    }
};

class Birou : public Proprietate {
public:
    Birou(const string& nume, int cost, int chirie) : Proprietate(nume, cost, chirie) {}

protected:
    int calculeazaChirie() override {
        int taxa = 0;
        for (auto proprietate : proprietar->getProprietati()) {
            if (auto birou = dynamic_cast<Birou*>(proprietate)) {
                taxa += birou->chirie;
            }
        }
        return taxa;
    }
};

class Rezidential : public Proprietate {
private:
    string numeComplex;
public:
    Rezidential(const string& nume, int cost, int chirie, const string& numeComplex) : Proprietate(nume, cost, chirie),
        numeComplex(numeComplex) {}

    const string& getNumeComplex() const {
        return numeComplex;
    }

protected:
    int calculeazaChirie() override {
        int taxa = 0;
        for (auto proprietate : proprietar->getProprietati()) {
            if (auto rezidential = dynamic_cast<Rezidential*>(proprietate)) {
                if (rezidential->getNumeComplex() == getNumeComplex()) {
                    taxa += rezidential->chirie;
                }
            }
        }
        return taxa;
    }
};

class Jucator : public IJucator {
protected:
    string nume;
    int baniTura;
    vector<Proprietate*> proprietati;
    int bani;
    vector<Camp*>& tabla;
    int pozitie;
public:
    Jucator(const string& nume, int baniTura, vector<Camp*>& tabla) :
        nume(nume), baniTura(baniTura), tabla(tabla), bani(0), pozitie(0) {}

    int platesteChirie(int baniC) override {
        if (bani >= baniC) {
            bani -= baniC;
            return baniC;
        }
        else {
            for (auto proprietate : proprietati)
                proprietate->elibereaza();
            proprietati.clear();
            baniC = bani;
            bani = -1;
            return baniC;
        }
    }

    void primesteChirie(int baniC) override {
        bani += baniC;
    }

    bool maiJoaca() {
        return bani >= 0;
    }

    void miscare(int zar) {
        bani += baniTura;
        pozitie = (pozitie + zar) % tabla.size();
        if (auto proprietate = dynamic_cast<Proprietate*>(tabla[pozitie])) {
            proprietate->picaJucator(this);
        }
    }

    vector<Proprietate*>& getProprietati() override {
        return proprietati;
    }

    const string& getNume() const {
        return nume;
    }

    int getBani() const {
        return bani;
    }
};

class Normal : public Jucator {
public:
    Normal(const string& nume, int baniTura, vector<Camp*>& tabla) : Jucator(nume, baniTura, tabla) {}

    void cumpara(Proprietate* proprietate) override {
        if (proprietate->getCost() <= bani) {
            bani -= proprietate->getCost();
            proprietati.push_back(proprietate);
            proprietate->adaugaProprietar(this);
        }
    }
};

class Avar : public Jucator {
public:
    Avar(const string& nume, int baniTura, vector<Camp*>& tabla) : Jucator(nume, baniTura, tabla) {}

    void cumpara(Proprietate* proprietate) override {

    }
};

class Investitor : public Jucator {
public:
    Investitor(const string& nume, int baniTura, vector<Camp*>& tabla) : Jucator(nume, baniTura, tabla) {}

    void cumpara(Proprietate* proprietate) override {
        if (proprietate->getCost() <= bani)
            if (auto birou = dynamic_cast<Birou*>(proprietate)) {
                bani -= proprietate->getCost();
                proprietati.push_back(proprietate);
                proprietate->adaugaProprietar(this);
            }
    }
};

class Dezvoltator : public Jucator {
private:
    int numarComplexe;
    vector<string> numeComplexe;
public:
    Dezvoltator(const string& nume, int baniTura, vector<Camp*>& tabla, int numarComplexe) : Jucator(nume, baniTura,
        tabla),
        numarComplexe(
            numarComplexe) {}

    void cumpara(Proprietate* proprietate) override {
        if (proprietate->getCost() <= bani)
            if (auto rezidential = dynamic_cast<Rezidential*>(proprietate)) {
                bool vaCumpara = false;
                for (auto numeComplex : numeComplexe) {
                    if (rezidential->getNumeComplex() == numeComplex) {
                        vaCumpara = true;
                    }
                }
                if (!vaCumpara && numarComplexe > numeComplexe.size()) {
                    numeComplexe.push_back(rezidential->getNumeComplex());
                    vaCumpara = true;
                }
                if (vaCumpara) {
                    bani -= proprietate->getCost();
                    proprietati.push_back(proprietate);
                    proprietate->adaugaProprietar(this);
                }
            }
    }
};

int main() {
    string fisier1, fisier2, fisier3;
    cin >> fisier1 >> fisier2 >> fisier3;

    ifstream inTabla(fisier1);
    ifstream inJucatori(fisier2);
    ifstream inMutari(fisier3);

    string linie;
    vector<Camp*> tabla;
    while (getline(inTabla, linie)) {
        stringstream inText(linie);
        string tipCamp;
        inText >> tipCamp;

        if (tipCamp == "LIBER") {
            tabla.push_back(new CampLiber());
        }
        else if (tipCamp == "PROPRIETATE") {
            string nume, tip;
            int cost, chirie;
            inText >> nume >> cost >> chirie >> tip;
            if (tip == "NORMAL") {
                tabla.push_back(new Normala(nume, cost, chirie));
            }
            else if (tip == "BIROU") {
                tabla.push_back(new Birou(nume, cost, chirie));
            }
            else if (tip == "REZIDENTIAL") {
                string numeComplex;
                inText >> numeComplex;
                tabla.push_back(new Rezidential(nume, cost, chirie, numeComplex));
            }
        }
    }

    vector<Jucator*> jucatori;
    while (getline(inJucatori, linie)) {
        stringstream inText(linie);
        string nume, tip;
        int baniPerTura;

        inText >> nume >> baniPerTura >> tip;

        if (tip == "AVAR") {
            jucatori.push_back(new Avar(nume, baniPerTura, tabla));
        }
        else if (tip == "NORMAL") {
            jucatori.push_back(new Normal(nume, baniPerTura, tabla));
        }
        else if (tip == "INVESTITOR") {
            jucatori.push_back((new Investitor(nume, baniPerTura, tabla)));
        }
        else if (tip == "DEZVOLTATOR") {
            int numarComplexe;
            inText >> numarComplexe;
            jucatori.push_back(new Dezvoltator(nume, baniPerTura, tabla, numarComplexe));
        }
    }

    int indiceJucator = 0;
    int zar;

    while (inMutari >> zar) {
        jucatori[indiceJucator]->miscare(zar);
        indiceJucator = (indiceJucator + 1) % jucatori.size();
        while (!jucatori[indiceJucator]->maiJoaca())
            indiceJucator = (indiceJucator + 1) % jucatori.size();
    }

    sort(jucatori.begin(), jucatori.end(), [](auto& left, auto& right) {
        return left->getBani() == right->getBani()
            ? left->getNume() < right->getNume()
            : left->getBani() > right->getBani();
        });

    for (auto jucator : jucatori) {
        cout << jucator->getNume() << " " << jucator->getBani() << endl;
        sort(jucator->getProprietati().begin(), jucator->getProprietati().end(),
            [](auto& left, auto& right) {
                return left->getNume() < right->getNume();
            });
        for (auto proprietati : jucator->getProprietati())
            cout << proprietati->getNume() << " ";
        cout << endl;
    }

    return 0;
}