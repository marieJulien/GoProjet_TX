#include "ResultatPartie.hpp"
#include "../GobanFiles/GobanIA.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ResultatPartie::ResultatPartie(boost::shared_ptr<FenetreJeu> fenetreJeu) : QWidget()
{
    std::cout << "eeeeeeeeeee";
    if (fenetreJeu.get()==0) std::cout << "NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n\n";
    boost::shared_ptr<GobanIA> goban = boost::dynamic_pointer_cast<GobanIA>(fenetreJeu->getGoban());

    std::cout << "aaaaaaaaaaa";

    QVBoxLayout* layoutVertical = new QVBoxLayout();
    QHBoxLayout* layoutBoutons = new QHBoxLayout();
    std::cout << "oooooooooooo";
    QLabel* text = new QLabel(QString::fromStdString(goban->getPartieIA()->resultat(goban)));
    QPushButton* boutonQuitter = new QPushButton("Quitter");
    QPushButton* boutonRecommencer = new QPushButton("Rejouer");

    std::cout << "iiiiiiiiii";

    connect(boutonQuitter,SIGNAL(clicked()),this,SLOT(quitter()));
    connect(boutonRecommencer,SIGNAL(clicked()),this,SLOT(recommencer()));

    layoutBoutons->addWidget(boutonRecommencer);
    layoutBoutons->addWidget(boutonQuitter);

    layoutVertical->addWidget(text);
    layoutVertical->addLayout(layoutBoutons);

    this->setLayout(layoutVertical);

    //this->show;
}

void ResultatPartie::quitter()
{

}

void ResultatPartie::recommencer()
{

}
