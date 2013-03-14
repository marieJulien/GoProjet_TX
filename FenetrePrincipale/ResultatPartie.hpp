#ifndef RESULTATPARTIE_HPP
#define RESULTATPARTIE_HPP

#include "../Partie/PartieIA.h"
#include "FenetreJeu.h"

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QButtonGroup>



class ResultatPartie : public QWidget
{
    Q_OBJECT

public :
    ResultatPartie(boost::shared_ptr<FenetreJeu> fenetreJeu);

public slots :
    void quitter();
    void recommencer();
};


#endif // RESULTATPARTIE_HPP
