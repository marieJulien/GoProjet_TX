#include "ResultatPartie.hpp"
#include "../GobanFiles/GobanIA.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ResultatPartie::ResultatPartie() : QWidget()
{
    layoutVertical = new QVBoxLayout();
    layoutBoutons = new QHBoxLayout();
    boutonQuitter = new QPushButton("Quitter");
    boutonRecommencer = new QPushButton("Rejouer");

    connect(boutonQuitter,SIGNAL(clicked()),this,SLOT(quitter()));
    connect(boutonRecommencer,SIGNAL(clicked()),this,SLOT(recommencer()));

    layoutBoutons->addWidget(boutonRecommencer);
    layoutBoutons->addWidget(boutonQuitter);

    layoutVertical->addLayout(layoutBoutons);

    setLayout(layoutVertical);

    std::cout << "Fenêtre de résultat créée\n";
}

void ResultatPartie::init(boost::shared_ptr<FenetreJeu> fenetreJeu)
{
    try
    {
        if (fenetreJeu.get()==0) std::cout << "ERREUR : POINTEUR NUL SUR FENETRE JEU ....\n\n";
        std::cout << "Initialisation de la fenêtre de résultat\n";
        boost::shared_ptr<Goban> gob = fenetreJeu->getGoban();
        if (gob.get()==0) std::cout << "POINTEUR SUR GOBAN NUL\n"; //plante ici ...
        boost::shared_ptr<GobanIA> goban = boost::dynamic_pointer_cast<GobanIA>(gob);
        std::cout << "Calcul du résultat :\n";
        if (goban.get()==0) std::cout << "ERREUR : POINTEUR SUR GOBAN NUL DANS FENETRE JEU\n\n";
        boost::shared_ptr<PartieIA> partie = goban->getPartieIA();
        if (partie.get()==0) std::cout << "ERREUR : POINTEUR SUR PARTIE IA NUL DANS GOBAN\n\n";
        QLabel* text = new QLabel(QString::fromStdString(partie->resultat(goban)));
        std::cout << "Calcul ok\n";
        layoutVertical->insertWidget(0,text);
    }
    catch(std::exception const& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void ResultatPartie::quitter()
{

}

void ResultatPartie::recommencer()
{

}
