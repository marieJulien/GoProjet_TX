#ifndef FENETREJEU_H
#define FENETREJEU_H

#include "FenetrePrincipale.h"
class GobanIA;

class FenetreJeu : public FenetrePrincipale
{
    Q_OBJECT

public :
    FenetreJeu();
    void init();
    void tourIA();
    boost::shared_ptr<FenetreJeu> sharedFromThis_Jeu()
    {
        std::cout << "OOOOOOOOOO";
        return  boost::shared_dynamic_cast<FenetreJeu,FenetrePrincipale>(shared_from_this());
    }

public slots :
    void bouton_goban(int a, int o);
    void abandon();
    void passer();


private :
    boost::shared_ptr<GobanIA> m_goban;
    QPushButton* resignButton;
    QPushButton* passButton;
};

#endif // FENETREJEU_H
